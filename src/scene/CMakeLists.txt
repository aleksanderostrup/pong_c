add_library(scene
  scene.cpp
  sceneFactory.cpp
  frameHandler.cpp
)

target_include_directories(scene
  PUBLIC ../include
  PUBLIC ../../include
)