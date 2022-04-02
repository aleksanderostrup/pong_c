#include "skybox.h"
#include <cmath>
#include <vector>

static bool skyboxExits = false;
static unsigned int skyboxVAO, skyboxVBO;

unsigned int LoadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            exit(1);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}  



Skybox::Skybox(Camera const& camera, uint32_t const& width, uint32_t const& height)
 : mCamera(camera)
 , mHeight(height)
 , mWidth(width)
//   Object(position, scale, name, mass)
{
  assert(skyboxExits == false);
  skyboxExits = true; // only allow one skybox to exist
  mShader = new Shader("../shaders/SkyboxShader.vs", "../shaders/SkyboxShader.fs");
    float skyboxVertices[] = {
        // positions          a
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };


    std::cout << "Prepare VBO + VAO for Skybox class" << std::endl;
    
    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

         // load textures
    // -------------
    std::vector<std::string> faces
    {
        "../skyboxes/water_and_mountains/right.jpg",
        "../skyboxes/water_and_mountains/left.jpg",
        "../skyboxes/water_and_mountains/top.jpg",
        "../skyboxes/water_and_mountains/bottom.jpg",
        "../skyboxes/water_and_mountains/front.jpg",
        "../skyboxes/water_and_mountains/back.jpg",
    };
    // Takes a little bit fo time
    mCubemapTexture = LoadCubemap(faces);

    mShader->use();
    mShader->setInt("skybox", 0);

}

Skybox::~Skybox()
{
    std::cout << "Clean up data (VBO + VAO)" << std::endl;
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    delete mShader;
}


/*
  make a selection between relative and absoulte draw (relative should be to previously drawn! - we need to make model member and reset it to 1.0f on absolute)

  also, we can re-use texture after first call... this should be called from updateScene
*/
void Skybox::DrawInit()
{
  // skyboxs
  glBindVertexArray(skyboxVAO);
}

void Skybox::Draw()
{
  glm::mat4 projection = glm::perspective(glm::radians(mCamera.GetZoom()), (float)mWidth / (float)mHeight, 0.1f, 100.0f);
  glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
  mShader->use();
  glm::mat4 view = glm::mat4(glm::mat3(mCamera.GetViewMatrix())); // remove translation from the view matrix
  mShader->setMat4("view", view);
  mShader->setMat4("projection", projection);
  
  // skybox cube
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapTexture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default
 
}


