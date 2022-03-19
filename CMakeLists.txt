cmake_minimum_required(VERSION 3.16.0)
project(phys_engine VERSION 0.1.0)


# include(CTest)
# enable_testing()

link_directories(
    /usr/lib/x86_64-linux-gnu/
    /home/alek/eclipse-workspace/pong_3d/shared
)

add_subdirectory(src)



# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
