#pragma once

#include "glm/glm.hpp"
#include "camera.h"
#include "shader_s.h"
#include "object.h"

class Skybox/*  : public Object */
{
  public:

    Skybox(Camera const& camera, uint32_t const& width, uint32_t const& height);
    ~Skybox();

    void DrawInit();
    void Draw();

  private:
    Shader*             mShader;
    unsigned int        mCubemapTexture;
    Camera const&       mCamera;
    uint32_t const&     mHeight;
    uint32_t const&     mWidth;
};
