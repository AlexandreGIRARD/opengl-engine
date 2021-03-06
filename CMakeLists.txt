cmake_minimum_required(VERSION 3.15)
project(engine)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -flto -march=native")
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/stb)

add_executable(engine
    src/main.cc
    src/glad.c
    src/program.cc
    src/camera.cc
    src/mesh.cc
    src/model.cc
    src/light.cc
    src/directional_light.cc
    src/point_light.cc
    src/deferred.cc
    src/water.cc
    src/util.cc
    src/normal_material.cc
    src/textured_material.cc
    src/skybox.cc
    src/scene.cc
    src/ambient_occlusion.cc
    src/boids.cc
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_impl_glfw.cpp
    )
target_compile_definitions(engine PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(engine glfw OpenGL::GL dl assimp)
