#include "line.h"
#include <cmath>

static int noOfLines = 0;
static unsigned int lineVAO[3];
static unsigned int lineVBO[3];


/* 
   To implement:
    - should be an object ?
    - different thickness / colors (fragment shaders should have color uniform)
    - possibility to choose which axis to draw (set with mask?)
    - shader can be optional by passing either a shader or a nullptr to the class
      -- if not passed, the calling function must handle the shader part, and be sure the correct one is selected!
    - make easy to rotate to align with a given vector!
    
   */
  
Line::Line(glm::vec3 position, glm::vec3 direction, float length, Camera const& camera)
  : mCamera(camera)
  , mLength(1.0f, 1.0f, 1.0f)
{
  noOfLines++;
  mModel = glm::mat4(1.0);
  SetPosition(position);
  setDirection(direction);
  setLength(length);

  std::cout << "GLM model = " << glm::to_string(mModel) << "\n";

  static Shader s("../shaders/lineshader.vs", "../shaders/overlayshader.fs");
  static bool init = true;
  const size_t itemsPerVertice = 3;

  // first line
  if (noOfLines == 1)
  {
    if (init)
    {
      float line[] = 
      {
        // positions  
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, // IF CHANGING THIS , ALSO CHANGE DIRECTION CHANGE ALGO!
      };

      // static_assert(numOfVertices == (sizeof(line) / (sizeof(line[0]) * itemsPerVertice)));

      // box VAO
      glGenVertexArrays(3, &lineVAO[0]);
      glGenBuffers(3, &lineVBO[0]);
      for (size_t i = 0; i < 3; i++)
      {
        if (i == 1)
        {
          line[itemsPerVertice]     = 0.0f;
          line[itemsPerVertice + 1] = 1.0f;
        }
        else if (i == 2)
        {
          line[itemsPerVertice + 1] = 0.0f;
          line[itemsPerVertice + 2] = 1.0f;
        }
        glBindVertexArray(lineVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(line), &line, GL_STATIC_DRAW);
        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
      }
      init = false;
      std::cout << "Prepare VBO + VAO for line class" << std::endl;
    }
  }

  mShader = &s;
}

Line::~Line()
{
  noOfLines--;
  if (noOfLines == 0)
  {
    std::cout << "Clean up data (VBO + VAO)" << std::endl;
    for (size_t i = 0; i < 3; i++)
    {
      glDeleteVertexArrays(1, &lineVAO[i]);
      glDeleteBuffers(1, &lineVBO[i]);
    }
    delete mShader;
  }
}


// void Line::drawInit()
// {
//   // lines
//   glBindVertexArray(lineVAO);
// }

void Line::Draw(Shader* shader, glm::mat4& projection)
{
  if (nullptr == shader)
  {
      shader = mShader;
  }
  shader->use();
  glm::mat4 view = mCamera.GetViewMatrix();
  
  shader->setMat4("model", mModel);
  shader->setMat4("view", view);
  shader->setMat4("projection", projection);
  
  for (size_t i = 0; i < 1; i++) // REMEMBER!: set to < 3 to draw all perpendicular 
  {
    glBindVertexArray(lineVAO[i]);
    glDrawArrays(GL_LINE_STRIP, 0, mNumOfVertices);
  }
}

void Line::SetPosition(glm::vec3& pos)
{
  // mModel = glm::translate(mModel, pos);
  mModel[3][0] = pos.x;
  mModel[3][1] = pos.y;
  mModel[3][2] = pos.z;
  mPos = pos;
}

void Line::setDirection(glm::vec3& dir)
{
  // following:
  // https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
  
  const glm::vec3 u(1.0f, 0.0f, 0.0f); // vector defined above!
  const glm::vec3& A{u};
  const glm::vec3 B{glm::normalize(dir)};

  float AdotB = glm::dot(A, B);
  auto AcrossBNorm = glm::length(glm::cross(B, A));

  const glm::vec3 G0(AdotB, -AcrossBNorm, 0);
  const glm::vec3 G1(AcrossBNorm, AdotB, 0);
  const glm::vec3 G2(0, 0, 1);
  glm::mat3 G((G0),(G1),(G2));
  G = glm::transpose(G);
  
  glm::vec3 v = (B - glm::dot(A, B) * A);
  if (glm::length(v) > 0)
  {
    v = glm::normalize(v);
    glm::vec3 w = glm::cross(B, A);

    glm::mat3 F(u, v, w);
    F = glm::transpose(F);
    auto U = glm::inverse(F) * G * F;
    
    mModel = glm::mat4(U);

    // the direction and position are NOT set right --
    // get the model right 
    // -- is the roatation matrix correctly placed?
    // -- what about position?
    // -- should we set the last (4th) coordinate to 1.0f?
    // -- what about scaling? 
    // -- and order of rotation / translation / scaling ?
    std::cout << "Rotate to " << glm::to_string(B) << "\n";
    std::cout << "Rotate calced " << glm::to_string(U * u) << "\n";
    // std::cout << "Rotation matrix " << glm::to_string(U) << "\n";

    // std::cout << "F_inv = " << glm::to_string(glm::inverse(F)) << "\n";
    // std::cout << "G = " << glm::to_string(G) << "\n";
    // std::cout << "F_inv * G = " << glm::to_string(glm::inverse(F) * G) << "\n";
    // std::cout << "F = " << glm::to_string(F) << "\n";
    // std::cout << "U = " << glm::to_string(U) << "\n";
    // std::cout << "Tried to set dir = " << glm::to_string(mDir) << "\n";
    // std::cout << "Tried to set pos = " << glm::to_string(mPos) << "\n";
  }
  else
  {
    mModel = glm::mat4(1.0f);
  }


  SetPosition(mPos);
  // mModel = glm::scale(mModel, mLength);
  mDir = B; // store new direction
}

void Line::setLength(float len)
{
  mLength = glm::vec3(len, len, len);
}
