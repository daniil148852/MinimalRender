// app/src/main/java/com/fcl/plugin/minimalrenderer/RendererService.kt
package com.fcl.plugin.minimalrenderer

import android.app.Service
import android.content.Intent
import android.os.IBinder
import android.util.Log

class RendererService : Service() {
    override fun onBind(intent: Intent?): IBinder? {
        Log.d("RendererService", "Service bound - ready for renderer init")
        // Here: Add logic to set up env vars like LD_LIBRARY_PATH to libminimalrenderer.so
        // For minimal: just return null, as meta-data handles discovery
        return null
    }
}
