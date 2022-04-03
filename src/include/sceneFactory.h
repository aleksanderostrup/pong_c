#pragma once

#include "scene.h"
#include "shader_s.h"

class SceneFactory
{
  public:

    enum class SceneId
    {
        kSceneTest1 = 1,
        kSceneTest2 = 2,
        kSceneTest3 = 3,
        kSceneTest4 = 4, // big box from beneath
        kSceneTest5 = 5, // only cubes to check for proper beheaviour
    };
    
    SceneFactory(Camera const& camera, uint32_t const& width, uint32_t const& height);

    Scene GetScene(SceneId enumScene/* , Shader& shader */);

  protected:

  private:
    Camera    const& mCamera;
    uint32_t  const& mWidth;
    uint32_t  const& mHeight;

};