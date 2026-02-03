// app/src/main/java/com/fcl/plugin/minimalrenderer/MainActivity.kt (Optional for config UI)
package com.fcl.plugin.minimalrenderer

import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // Minimal UI: e.g., button to select folder for config.json / shader cache
        Toast.makeText(this, "Select folder for renderer config", Toast.LENGTH_LONG).show()
        // Implement file picker here (e.g., Storage Access Framework)
        // For super minimal: just finish() after toast
        finish()
    }
}
