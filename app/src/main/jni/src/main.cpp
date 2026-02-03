// app/src/main/jni/src/main.cpp (Entry point / exports)
#include <jni.h>
#include <GLES3/gl3.h>
#include <android/log.h>

#define LOG_TAG "MinimalRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Example: Export GL functions (dlopen will load this lib)
extern "C" void glClear(GLbitfield mask) {
    // Wrapper: Add FPS opts like batching checks
    glClear(mask);
    LOGI("glClear called - minimal wrapper");
}

// More GL funcs to wrap (implement full in gl_wrapper.cpp)
