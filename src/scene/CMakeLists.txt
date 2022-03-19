add_library(scene
  sceneFactory.cpp
)

target_include_directories(scene
  PUBLIC ../include
  PUBLIC ../../include
)