#pragma once

#include "glm/glm.hpp"
#include "shader_s.h"
#include "object.h"
#include "box.h"

class Cube : public Box
{
  public:

    Cube(glm::vec3 position, float scale, const char* name, float mass = 1.0f);
};
