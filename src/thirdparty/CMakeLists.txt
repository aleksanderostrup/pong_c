add_library(thirdparty
  glad.c
  std_image.cpp
)

target_include_directories(thirdparty
    PUBLIC include/glm/
)
