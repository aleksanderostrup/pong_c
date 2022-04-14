add_library(collisions
  collisionDetector.cpp
)

target_include_directories(collisions
  PUBLIC ../include
  PUBLIC ../../include
)