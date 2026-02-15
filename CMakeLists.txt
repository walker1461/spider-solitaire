cmake_minimum_required(VERSION 3.16)
project(SpiderSolitaire LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SpiderSolitaire
        src/main.cpp

        src/core/game.cpp
        src/core/layout.cpp
        src/core/path.cpp
        src/core/move_system.cpp

        src/rules/run_rules.cpp
        src/rules/spider_rules.cpp

        src/input/card_drag.cpp

        src/model/card.cpp

        src/render/render.cpp

        include/glad/glad.c

        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imgui_tables.cpp

        include/imgui/backends/imgui_impl_glfw.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp

        src/ui/main_menu.cpp
        src/ui/pause_menu.cpp
)

target_include_directories(SpiderSolitaire PRIVATE
        src
        include
        include/glad
        include/glm
        include/imgui
        include/imgui/backends
        include/KHR
)

# Dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(SpiderSolitaire PRIVATE
        glfw
        OpenGL::GL
)

# Platform specific
if (APPLE)
    target_link_libraries(SpiderSolitaire PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
endif()

# Asset installation
install(TARGETS SpiderSolitaire
        RUNTIME DESTINATION bin
)

install(DIRECTORY assets/
        DESTINATION share/SpiderSolitaire/assets
)

# CPack
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "SpiderSolitaire")
set(CPACK_PACKAGE_VENDOR "Walker Immel")
set(CPACK_PACKAGE_VERSION "0.1.0")

if (APPLE)
    set(CPACK_GENERATOR "ZIP")
elseif (WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
