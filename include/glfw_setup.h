#pragma once

#include "camera.h"
#include <GLFW/glfw3.h>


GLFWwindow* glfw_init(Camera* camera, uint32_t* scr_width, uint32_t* scr_height);