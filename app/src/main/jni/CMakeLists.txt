# app/src/main/jni/CMakeLists.txt
cmake_minimum_required(VERSION 3.22.1)

project(minimalrenderer)

# Include directories (add GLM, SPIRV-Cross etc. as submodules later)
include_directories(include)

# Source files for OpenGL → GLES wrapper
add_library(minimalrenderer SHARED
    src/main.cpp
    src/gl_wrapper.cpp
    # Add more: shader_cache.cpp, draw_optimizer.cpp etc.
)

# Link GLESv3, log etc.
target_link_libraries(minimalrenderer
    GLESv3
    android
    log
    # Later: vulkan, angle for Vulkan backend
)
