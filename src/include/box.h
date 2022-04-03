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

    Box(glm::vec3 position, glm::vec3 scale, const char* name, float mass=1.0f);
    ~Box();

    void          DrawInit();
    void          Draw(Shader& shader);
    void          UpdateBoundBox();
    virtual float ContainingRadius() const;
    bool          SetTexture(EnumTexture texture);

  private:

    void CreateBoundBox();
};
