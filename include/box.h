#pragma once

#include "glm/glm.hpp"
#include "shader_s.h"
#include "object.h"

class Box : public Object
{
  public:

    Box(glm::vec3 position, glm::vec3 scale, Shader* shader, const char* name, float mass=1.0f);
    ~Box();

    void drawInit();
    void draw();
    void UpdateBoundBox();


  private:
    Shader* mShader;

    void CreateBoundBox();
};
