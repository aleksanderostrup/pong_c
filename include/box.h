#pragma once

#include "glm/glm.hpp"
#include "shader_s.h"
#include "object.h"

class Box : public Object
{
  public:

    enum EnumTexture
    {
      kDan    = 1,
      kMetal  = 2,
    };

    Box(glm::vec3 position, glm::vec3 scale, Shader* shader, const char* name, float mass=1.0f);
    ~Box();

    void  drawInit();
    void  draw();
    void  UpdateBoundBox();
    float containingRadius();
    bool  SetTexture(EnumTexture texture);

  private:
    Shader* mShader;

    void CreateBoundBox();
};
