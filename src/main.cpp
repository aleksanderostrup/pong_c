#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_s.h"
#include "camera.h"
#include "model.h"
#include "cube.h"
#include "skybox.h"
#include "plane.h"
#include "scene.h"
#include "sceneFactory.h"
#include "cmdInterpreter.h"
#include "inputprocess.h"
#include "glfw_setup.h"

#include <iostream>
#include <vector>
#include <thread>
#include <time.h>
#include <cmath>

/*
todo:
    - scene recording with events and specific times (including slo-mo)
    - enum -> enum class
    - possibility to play "background" music (direction? Stereo? Surround?)
    - arrow object to attach to other objects to debug vectors :D
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


// void threadTest();

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

    // create instance of input processing
    InputProcess inputProcess(window, &camera);

    // add commands and put the interpreter in a new thread -> poll it in frame loop?
    // we should add a wrapper around the interpreter that handles commands as either sync or async
    // sync commands are run in the frame loop, async are meta commands that are run immediately
    // we should keep the CmdInterpreter completely agnostic about anything related to graphics and let the wrapper handle it if needed

    // CmdInterpreter cmdIntptr;
    // CmdInterpreter::CmdInterpreterFnc testFnc = [](std::string str) { std::cout << "Running test command! " << str << "\n"; };
    // cmdIntptr.addCmd(std::string("test"), testFnc);
    // cmdIntptr.getCmd();

    
    // create a scene
    SceneFactory sceneFactory(camera, scr_width, scr_height);
    Scene scene = sceneFactory.GetScene(SceneFactory::kSceneTest3);
    
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
    float timeMultiplier = 1.0f; // speed time up or down

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
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        
        // only update frame if we want pause and no step frame forward
        bool pauseFrame = inputProcess.keyActions.pause & !inputProcess.keyActions.frameForward;

        if      (inputProcess.keyActions.printDebug)     { scene.printInfoForSelected(); }
        if      (inputProcess.keyActions.fAction)        { scene.stupidDebug(); }
        if      (inputProcess.keyActions.keyUpAction)    {std::cout << "Up\n"; timeMultiplier *= 2; }
        else if (inputProcess.keyActions.keyDownAction)  {std::cout << "Down\n"; timeMultiplier /= 2; }

        // TODO -> INSERT DELTA TIME INSTEAD OF FIXED TIME!!!!!!
        // all objects are moved, drawn here
        // also, collisions are detected and calculated
        
        scene.updateScene(0.0124 * timeMultiplier/* deltaTime */, pauseFrame);
        
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


// utility function for loading a 2D texture from file
// ---------------------------------------------------
// unsigned int loadTexture(char const * path)
// {
//     unsigned int textureID;
//     glGenTextures(1, &textureID);

//     int width, height, nrComponents;
//     unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
//     if (data)
//     {
//         GLenum format;
//         if (nrComponents == 1)
//             format = GL_RED;
//         else if (nrComponents == 3)
//             format = GL_RGB;
//         else if (nrComponents == 4)
//             format = GL_RGBA;

//         glBindTexture(GL_TEXTURE_2D, textureID);
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);

//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         stbi_image_free(data);
//     }
//     else
//     {
//         std::cout << "Texture failed to load at path: " << path << std::endl;
//         stbi_image_free(data);
//     }

//     return textureID;
// }

// void threadTest()
// {
//     struct timespec requested_time;
//     requested_time.tv_sec = 2;
//     requested_time.tv_nsec = 0L;
//     int i;
//     while (1)
//     {
//         std::cin >> i;
//         nanosleep(&requested_time, NULL);
//         std::cout << i << "\n" << std::endl;
        
//     }
// }