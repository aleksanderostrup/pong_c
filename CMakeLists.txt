cmake_minimum_required(VERSION 3.16.0)
project(phys_engine VERSION 0.1.0)


# include(CTest)
# enable_testing()

link_directories(
    /usr/lib/x86_64-linux-gnu/
    /home/alek/eclipse-workspace/pong_3d/shared
)

add_library(external_dependencies
    src/std_image.cpp
)

add_executable(phys_engine 
    src/glad.c
    src/box.cpp
    src/line.cpp
    src/skybox.cpp
    src/cube.cpp
    src/plane.cpp
    src/main.cpp
    src/inputprocess.cpp
    src/glfw_setup.cpp
    src/cmdInterpreter.cpp
    src/sceneFactory.cpp
    src/textureManager.cpp
)

target_include_directories(phys_engine
    PUBLIC include/
    PUBLIC include/assimp/
    PUBLIC /usr/include/
    PUBLIC /usr/include/
)

target_include_directories(external_dependencies
    PUBLIC include/glm/
)

target_compile_options(phys_engine
    PRIVATE "-Wall"    
)

target_link_libraries(phys_engine
    glfw
    X11
    Xi
    dl
    GL
    assimp
    pthread
    Xrandr
    external_dependencies
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
