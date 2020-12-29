#pragma once

#include "glm/glm.hpp"
#include "shader_s.h"
#include "object.h"

class Plane : public Object
{
  public:

    Plane(glm::vec3 position, glm::vec2 xyscale, Shader* shader, const char* name, float mass=1.0f);
    ~Plane();

    void  drawInit();
    void  draw();
    void  UpdateBoundBox();
    float containingRadius();

  private:
    Shader* shader;
};
