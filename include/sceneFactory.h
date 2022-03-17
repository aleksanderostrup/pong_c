#pragma once

#include "scene.h"
#include "shader_s.h"

class SceneFactory
{
  public:

    enum EnumScene 
    {
        kSceneTest1 = 1,
        kSceneTest2 = 2,
        kSceneTest3 = 3,
        kSceneTest4 = 4, // big box from beneath
    };
    
    SceneFactory(Camera& camera, uint32_t& width, uint32_t& height);

    Scene GetScene(EnumScene enumScene/* , Shader& shader */);

  protected:

  private:

    Camera&   mCamera;
    uint32_t& mWidth;
    uint32_t& mHeight;

};