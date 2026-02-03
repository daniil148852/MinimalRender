#include <jni.h>
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <android/log.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>

#define LOG_TAG "MinimalGL"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ===== Shader Cache для FPS оптимизации =====
static std::unordered_map<std::string, std::vector<uint8_t>> g_shaderCache;
static std::string g_cachePath = "/sdcard/MinimalGL/cache/";

// Сохранение бинарного шейдера
void cacheShaderBinary(GLuint program, const std::string& key) {
    GLint binaryLength = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
    
    if (binaryLength > 0) {
        std::vector<uint8_t> binary(binaryLength);
        GLenum format;
        glGetProgramBinary(program, binaryLength, nullptr, &format, binary.data());
        g_shaderCache[key] = binary;
        
        // Сохранить на диск
        std::ofstream file(g_cachePath + key + ".bin", std::ios::binary);
        if (file) {
            file.write(reinterpret_cast<char*>(&format), sizeof(format));
            file.write(reinterpret_cast<char*>(binary.data()), binaryLength);
        }
        LOGI("Cached shader: %s (%d bytes)", key.c_str(), binaryLength);
    }
}

// Загрузка кэшированного шейдера
bool loadCachedShader(GLuint program, const std::string& key) {
    std::string path = g_cachePath + key + ".bin";
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    
    if (!file) return false;
    
    size_t size = file.tellg();
    file.seekg(0);
    
    GLenum format;
    file.read(reinterpret_cast<char*>(&format), sizeof(format));
    
    std::vector<uint8_t> binary(size - sizeof(format));
    file.read(reinterpret_cast<char*>(binary.data()), binary.size());
    
    glProgramBinary(program, format, binary.data(), binary.size());
    
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    
    if (status == GL_TRUE) {
        LOGI("Loaded cached shader: %s", key.c_str());
        return true;
    }
    return false;
}

// ===== OpenGL → GLES Wrapper =====

// Эмуляция glBegin/glEnd через VBO (для совместимости)
static GLenum g_currentMode = 0;
static std::vector<float> g_vertexBuffer;
static bool g_immediateMode = false;

extern "C" {

// Инициализация рендерера
JNIEXPORT jboolean JNICALL
Java_com_renderer_minimal_MainActivity_initRenderer(JNIEnv* env, jobject thiz, jstring cachePath) {
    const char* path = env->GetStringUTFChars(cachePath, nullptr);
    g_cachePath = path;
    env->ReleaseStringUTFChars(cachePath, path);
    
    LOGI("MinimalGL Renderer initialized");
    LOGI("GL_VENDOR: %s", glGetString(GL_VENDOR));
    LOGI("GL_RENDERER: %s", glGetString(GL_RENDERER));
    LOGI("GL_VERSION: %s", glGetString(GL_VERSION));
    
    return JNI_TRUE;
}

// Получить версию GL
JNIEXPORT jstring JNICALL
Java_com_renderer_minimal_MainActivity_getGLVersion(JNIEnv* env, jobject thiz) {
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    return env->NewStringUTF(version ? version : "Unknown");
}

// Эмуляция glBegin (OpenGL 1.x/2.x совместимость)
void mgBegin(GLenum mode) {
    g_currentMode = mode;
    g_vertexBuffer.clear();
    g_immediateMode = true;
}

// Эмуляция glVertex3f
void mgVertex3f(float x, float y, float z) {
    if (g_immediateMode) {
        g_vertexBuffer.push_back(x);
        g_vertexBuffer.push_back(y);
        g_vertexBuffer.push_back(z);
    }
}

// Эмуляция glEnd - отрисовка через VBO
void mgEnd() {
    if (!g_immediateMode || g_vertexBuffer.empty()) return;
    
    GLuint vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, g_vertexBuffer.size() * sizeof(float), 
                 g_vertexBuffer.data(), GL_STREAM_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(g_currentMode, 0, g_vertexBuffer.size() / 3);
    
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    g_immediateMode = false;
}

// Оптимизация: Batch draw calls
static std::vector<GLuint> g_batchedVAOs;

void mgFlushBatch() {
    for (GLuint vao : g_batchedVAOs) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36); // Пример для кубов
    }
    g_batchedVAOs.clear();
}

} // extern "C"
