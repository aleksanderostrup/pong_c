
// TODO: clean up includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_s.h"
#include "camera.h"
#include "model.h"
#include "sceneFactory.h"
#include "cmdInterpreter.h"
#include "inputprocess.h"
#include "glfw_setup.h"
#include "box.h"

#include <iostream>
#include <vector>
#include <thread>
#include <time.h>
#include <cmath>

/*
todo:
    - scene recording with events and specific times (including slo-mo)
    TODO: record all collision data entering a collision that does not conserve velocity + rotation momentum OR does not converge
    TODO: then we can play those collisions back and recreate the 
    - enum -> enum class
    - possibility to play "background" music (direction? Stereo? Surround?)
    - arrow object to attach to other objects to debug vectors
    - threading
        * have an input thread running or just wrap in class and poll?
    - audio from https://openal.org/  [openGL like API with 3D sound]
    - audio should use a 2D LUT with both dims = material and value = sound 
    - more physics
    - lightning class
    - continue with advanced OpenGL https://learnopengl.com/Advanced-OpenGL/Framebuffers
    - option to pause time when collision occurs (but still move camera)
    - get object from scene by name
    - upgrade to OpenGL 4.6 [set version below AND download new API (glad lib must be downloaded with new generated code)]
    - if objects are set to immovable, they should not have their model updated
    - create destructor for the object abstract class
    - soundtrack MUST include Freude Schöner Götterfunker
        * fun implementation: each box selects a different track being played :)
        * join with the openal implementation
    - object highlighting and selecting (with lookAt)
    - manipulation of selected objects
    - console with command input
        * one should be to print total linear momentum / angular momentum -> we can check if it is preserved
        * one should be to start recording (of postions) so we can go back and forth to observe. This requires recording all the physical parameters
    - themes (including Dan theme)
    - go through comments and fix stuff
    - instant test scenatios
    - command line input
    - track object with a (user-defined?) depth so we can back track once on collision (for a simple start)
      and go back in time a few frames to inspect collisions
    - check functions and set pass by reference where apt
    - slow motion (via dt)
    - consider renaming scene.h / camera.h (others?), since assimp has same named header files!
    - add this amazing resource to the docs: https://www.geometrictools.com/Documentation/Documentation.html
    - use caching in SAT ie use last failing index / axis first on next try!
    - pre-compute sphere and store in object, instead of taking sqrt every frame!
    - the information stored between two objects can be released once their 'spheres' don't touch anymore, thus re-using the memory
    - better collision handling by making box slightly bigger until collision - at the collision it just retracts to 'original' / correct size, and bounces back again
      after a few frames
    - replace the constant: deltaTime in UpdateScene with the variable
    - skybox draw function must be connected to changes in height / width! 
          * in this line: glm::mat4 projection = glm::perspective(glm::radians(mCamera.Zoom), (float)2520 / (float)1400, 0.1f, 100.0f);
    - mouse click mode (change between select / shoot / etc.)
*/


// unsigned int loadTexture(const char *path);
// TODO: settings should be in separate object!
// settings
const unsigned int SCR_WIDTH = 2520;
const unsigned int SCR_HEIGHT = 1400;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

vector<Object*> dbgObj;

int main()
{
    auto scr_width  = SCR_WIDTH;
    auto scr_height = SCR_HEIGHT;
    GLFWwindow* window = glfw_init(&camera, &scr_width, &scr_height);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // command thread 
    // std::thread cmds(threadTest);

    // add commands and put the interpreter in a new thread -> poll it in frame loop?
    // we should add a wrapper around the interpreter that handles commands as either sync or async
    // sync commands are run in the frame loop, async are meta commands that are run immediately
    // we should keep the CmdInterpreter completely agnostic about anything related to graphics and let the wrapper handle it if needed

    // CmdInterpreter cmdIntptr;
    // CmdInterpreter::CmdInterpreterFnc testFnc = [](std::string str) { std::cout << "Running test command! " << str << "\n"; };
    // cmdIntptr.addCmd(std::string("test"), testFnc);
    // cmdIntptr.getCmd();

    
    // create a scene
    auto scene = SceneFactory(camera, scr_width, scr_height).GetScene(SceneFactory::SceneId::kSceneTest5);

    // create instance of input processing
    InputProcess inputProcess(window, &camera, scene);
    inputProcess.BindKeyToAction(GLFW_KEY_SPACE, [](Scene& scene){scene.TogglePause();              });
    inputProcess.BindKeyToAction(GLFW_KEY_P,     [](Scene& scene){scene.PrintInfoForSelected();     });
    inputProcess.BindKeyToAction(GLFW_KEY_F,     [](Scene& scene){scene.StupidDebug();              });
    inputProcess.BindKeyToAction(GLFW_KEY_T,     [](Scene& scene){scene.PrintSummedVelAndRot();     });
    inputProcess.BindKeyToAction(GLFW_KEY_UP,    [](Scene& scene){scene.ModifyTime(2.0f);           });
    inputProcess.BindKeyToAction(GLFW_KEY_DOWN,  [](Scene& scene){scene.ModifyTime(0.5f);           });
    inputProcess.BindKeyToAction(GLFW_KEY_RIGHT, [](Scene& scene){scene.ForceFrameForward(0.0124);  });
    inputProcess.BindKeyToAction(GLFW_KEY_LEFT,  [](Scene& scene){scene.FrameBackward();            });
    inputProcess.BindKeyToAction(GLFW_KEY_R,     [](Scene& scene){scene.ToggleFrameForwardDebug();  });

    // make a bit larger bounding box that is deactivated after a collision (for e.g. 5 frames). and then it is re-activated
    // or just fix cases where we have a FACE-FACE or FACE-EDGE collision -> this seems to be the issue when the rotation is rotated into the 
    // object it hits because the collision point could not be resolved

    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    // unsigned int transparentTexture = loadTexture("../textures/window.png");

    // transparent window locations
    // --------------------------------
    vector<glm::vec3> windows
    {
        glm::vec3( -1.350388 - 0.5f, 0.382403, 0.104056 ),
        // glm::vec3( -0.0f, 0.0f, 0.0f),
        // glm::vec3( 1.5f, 0.0f, 0.51f),
        // glm::vec3( 0.0f, 0.0f, 0.7f),
        // glm::vec3(-0.3f, 0.0f, -2.3f),
        // glm::vec3( 0.5f, 0.0f, -0.6f)
    };

    // shader configuration
    // --------------------
    // shader.use(); // if multiple shaders exists, they should be set to use in the render loop
    // shader.setInt("texture1", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        inputProcess.processAllInput(deltaTime);

        // sort the transparent windows before rendering
        // ---------------------------------------------
        std::map<float, glm::vec3> sorted;
        for (auto const& window : windows)
        {
            float distance = glm::length(camera.GetPosition() - window);
            sorted[distance] = window;
        }

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        // only update frame if we want pause and no step frame forward
        // all objects are moved and drawn here
        // also, collisions are detected and calculated
        // TODO -> INSERT DELTA TIME INSTEAD OF FIXED TIME!! - Fixed time only for recreating issues
        scene.UpdateScene(0.0124/* deltaTime */);

        
        // windows (from furthest to nearest)
        // glBindVertexArray(transparentVAO);
        // glBindTexture(GL_TEXTURE_2D, transparentTexture);
        // for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        // {
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, it->second);
        //     shader.setMat4("model", model);
        //     glDrawArrays(GL_TRIANGLES, 0, 6);
        // }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
