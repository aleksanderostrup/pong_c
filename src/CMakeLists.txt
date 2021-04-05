cmake_minimum_required(VERSION 3.16.0)
project(pong3d VERSION 0.1.0)

# include(CTest)
# enable_testing()

link_directories(
    /usr/lib/x86_64-linux-gnu/
    /home/alek/eclipse-workspace/pong_3d/shared
)

add_library(external_dependencies
    std_image.cpp
)

add_executable(pong3d 
    glad.c
    box.cpp
    skybox.cpp
    cube.cpp
    plane.cpp
    main.cpp
    inputprocess.cpp
    glfw_setup.cpp
    cmdInterpreter.cpp
    sceneFactory.cpp
    textureManager.cpp
)

target_include_directories(pong3d
    PUBLIC ../include/
    PUBLIC ../include/assimp/
    PUBLIC /usr/include/
    PUBLIC /usr/include/
)

target_include_directories(external_dependencies
    PUBLIC ../include/glm/
)

target_compile_options(pong3d
    PRIVATE "-Wall"    
)

target_link_libraries(pong3d
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
