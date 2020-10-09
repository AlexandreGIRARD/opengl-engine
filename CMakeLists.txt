cmake_minimum_required(VERSION 3.15)
project(engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(engine
    main.cc
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
    )

target_link_libraries(engine glfw OpenGL::GL X11 pthread Xrandr Xi dl assimp)
