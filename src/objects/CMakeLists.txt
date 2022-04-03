add_library(objects
  object.cpp
  box.cpp
  line.cpp
  skybox.cpp
  cube.cpp
  plane.cpp
)

target_include_directories(objects
  PUBLIC ../include
  PUBLIC ../../include
)

