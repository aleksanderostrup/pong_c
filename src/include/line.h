#pragma once

#include "glm/glm.hpp"
#include "shader_s.h"
#include "object.h"
#include "camera.h"

class Line
{
  public:

    Line(glm::vec3 position, glm::vec3 direction, float length, Camera& camera);
    ~Line();

    // void drawInit();
    void draw           (Shader* shader, glm::mat4& projection);
    void setPosition    (glm::vec3& pos);   // updates model indirectly
    void setDirection   (glm::vec3& dir);   // updates model indirectly
    void setModel       (glm::mat4& model); // set directly
    void setLength      (float len);

  private:
    const size_t mNumOfVertices = 2;
    Shader*    mShader;
    glm::mat4  mModel;
    Camera&    mCamera;
    glm::vec3  mPos;
    glm::vec3  mDir;
    glm::vec3  mLength;
};
