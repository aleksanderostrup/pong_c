cmake_minimum_required(VERSION 3.16.0)
project(pong3d VERSION 0.1.0)

# include(CTest)
# enable_testing()

link_directories(
    /usr/lib/x86_64-linux-gnu/
    /home/alek/eclipse-workspace/pong_3d/shared
)

add_executable(pong3d 
    glad.c
    box.cpp
    cube.cpp
    plane.cpp
    std_image.cpp
    main.cpp)

target_include_directories(pong3d
    PUBLIC ../include/
    PUBLIC ../include/assimp/
    PUBLIC ../include/glm/
    PUBLIC /usr/include/
    PUBLIC /usr/include/
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
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
