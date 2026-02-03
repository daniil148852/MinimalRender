cmake_minimum_required(VERSION 3.22.1)
project("gl_wrapper")

add_library(gl_wrapper SHARED gl_wrapper.cpp)

find_library(log-lib log)
find_library(gles3-lib GLESv3)
find_library(egl-lib EGL)

target_link_libraries(gl_wrapper
    ${log-lib}
    ${gles3-lib}
    ${egl-lib}
)
