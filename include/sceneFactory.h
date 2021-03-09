#pragma once

#include "scene.h"
#include "../include/shader_s.h"

class SceneFactory
{
  public:

    enum EnumScene 
    {
        kSceneTest1 = 1,
        kSceneTest2 = 2,
        kSceneTest3 = 3,
    };
    
    SceneFactory();

    Scene GetScene(EnumScene enumScene, Shader& shader);

  protected:

  private:
};