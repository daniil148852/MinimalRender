package com.renderer.minimal

import android.os.Bundle
import android.os.Environment
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import java.io.File

class MainActivity : AppCompatActivity() {
    
    companion object {
        init {
            System.loadLibrary("gl_wrapper")
        }
    }
    
    private external fun initRenderer(cachePath: String): Boolean
    private external fun getGLVersion(): String
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        
        // Создать директории
        val baseDir = File(Environment.getExternalStorageDirectory(), "MinimalGL")
        val cacheDir = File(baseDir, "cache")
        cacheDir.mkdirs()
        
        // Записать config.json для лаунчеров
        val config = File(baseDir, "config.json")
        config.writeText("""
            {
                "name": "MinimalGL",
                "version": "1.0",
                "library": "libgl_wrapper.so",
                "gl_version": "3.2",
                "features": ["shader_cache", "batch_draw"]
            }
        """.trimIndent())
        
        // Скопировать .so в доступное место
        copyNativeLib(baseDir)
        
        findViewById<TextView>(R.id.statusText).text = 
            "MinimalGL Renderer установлен!\n\nПуть: ${baseDir.absolutePath}"
    }
    
    private fun copyNativeLib(destDir: File) {
        val libDir = File(destDir, "lib")
        libDir.mkdirs()
        
        listOf("arm64-v8a", "armeabi-v7a").forEach { abi ->
            val abiDir = File(libDir, abi)
            abiDir.mkdirs()
            
            try {
                val nativeDir = File(applicationInfo.nativeLibraryDir)
                File(nativeDir, "libgl_wrapper.so").copyTo(
                    File(abiDir, "libgl_wrapper.so"), overwrite = true
                )
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }
}
