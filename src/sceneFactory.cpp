#include "../include/sceneFactory.h"
#include "../include/object.h"
#include "../include/plane.h"
#include "../include/box.h"
#include "../include/cube.h"

SceneFactory::SceneFactory()
{
}


static inline void AddDebugObjects(Scene& scene, Shader& shader)
{
    auto* colPointBox = new Box(glm::vec3( 3.0f, 0.0f,  -3.5f), glm::vec3( 0.08f, 0.08f,  0.08f), &shader, "colPointBox");

    scene.setColPointDebugObject(colPointBox);
    std::vector<Object*> toInsert;
    std::vector<Object*> toInsert2;
    for (size_t i = 0; i < 5; i++)
    {
        std::string tmpName = "colNormalPointBox" + i;
        auto* colNormalBox = new Box(glm::vec3( 3.0f, 0.0f,  -3.5f), glm::vec3( 0.06f, 0.06f,  0.06f), &shader, tmpName.c_str());
        auto* colNormalBox2 = new Box(glm::vec3( 3.0f, 0.0f,  -3.5f), glm::vec3( 0.06f, 0.06f,  0.06f), &shader, tmpName.c_str());
        toInsert.push_back(colNormalBox);
        toInsert2.push_back(colNormalBox2);
    }
    scene.setColNormalDebugObject(toInsert, 0);
    scene.setColNormalDebugObject(toInsert, 1);
    colPointBox->SetTexture(Box::kMetal);
}

static void PopulateSceneTest1(Scene& scene, Shader& shader)
{
    /* Object creation */
    auto* cube1 = new  Cube (glm::vec3( 0.0f, 0.0f,  0.0f),     1.0f, &shader, "cube1", 1.0f);
    auto* cube2 = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f),    glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube2");
    auto* cube3 = new  Cube (glm::vec3( -1.25f, 0.0f,  0.65f),  1.0f, &shader, "cube3", 1.0f);
    auto* cube4 = new  Cube (glm::vec3( -1.25f, 0.0f, -0.65f),  1.0f, &shader, "cube4", 1.0f);
    auto* cube5 = new  Cube (glm::vec3( -2.35f, 0.0f,  0.85f),  1.0f, &shader, "cube5", 1.0f);
    auto* cube6 = new  Cube (glm::vec3( -2.35f, 0.0f, -0.85f),  1.0f, &shader, "cube6", 1.0f);
    // auto* cube7 = new  Cube (glm::vec3( -2.35f, 0.0f,  1.70f),  1.0f, &shader, "cube7", 1.0f);
    // auto* cube8 = new  Cube (glm::vec3( -2.35f, 0.0f, -1.70f),  1.0f, &shader, "cube8", 1.0f);
    auto* cube9 = new  Cube (glm::vec3( 0.0f, -10.0f, 0.0f),    1.0f, &shader, "cube9", 1.0f);
    // Cube    cube12   (glm::vec3( 0.0f, 0.0f,  -4.0f), 1.0f, &shader, "cube12", 50000.0f);
    // Box     cube22   (glm::vec3( 3.0f, 0.0f,  -4.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube22");
    // Box     cube33   (glm::vec3( 3.0f, 0.0f,  -8.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube33");
    //      Cube      cube2(glm::vec3( 2.0f, 0.0f,  0.0f), 1.0f, &shader, "cube2");
    // Cube    cube10   (glm::vec3( 0.0f, 0.0f,  -3.5f), 1.0f, &shader, "cube10", 50000.0f);
    auto* plane1 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), &shader, "plane1");
    // auto* plane2 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), &shader, "plane2");

    // debug
    // dbgObj.push_back(&cube1);
    // dbgObj.push_back(&cube2);

    /* Object configuration */
    // cube1->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube4->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube6->setVelocity(glm::vec3(0.0f, 0.0f, 0.3f));
    // cube1->setScale(glm::vec3(2.0f, 1.0f, 2.0f)); // showing that that the 'horizontal' plane is actually the xz-plane! 
    cube1->setRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube1->setRotation((static_cast<float>(M_PI_2 + M_PI_4)) * glm::vec3( 0.0f, 1.0f, 0.0f));
    cube2->setRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube2->setRotation(-0.3f * glm::vec3( 0.0f, 1.0f, 0.0f));
    // cube1>setRotationVelocity(1.8f * glm::vec3( 0.0f, 1.0f, 1.0f));
    cube2->setVelocity(glm::vec3(-25.32f, 0.0f, 0.0f));
    cube9->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    plane1->setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane1->setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane2->setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));

    /* Objects added to scene */
    // scene.addObject(plane1);
    // scene.addObject(plane2);
    scene.addObject(cube1);
    scene.addObject(cube2);
    scene.addObject(cube3);
    scene.addObject(cube4);
    scene.addObject(cube5);
    scene.addObject(cube6);
    scene.addObject(cube9);
    // scene.addObject(cube7);
    // scene.addObject(cube8);
    // scene.addObject(cube3);
    // scene.addObject(cube12);
    // scene.addObject(cube22);
    // scene.addObject(cube33);
    // scene.addObject(cube10);
    // scene.addObject(box20);
    // scene.addObject(cube3);
    // scene.addObject(cube4);
    // scene.addObject(cube5);
    // scene.addObject(cube6);
    
}

/* 
    used for trying to debug the rotation issue
 */
static void PopulateSceneTest2(Scene& scene, Shader& shader)
{
    /* Object creation */
    auto* cube1 = new  Cube (glm::vec3( 0.0f, 0.0f,  0.0f),     1.0f, &shader, "cube1", 1.0f);
    auto* cube2 = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f),    glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube2");
    auto* cube3 = new  Cube (glm::vec3( -1.25f, 0.0f,  0.65f),  1.0f, &shader, "cube3", 1.0f);
    // auto* cube4 = new  Cube (glm::vec3( -1.25f, 0.0f, -0.65f),  1.0f, &shader, "cube4", 1.0f);
    // auto* cube5 = new  Cube (glm::vec3( -2.35f, 0.0f,  0.85f),  1.0f, &shader, "cube5", 1.0f);
    // auto* cube6 = new  Cube (glm::vec3( -2.35f, 0.0f, -0.85f),  1.0f, &shader, "cube6", 1.0f);
    // auto* cube7 = new  Cube (glm::vec3( -2.35f, 0.0f,  1.70f),  1.0f, &shader, "cube7", 1.0f);
    // auto* cube8 = new  Cube (glm::vec3( -2.35f, 0.0f, -1.70f),  1.0f, &shader, "cube8", 1.0f);
    auto* cube9 = new  Cube (glm::vec3( 0.0f, -10.0f, 0.0f),    1.0f, &shader, "cube9", 1.0f);
    // Cube    cube12   (glm::vec3( 0.0f, 0.0f,  -4.0f), 1.0f, &shader, "cube12", 50000.0f);
    // Box     cube22   (glm::vec3( 3.0f, 0.0f,  -4.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube22");
    // Box     cube33   (glm::vec3( 3.0f, 0.0f,  -8.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube33");
    //      Cube      cube2(glm::vec3( 2.0f, 0.0f,  0.0f), 1.0f, &shader, "cube2");
    // Cube    cube10   (glm::vec3( 0.0f, 0.0f,  -3.5f), 1.0f, &shader, "cube10", 50000.0f);
    auto* plane1 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), &shader, "plane1");
    // auto* plane2 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), &shader, "plane2");

    // debug
    // dbgObj.push_back(&cube1);
    // dbgObj.push_back(&cube2);

    /* Object configuration */
    // cube1->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube4->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube6->setVelocity(glm::vec3(0.0f, 0.0f, 0.3f));
    // cube1->setScale(glm::vec3(2.0f, 1.0f, 2.0f)); // showing that that the 'horizontal' plane is actually the xz-plane! 
    cube1->setRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube1->setRotation((static_cast<float>(M_PI_2 + M_PI_4)) * glm::vec3( 0.0f, 1.0f, 0.0f));
    cube2->setRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    cube2->setRotationVelocity(10.8f * glm::vec3( 0.0f, 0.0f, 1.0f));
    // cube2->setRotation(-0.3f * glm::vec3( 0.0f, 1.0f, 0.0f));
    // cube1->setRotationVelocity(1.8f * glm::vec3( 0.0f, 1.0f, 1.0f));
    cube2->setVelocity(glm::vec3(-25.32f, 0.0f, 0.0f));
    cube9->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    plane1->setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // plane1->setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane2->setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));

    /* Objects added to scene */
    // scene.addObject(plane1);
    // scene.addObject(plane2);
    scene.addObject(cube1);
    scene.addObject(cube2);
    scene.addObject(cube3);
    // scene.addObject(cube4);
    // scene.addObject(cube5);
    // scene.addObject(cube6);
    // scene.addObject(cube9);
    
}



/* 
    used for trying to debug the rotation issue
 */
static void PopulateSceneTest3(Scene& scene, Shader& shader)
{
    /* Object creation */
    auto* cube1 = new  Cube (glm::vec3( 0.0f, 0.0f,  0.0f),     1.0f, &shader, "cube1", 1.0f);
    auto* cube2 = new  Box  (glm::vec3( 30.0f, 0.0f,  0.0f),    glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube2");
    auto* cube3 = new  Cube (glm::vec3( -1.25f, 0.0f,  0.65f),  1.0f, &shader, "cube3", 1.0f);
    // auto* cube4 = new  Cube (glm::vec3( -1.25f, 0.0f, -0.65f),  1.0f, &shader, "cube4", 1.0f);
    // auto* cube5 = new  Cube (glm::vec3( -2.35f, 0.0f,  0.85f),  1.0f, &shader, "cube5", 1.0f);
    // auto* cube6 = new  Cube (glm::vec3( -2.35f, 0.0f, -0.85f),  1.0f, &shader, "cube6", 1.0f);
    // auto* cube7 = new  Cube (glm::vec3( -2.35f, 0.0f,  1.70f),  1.0f, &shader, "cube7", 1.0f);
    // auto* cube8 = new  Cube (glm::vec3( -2.35f, 0.0f, -1.70f),  1.0f, &shader, "cube8", 1.0f);
    auto* cube9 = new  Cube (glm::vec3( 0.0f, -10.0f, 0.0f),    1.0f, &shader, "cube9", 1.0f);
    // Cube    cube12   (glm::vec3( 0.0f, 0.0f,  -4.0f), 1.0f, &shader, "cube12", 50000.0f);
    // Box     cube22   (glm::vec3( 3.0f, 0.0f,  -4.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube22");
    // Box     cube33   (glm::vec3( 3.0f, 0.0f,  -8.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube33");
    //      Cube      cube2(glm::vec3( 2.0f, 0.0f,  0.0f), 1.0f, &shader, "cube2");
    // Cube    cube10   (glm::vec3( 0.0f, 0.0f,  -3.5f), 1.0f, &shader, "cube10", 50000.0f);
    auto* plane1 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), &shader, "plane1");
    // auto* plane2 = new Plane(glm::vec3(10.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 1.0f), &shader, "plane2");

    // debug
    // dbgObj.push_back(&cube1);
    // dbgObj.push_back(&cube2);

    /* Object configuration */
    // cube1->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube4->setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube6->setVelocity(glm::vec3(0.0f, 0.0f, 0.3f));
    // cube1->setScale(glm::vec3(2.0f, 1.0f, 2.0f)); // showing that that the 'horizontal' plane is actually the xz-plane! 
    cube1->setRotation(-1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    // cube1->setRotation((static_cast<float>(M_PI_2 + M_PI_4)) * glm::vec3( 0.0f, 1.0f, 0.0f));
    cube2->setRotation(1.3f * glm::vec3( 0.1f, 0.1f, 0.1f));
    cube2->setRotationVelocity(10.8f * glm::vec3( 0.0f, 0.0f, 1.0f));
    // cube2->setRotation(-0.3f * glm::vec3( 0.0f, 1.0f, 0.0f));
    // cube1->setRotationVelocity(1.8f * glm::vec3( 0.0f, 1.0f, 1.0f));
    cube2->setVelocity(glm::vec3(-25.32f, 0.0f, 0.0f));
    cube9->setVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
    plane1->setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // plane1->setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane2->setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));

    /* Objects added to scene */
    // scene.addObject(plane1);
    // scene.addObject(plane2);
    scene.addObject(cube1);
    scene.addObject(cube2);
    scene.addObject(cube3);
    // scene.addObject(cube4);
    // scene.addObject(cube5);
    // scene.addObject(cube6);
    // scene.addObject(cube9);
    
}

Scene SceneFactory::GetScene(EnumScene enumScene, Shader& shader)
{
    // create a scene
    Scene scene(0.01);
    
    switch (enumScene)
    {
        case kSceneTest1: PopulateSceneTest1(scene, shader); break;
        case kSceneTest2: PopulateSceneTest2(scene, shader); break;
        case kSceneTest3: PopulateSceneTest3(scene, shader); break;
    }
    // handle debug objects adding centrally
    AddDebugObjects(scene, shader);
    return scene;
}

