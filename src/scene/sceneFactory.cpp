#include "sceneFactory.h"
#include "object.h"
#include "plane.h"
#include "box.h"
#include "cube.h"

SceneFactory::SceneFactory(Camera const& camera, uint32_t const& width, uint32_t const& height)
 :  mCamera(camera)
 ,  mWidth(width)
 ,  mHeight(height)
{
}


static inline void AddDebugObjects(Scene& scene, Shader& shader)
{
    auto* colPointBox = new Box(glm::vec3( 3.0f, 0.0f,  -3.5f), glm::vec3( 0.08f, 0.08f,  0.08f), "colPointBox");

    scene.setColPointDebugObject(colPointBox);
    std::vector<Object*> toInsert;
    std::vector<Object*> toInsert2;
    for (size_t i = 0; i < 5; i++)
    {
        std::string tmpName = "colNormalPointBox" + i;
        auto* colNormalBox = new Box(glm::vec3( 3.0f, 0.0f,  -3.5f), glm::vec3( 0.06f, 0.06f,  0.06f), tmpName.c_str());
        auto* colNormalBox2 = new Box(glm::vec3( 3.0f, 0.0f,  -3.5f), glm::vec3( 0.06f, 0.06f,  0.06f), tmpName.c_str());
        toInsert.push_back(colNormalBox);
        toInsert2.push_back(colNormalBox2);
    }
    scene.SetColNormalDebugObject(toInsert, 0);
    scene.SetColNormalDebugObject(toInsert, 1);
    // colPointBox->SetTexture(Box::kMetal); // DELETE, AND DELETE SETTEXTURE
}

static void PopulateSceneTest1(Scene& scene)
{
    // TODO: builder pattern for these objects and then add them directly
    // TODO: also, they should be pointers!
    /* Object creation */
    auto* cube1 = new  Cube (glm::vec3( 0.0f, 0.0f,  0.0f),     1.0f, "cube1", 1.0f);
    auto* cube2 = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f),    glm::vec3( 2.0f, 1.0f,  1.0f), "cube2");
    auto* cube3 = new  Cube (glm::vec3( -1.25f, 0.0f,  0.65f),  1.0f, "cube3", 1.0f);
    auto* cube4 = new  Cube (glm::vec3( -1.25f, 0.0f, -0.65f),  1.0f, "cube4", 1.0f);
    auto* cube5 = new  Cube (glm::vec3( -2.35f, 0.0f,  0.85f),  1.0f, "cube5", 1.0f);
    auto* cube6 = new  Cube (glm::vec3( -2.35f, 0.0f, -0.85f),  1.0f, "cube6", 1.0f);
    // auto* cube7 = new  Cube (glm::vec3( -2.35f, 0.0f,  1.70f),  1.0f, "cube7", 1.0f);
    // auto* cube8 = new  Cube (glm::vec3( -2.35f, 0.0f, -1.70f),  1.0f, "cube8", 1.0f);
    auto* cube9 = new  Cube (glm::vec3( 0.0f, -10.0f, 0.0f),    1.0f, "cube9", 1.0f);
    // Cube    cube12   (glm::vec3( 0.0f, 0.0f,  -4.0f), 1.0f, "cube12", 50000.0f);
    // Box     cube22   (glm::vec3( 3.0f, 0.0f,  -4.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "cube22");
    // Box     cube33   (glm::vec3( 3.0f, 0.0f,  -8.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "cube33");
    //      Cube      cube2(glm::vec3( 2.0f, 0.0f,  0.0f), 1.0f, "cube2");
    // Cube    cube10   (glm::vec3( 0.0f, 0.0f,  -3.5f), 1.0f, "cube10", 50000.0f);
    auto* plane1 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), "plane1");
    // auto* plane2 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), "plane2");

    // debug
    // dbgObj.push_back(&cube1);
    // dbgObj.push_back(&cube2);

    /* Object configuration */
    // cube1->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube4->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube6->setVelocity(glm::vec3(0.0f, 0.0f, 0.3f));
    // cube1->SetScale(glm::vec3(2.0f, 1.0f, 2.0f)); // showing that that the 'horizontal' plane is actually the xz-plane! 
    cube1->SetRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube1->SetRotation((static_cast<float>(M_PI_2 + M_PI_4)) * glm::vec3( 0.0f, 1.0f, 0.0f));
    cube2->SetRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube2->SetRotation(-0.3f * glm::vec3( 0.0f, 1.0f, 0.0f));
    // cube1>SetRotationVelocity(1.8f * glm::vec3( 0.0f, 1.0f, 1.0f));
    cube2->setVelocity(glm::vec3(-25.32f, 0.0f, 0.0f));
    cube9->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    plane1->SetRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane1->SetRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane2->SetRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));

    /* Objects added to scene */
    // scene.AddObject(plane1);
    // scene.AddObject(plane2);
    scene.AddObject(cube1);
    scene.AddObject(cube2, TextureManager::kMetal);
    scene.AddObject(cube3);
    scene.AddObject(cube4);
    scene.AddObject(cube5, TextureManager::kRedWindow);
    scene.AddObject(cube6);
    scene.AddObject(cube9);
    // scene.AddObject(cube7);
    // scene.AddObject(cube8);
    // scene.AddObject(cube3);
    // scene.AddObject(cube12);
    // scene.AddObject(cube22);
    // scene.AddObject(cube33);
    // scene.AddObject(cube10);
    // scene.AddObject(box20);
    // scene.AddObject(cube3);
    // scene.AddObject(cube4);
    // scene.AddObject(cube5);
    // scene.AddObject(cube6);
    
}

/* 
    used for trying to debug the rotation issue
 */
static void PopulateSceneTest2(Scene& scene)
{
    /* Object creation */
    auto* cube1 = new  Cube (glm::vec3( 0.0f, 0.0f,  0.0f),     1.0f, "cube1", 1.0f);
    auto* cube2 = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f),    glm::vec3( 2.0f, 1.0f,  1.0f), "cube2");
    auto* cube3 = new  Cube (glm::vec3( -1.25f, 0.0f,  0.65f),  1.0f, "cube3", 1.0f);
    // auto* cube4 = new  Cube (glm::vec3( -1.25f, 0.0f, -0.65f),  1.0f, "cube4", 1.0f);
    // auto* cube5 = new  Cube (glm::vec3( -2.35f, 0.0f,  0.85f),  1.0f, "cube5", 1.0f);
    // auto* cube6 = new  Cube (glm::vec3( -2.35f, 0.0f, -0.85f),  1.0f, "cube6", 1.0f);
    // auto* cube7 = new  Cube (glm::vec3( -2.35f, 0.0f,  1.70f),  1.0f, "cube7", 1.0f);
    // auto* cube8 = new  Cube (glm::vec3( -2.35f, 0.0f, -1.70f),  1.0f, "cube8", 1.0f);
    auto* cube9 = new  Cube (glm::vec3( 0.0f, -10.0f, 0.0f),    1.0f, "cube9", 1.0f);
    // Cube    cube12   (glm::vec3( 0.0f, 0.0f,  -4.0f), 1.0f, "cube12", 50000.0f);
    // Box     cube22   (glm::vec3( 3.0f, 0.0f,  -4.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "cube22");
    // Box     cube33   (glm::vec3( 3.0f, 0.0f,  -8.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "cube33");
    //      Cube      cube2(glm::vec3( 2.0f, 0.0f,  0.0f), 1.0f, "cube2");
    // Cube    cube10   (glm::vec3( 0.0f, 0.0f,  -3.5f), 1.0f, "cube10", 50000.0f);
    auto* plane1 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), "plane1");
    // auto* plane2 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), "plane2");

    // debug
    // dbgObj.push_back(&cube1);
    // dbgObj.push_back(&cube2);

    /* Object configuration */
    // cube1->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube4->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube6->setVelocity(glm::vec3(0.0f, 0.0f, 0.3f));
    // cube1->SetScale(glm::vec3(2.0f, 1.0f, 2.0f)); // showing that that the 'horizontal' plane is actually the xz-plane! 
    cube1->SetRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube1->SetRotation((static_cast<float>(M_PI_2 + M_PI_4)) * glm::vec3( 0.0f, 1.0f, 0.0f));
    cube2->SetRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    cube2->SetRotationVelocity(10.8f * glm::vec3( 0.0f, 0.0f, 1.0f));
    // cube2->SetRotation(-0.3f * glm::vec3( 0.0f, 1.0f, 0.0f));
    // cube1->SetRotationVelocity(1.8f * glm::vec3( 0.0f, 1.0f, 1.0f));
    cube2->setVelocity(glm::vec3(-25.32f, 0.0f, 0.0f));
    cube9->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    plane1->SetRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // plane1->SetRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane2->SetRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));

    /* Objects added to scene */
    // scene.AddObject(plane1);
    // scene.AddObject(plane2);
    scene.AddObject(cube1);
    scene.AddObject(cube2);
    scene.AddObject(cube3);
    // scene.AddObject(cube4);
    // scene.AddObject(cube5);
    // scene.AddObject(cube6);
    // scene.AddObject(cube9);
    
}



/* 
    used for trying to debug the rotation issue
 */
static void PopulateSceneTest3(Scene& scene)
{
    /* Object creation */
    auto* cube1 = new  Cube (glm::vec3( 0.0f, 0.0f,  0.0f),     1.0f, "cube1", 1.0f);
    auto* cube2 = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f),    glm::vec3( 2.0f, 1.0f,  1.0f), "cube2");
    auto* cube3 = new  Cube (glm::vec3( -1.25f, 0.0f,  0.65f),  1.0f, "cube3", 1.0f);
    // auto* cube4 = new  Cube (glm::vec3( -1.25f, 0.0f, -0.65f),  1.0f, "cube4", 1.0f);
    // auto* cube5 = new  Cube (glm::vec3( -2.35f, 0.0f,  0.85f),  1.0f, "cube5", 1.0f);
    // auto* cube6 = new  Cube (glm::vec3( -2.35f, 0.0f, -0.85f),  1.0f, "cube6", 1.0f);
    // auto* cube7 = new  Cube (glm::vec3( -2.35f, 0.0f,  1.70f),  1.0f, "cube7", 1.0f);
    // auto* cube8 = new  Cube (glm::vec3( -2.35f, 0.0f, -1.70f),  1.0f, "cube8", 1.0f);
    auto* cube9 = new  Cube (glm::vec3( 0.0f, -10.0f, 0.0f),    1.0f, "cube9", 1.0f);
    // Cube    cube12   (glm::vec3( 0.0f, 0.0f,  -4.0f), 1.0f, "cube12", 50000.0f);
    // Box     cube22   (glm::vec3( 3.0f, 0.0f,  -4.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "cube22");
    // Box     cube33   (glm::vec3( 3.0f, 0.0f,  -8.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "cube33");
    //      Cube      cube2(glm::vec3( 2.0f, 0.0f,  0.0f), 1.0f, "cube2");
    // Cube    cube10   (glm::vec3( 0.0f, 0.0f,  -3.5f), 1.0f, "cube10", 50000.0f);
    auto* plane1 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), "plane1");
    // auto* plane2 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), "plane2");

    // debug
    // dbgObj.push_back(&cube1);
    // dbgObj.push_back(&cube2);

    /* Object configuration */
    // cube1->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube4->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube6->setVelocity(glm::vec3(0.0f, 0.0f, 0.3f));
    // cube1->SetScale(glm::vec3(2.0f, 1.0f, 2.0f)); // showing that that the 'horizontal' plane is actually the xz-plane! 
    cube1->SetRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube1->SetRotation((static_cast<float>(M_PI_2 + M_PI_4)) * glm::vec3( 0.0f, 1.0f, 0.0f));
    cube2->SetRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    cube2->SetRotationVelocity(10.8f * glm::vec3( 0.0f, 0.0f, 1.0f));
    // cube2->SetRotation(-0.3f * glm::vec3( 0.0f, 1.0f, 0.0f));
    // cube1->SetRotationVelocity(1.8f * glm::vec3( 0.0f, 1.0f, 1.0f));
    cube2->setVelocity(glm::vec3(-25.32f, 0.0f, 0.0f));
    cube9->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    plane1->SetRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // plane1->SetRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane2->SetRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));

    /* Objects added to scene */
    // scene.AddObject(plane1);
    // scene.AddObject(plane2);
    scene.AddObject(cube1);
    scene.AddObject(cube2);
    scene.AddObject(cube3);
    // scene.AddObject(cube4);
    // scene.AddObject(cube5);
    // scene.AddObject(cube6);
    // scene.AddObject(cube9);
    
}

static void PopulateSceneTest4(Scene& scene)
{
    float s = 1.25f;
    float s2 = 2 * s - 0.2f;
    float ys = 0.0f;
    /* Object creation */
    auto* smasher = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "smasher");
    auto* cube2   = new  Cube (glm::vec3( 0.0f, ys,  0.0f), 1.0f, "cube2", 1.0f);
    auto* cube3   = new  Cube (glm::vec3( -s,   ys,  -s),   1.0f, "cube3", 1.0f);
    auto* cube4   = new  Cube (glm::vec3( -s,   ys,   s),   1.0f, "cube4", 1.0f);
    auto* cube5   = new  Cube (glm::vec3( -s,   ys,  0.0f), 1.0f, "cube5", 1.0f);
    auto* cube6   = new  Cube (glm::vec3( -s2,  ys,  -s2),  1.0f, "cube6", 1.0f);
    auto* cube7   = new  Cube (glm::vec3( -s2,  ys,  -s),   1.0f, "cube7", 1.0f);
    auto* cube8   = new  Cube (glm::vec3( -s2,  ys,   s),   1.0f, "cube8", 1.0f);
    auto* cube9   = new  Cube (glm::vec3( -s2,  ys,   s2),  1.0f, "cube9", 1.0f);
    
    auto* hugeCube = new  Cube (glm::vec3( 0.0f, -30.0f,  0.65f),  10.0f, "hugeCube", 20.0f);


    cube2->SetRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    smasher->SetRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    smasher->SetRotationVelocity(10.8f * glm::vec3( 0.0f, 0.0f, 1.0f));
    smasher->setVelocity(glm::vec3(-25.32f, 0.0f, 0.0f));
    hugeCube->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    hugeCube->SetRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    
    
    scene.AddObject(cube2);
    scene.AddObject(smasher);
    scene.AddObject(cube3);
    scene.AddObject(cube4);
    scene.AddObject(cube5);
    scene.AddObject(cube6);
    scene.AddObject(cube7);
    scene.AddObject(cube8, TextureManager::kRedWindow);
    scene.AddObject(cube9, TextureManager::kMetal);
    scene.AddObject(hugeCube, TextureManager::kMetal);
    
}


static void PopulateSceneTest5(Scene& scene)
{
    float s = 1.25f;
    // float s2 = 2 * s - 0.2f;
    float ys = 0.0f;
    /* Object creation */
    // auto* smasher = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f), glm::vec3( 2.0f, 1.0f,  1.0f), "smasher");
    auto* smasher       = new  Cube (glm::vec3( -10.0f, ys,  0.0f), 1.0f, "smasher", 1.0f);
    auto* smasherRev    = new  Cube (glm::vec3( 12.0f, ys,  0.0f), 1.0f, "smasherRev", 1.0f);
    auto* cube3         = new  Cube (glm::vec3( -s,   ys,  -s),   1.0f, "cube3", 1.0f);
    auto* cube4         = new  Cube (glm::vec3( -s,   ys,   s),   1.0f, "cube4", 1.0f);
    auto* cube5         = new  Cube (glm::vec3( -s,   ys,  0.0f), 1.0f, "cube5", 1.0f);
    // auto* cube6   = new  Cube (glm::vec3( -s2,  ys,  -s2),  1.0f, "cube6", 1.0f);
    // auto* cube7   = new  Cube (glm::vec3( -s2,  ys,  -s),   1.0f, "cube7", 1.0f);
    // auto* cube8   = new  Cube (glm::vec3( -s2,  ys,   s),   1.0f, "cube8", 1.0f);
    // auto* cube9   = new  Cube (glm::vec3( -s2,  ys,   s2),  1.0f, "cube9", 1.0f);
    
    // auto* hugeCube = new  Cube (glm::vec3( 0.0f, -30.0f,  0.65f),  10.0f, "hugeCube", 20.0f);


    // smasher->SetRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // smasher->SetRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    smasher->SetRotationVelocity(10.8f * glm::vec3( 0.0f, 0.0f, 1.0f));
    smasher->setVelocity(glm::vec3(25.32f, 0.0f, 0.0f));
    smasherRev->SetRotationVelocity(10.8f * glm::vec3( 0.0f, 0.0f, 1.0f));
    smasherRev->setVelocity(glm::vec3(-20.32f, 0.0f, 0.0f));
    // hugeCube->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    // hugeCube->SetRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    
    
    scene.AddObject(smasher);
    scene.AddObject(smasherRev);
    scene.AddObject(cube3);
    scene.AddObject(cube4);
    scene.AddObject(cube5);
    // scene.AddObject(cube6);
    // scene.AddObject(cube7);
    // scene.AddObject(cube8, TextureManager::kRedWindow);
    // scene.AddObject(cube9, TextureManager::kMetal);
    // scene.AddObject(hugeCube, TextureManager::kMetal);
    
}

Scene SceneFactory::GetScene(EnumScene enumScene/* , Shader& shader */)
{
    // create a scene
    Scene scene(0.01, mCamera, mWidth, mHeight/* , shader */);
    
    switch (enumScene)
    {
        case kSceneTest1: PopulateSceneTest1(scene); break;
        case kSceneTest2: PopulateSceneTest2(scene); break;
        case kSceneTest3: PopulateSceneTest3(scene); break;
        case kSceneTest4: PopulateSceneTest4(scene); break;
        case kSceneTest5: PopulateSceneTest5(scene); break;
    }
    // handle debug objects adding centrally
    AddDebugObjects(scene, scene.GetShader());
    return scene;
}

