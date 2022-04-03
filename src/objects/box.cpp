#include "box.h"
#include <cmath>

static int noOfBoxes = 0;
static unsigned int boxVAO, boxVBO;

static inline glm::mat3 CalcInertiaTensor(const glm::vec3& scale, const float mass)
{
  // formula refers to the whole side of the side of the cuboid
  const float h2     = std::pow(scale.y, 2);
  const float w2     = std::pow(scale.x, 2);
  const float d2     = std::pow(scale.z, 2);
  const glm::vec3 Ix = glm::vec3(h2 + d2, 0, 0);
  const glm::vec3 Iy = glm::vec3(0, w2 + d2, 0);
  const glm::vec3 Iz = glm::vec3(0, 0, w2 + h2);
  float realistic_feel_factor = 1.0f; 
  return realistic_feel_factor * (mass / 12 * glm::mat3(Ix, Iy, Iz));
}

Box::Box(glm::vec3 position, glm::vec3 scale, const char* name, float mass) : 
  Object(position, scale, name, mass)
{
  mInertiaTensor = CalcInertiaTensor(scale, mass);
  this->UpdateModel();
  noOfBoxes++;
  mBoundBoxEdges = new sBoundBoxEdges;
  UpdateBoundBox();
  // first box
  if (noOfBoxes == 1)
  {
    const float boxVertices[] = 
    {
      // positions          // texture Coords
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    std::cout << "Prepare VBO + VAO for Box class" << std::endl;
    // box VAO
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  }
}

Box::~Box()
{
  noOfBoxes--;
  if (noOfBoxes == 0)
  {
    std::cout << "Clean up data (VBO + VAO)" << std::endl;
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteBuffers(1, &boxVBO);
  }
  delete mBoundBoxEdges;
}


/*
  make a selection between relative and absoulte draw (relative should be to previously drawn! - we need to make model member and ref it to 1.0f on absolute)

  also, we can re-use texture after first call... this should be called from UpdateScene
*/
void Box::DrawInit()
{
  // boxs
  glBindVertexArray(boxVAO);
}

void Box::Draw(Shader& shader)
{
  shader.setMat4("model", mModel);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Box::UpdateBoundBox()
{
  // get upper 3x3 matrix (Rotation matrix) from the model matrix
  glm::mat3 rotM  { GetRotationMatrix() };
  // // they have to be unit vectors!
  mBoundBoxEdges->Ax = glm::normalize(rotM[0]);
  mBoundBoxEdges->Ay = glm::normalize(rotM[1]);
  mBoundBoxEdges->Az = glm::normalize(rotM[2]);
}


float Box::ContainingRadius() const
{
  // sides from center are 1/2. Using Pyth. Theorem, and squaring we get:
  // (a/2)^2 + (b/2)^2 + (c/2)^2 = (a^2 + b^2 + c^2) / 4 = r^2    <=>  r = sqrt(a<dot>a) / 2
  return (glm::sqrt((glm::dot(mScale, mScale))) / 2);
}

