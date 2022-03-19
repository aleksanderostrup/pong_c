#pragma once

#include "glm/glm.hpp"
#include "camera.h"
#include "shader_s.h"
#include "object.h"

class Skybox/*  : public Object */
{
  public:

    Skybox(Camera& camera, uint32_t& width, uint32_t& height);
    ~Skybox();

    void          drawInit();
    void          draw();

  private:
    Shader* mShader;
    unsigned int mCubemapTexture;
    Camera& mCamera;
    uint32_t& mHeight;
    uint32_t& mWidth;
};
