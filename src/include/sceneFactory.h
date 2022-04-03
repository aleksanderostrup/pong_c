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
        kSceneTest5 = 5, // only cubes to check for proper beheaviour
    };
    
    SceneFactory(Camera const& camera, bool const& isPaused, uint32_t const& width, uint32_t const& height);

    Scene GetScene(EnumScene enumScene/* , Shader& shader */);

  protected:

  private:
    bool      const& mIsPaused;
    Camera    const& mCamera;
    uint32_t  const& mWidth;
    uint32_t  const& mHeight;

};