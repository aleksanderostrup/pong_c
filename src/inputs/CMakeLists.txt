add_library(inputs
  inputprocess.cpp
)

target_include_directories(inputs
  PUBLIC ../include
  PUBLIC ../../include
)