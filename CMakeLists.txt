cmake_minimum_required(VERSION 3.16)
project(learnopengl)

set(CMAKE_CXX_STANDARD 17)

add_executable(learnopengl
    src/main.cpp
    src/glad.c
        src/math.h
        src/card.h
        src/card.cpp
        src/render.h
        src/render.cpp
        src/card_drag.h
        src/card_drag.cpp
)

set(GLFW_ROOT /Users/walkerimmel/programming/glfw-3.4)

target_include_directories(learnopengl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GLFW_ROOT}/include
)

target_link_libraries(learnopengl
    ${GLFW_ROOT}/build/src/libglfw3.a
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)