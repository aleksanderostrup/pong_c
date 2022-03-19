add_executable(phys_engine
  main.cpp
)

target_compile_options(phys_engine
    PRIVATE "-Wall"    
)

# TODO: set compiler to c++17

target_include_directories(phys_engine
    PUBLIC ../include/
    PUBLIC ../include/assimp/
    PUBLIC include/
    PUBLIC include/assimp/
    PUBLIC /usr/include/
    PUBLIC /usr/include/
)

add_subdirectory(commands)
add_subdirectory(objects)
add_subdirectory(inputs)
add_subdirectory(scene)
add_subdirectory(setup)
add_subdirectory(texture)
add_subdirectory(thirdparty)

target_link_libraries(phys_engine
  commands
  inputs
  scene
  setup
  texture
  thirdparty
  glfw
  objects
  X11
  Xi
  dl
  GL
  assimp
  pthread
  Xrandr
)


# target_link_libraries(phys_engine
    
    
    
    
    
    
    
    
#     external_dependencies
# )
