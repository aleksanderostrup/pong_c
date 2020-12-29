#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/shader_s.h"
#include "../include/camera.h"
#include "../include/model.h"
#include "../include/cube.h"
#include "../include/plane.h"
#include "../include/scene.h"
#include "../include/inputprocess.h"
#include "../include/glfw_setup.h"

#include <iostream>
#include <vector>

/*
todo:
    - arrow object to attach to other objects to debug vectors :D
    - threading
        * have an input thread running or just wrap in class and poll?
    - audio from https://openal.org/  [openGL like API with 3D sound]
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
    - skybox 
    - themes (including Dan theme)
    - go through comments and fix stuff
    - instant test scenatios
    - command line input
    - track object with a (user-defined?) depth so we can back track once on collision (for a simple start)
      and go back in time a few frames to inspect collisions
    - rotation in 3 degrees
    - check functions and set pass by reference where apt
    - slow motion (via dt)
    - consider renaming scene.h / camera.h (others?), since assimp has same named header files!
    - add this amazing resource to the docs: https://www.geometrictools.com/Documentation/Documentation.html
*/

unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

vector<Object*> dbgObj;

int main()
{
    GLFWwindow* window = glfw_init(&camera);

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create instance of input processing
    InputProcess inputProcess(window, &camera);

    // build and compile shaders
    // -------------------------
    Shader shader("../shaders/shader.vs", "../shaders/shader.fs");

    
    // create a scene
    Scene scene(0.01);
    Cube cube1(glm::vec3( 0.0f, 0.0f,  0.0f), 1.0f, &shader, "cube1", 50000.0f);
    Box cube2(glm::vec3( 3.0f, 0.0f,  0.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube2");
    Cube cube12(glm::vec3( 0.0f, 0.0f,  -4.0f), 1.0f, &shader, "cube12", 50000.0f);
    Box cube22(glm::vec3( 3.0f, 0.0f,  -4.0f), glm::vec3( 2.0f, 1.0f,  1.0f), &shader, "cube22");
    // Cube cube2(glm::vec3( 2.0f, 0.0f,  0.0f), 1.0f, &shader, "cube2");
    Cube cube10(glm::vec3( 0.0f, 0.0f,  -3.5f), 1.0f, &shader, "cube10", 50000.0f);
    Box box20(glm::vec3( 3.0f, 0.0f,  -3.5f), glm::vec3( 1.0f, 1.0f,  1.0f), &shader, "box20");
    Cube cube3(glm::vec3( 9.0f, 0.0f, -1.0f), 1.0f, &shader, "cube3");
    Cube cube4(glm::vec3(-1.0f, 0.0f, -5.0f), 1.0f, &shader, "cube4");
    Cube cube5(glm::vec3( 2.0f, 0.0f, -5.0f), 1.0f, &shader, "cube5");
    Cube cube6(glm::vec3( 9.0f, 0.0f, -5.0f), 1.0f, &shader, "cube6");
    Plane plane1(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), &shader, "plane1");
    Plane plane2(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), &shader, "plane2");

    // debug
    dbgObj.push_back(&cube1);
    dbgObj.push_back(&cube2);

    //cube1.setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube4.setVelocity(glm::vec3(0.3f, 0.0f, 0.0f));
    // cube6.setVelocity(glm::vec3(0.0f, 0.0f, 0.3f));
    // cube1.setRotationVelocity(0.1);
    // cube2.setRotationVelocity(0.1);
    // cube1.setScale(glm::vec3(2.0f, 1.0f, 2.0f)); // showing that that the 'horizontal' plane is actually the xz-plane! 
    cube1.setRotation(glm::vec3( 0.0f, 1.0f, 1.0f), 3.141 / 4);
    cube2.setVelocity(glm::vec3(-0.3f, 0.0f, 0.0f));
    // cube12.setRotation(glm::vec3( 0.0f, 0.0f, 1.0f), 3.141 / 4);
    cube22.setVelocity(glm::vec3(-0.3f, 0.0f, 0.0f));
    // cube10.setRotation(glm::vec3( 0.0f, 1.0f, 1.0f), 1.32 * 3.14 / 4);
    // box20.setRotation(-glm::vec3( 0.0f, 1.0f, 1.0f),  1.32 * 3.14 / 4);
    // box20.setVelocity(glm::vec3(-0.3f, 0.0f, 0.0f));
    // plane1.setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane1.setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    // plane2.setRotationVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    // scene.addObject(&plane1);
    // scene.addObject(&plane2);
    scene.addObject(&cube1);
    scene.addObject(&cube2);
    scene.addObject(&cube12);
    scene.addObject(&cube22);
    // scene.addObject(&cube10);
    // scene.addObject(&box20);
    // scene.addObject(&cube3);
    // scene.addObject(&cube4);
    // scene.addObject(&cube5);
    // scene.addObject(&cube6);
    

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
    // unsigned int floorTexture = loadTexture("textures/metal.png");
    unsigned int transparentTexture = loadTexture("../textures/window.png");

    // transparent window locations
    // --------------------------------
    vector<glm::vec3> windows
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3( 1.5f, 0.0f, 0.51f),
        glm::vec3( 0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3( 0.5f, 0.0f, -0.6f)
    };

    // shader configuration
    // --------------------
    shader.use(); // if multiple shaders exists, they should be set to use in the render loop
    shader.setInt("texture1", 0);

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

        if (inputProcess.keyActions.printDebug)
        {
            for (auto& v : dbgObj)
            {
                v->printObject();
            }
        }

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw objects
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        
        // all objects are moved, drawn here
        // also, collisions are detected and calculated
        scene.updateScene(deltaTime, inputProcess.keyActions.pause);
        
        // windows (from furthest to nearest)
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


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
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
