add_library(camera
  camera.cpp
)

target_include_directories(camera
  PUBLIC ../../include/glm/
  PUBLIC ../include
  PUBLIC ../../include
)

