add_library(scene
  scene.cpp
  sceneFactory.cpp
)

target_include_directories(scene
  PUBLIC ../include
  PUBLIC ../../include
)