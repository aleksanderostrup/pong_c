#include "../include/box.h"

static int noOfBoxes = 0;
static unsigned int boxVAO, boxVBO;
// this should be selectable via a map
static unsigned int boxTexture;

Box::Box(glm::vec3 position, glm::vec3 scale, Shader* shader, const char* name, float mass) : 
  Object(position, scale, name, mass)
{
  this->mShader = shader;
  this->updateModel();
  noOfBoxes++;
  CreateBoundBox();
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

    // set default texture for this
    boxTexture = this->loadTexture("../textures/marble.jpg");
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
  delete mBoundBox;
  delete mBoundBoxNonRotated;
  delete mBoundBoxEdges;
}


/*
  make a selection between relative and absoulte draw (relative should be to previously drawn! - we need to make model member and reset it to 1.0f on absolute)

  also, we can re-use texture after first call... this should be called from updateScene
*/
void Box::drawInit()
{
  // boxs
  glBindVertexArray(boxVAO);
}

void Box::draw()
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, boxTexture);
  mShader->setMat4("model", mModel);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Box::CreateBoundBox()
{
  if (mBoundBox == nullptr && mBoundBoxNonRotated == nullptr)
  {
      // bound box
      mBoundBox           = new sBoundBox;
      mBoundBoxNonRotated = new sBoundBox;
      mBoundBoxEdges      = new sBoundBoxEdges;
      // populate non rotated
      glm::vec3 halfSide         = 0.5f * glm::abs(this->mScale);
      mBoundBoxNonRotated->bot11 = -halfSide;
      mBoundBoxNonRotated->bot12 = glm::vec3( halfSide.x, -halfSide.y, -halfSide.z);
      mBoundBoxNonRotated->bot13 = glm::vec3( halfSide.x, -halfSide.y,  halfSide.z);
      mBoundBoxNonRotated->bot14 = glm::vec3(-halfSide.x, -halfSide.y,  halfSide.z);
      mBoundBoxNonRotated->top11 = glm::vec3(-halfSide.x,  halfSide.y, -halfSide.z);
      mBoundBoxNonRotated->top12 = glm::vec3( halfSide.x,  halfSide.y, -halfSide.z);
      mBoundBoxNonRotated->top13 = halfSide;
      mBoundBoxNonRotated->top14 = glm::vec3(-halfSide.x,  halfSide.y,  halfSide.z);
  }
}

void Box::UpdateBoundBox()
{
  if ((nullptr == mBoundBox) || (nullptr == mBoundBoxNonRotated)) 
  {
    std::cout << "Grave grave error!" << std::endl;
    return;
  }
  // get upper 3x3 matrix (Rotation matrix) from the model matrix
  glm::mat3 rotM  = glm::mat3(mModel);
  // rotate all the points with the current rotation matrix 
  // and translate around object center
  mBoundBox->top11 = (rotM * mBoundBoxNonRotated->top11) + mPosition;
  mBoundBox->top12 = (rotM * mBoundBoxNonRotated->top12) + mPosition;
  mBoundBox->top13 = (rotM * mBoundBoxNonRotated->top13) + mPosition;
  mBoundBox->top14 = (rotM * mBoundBoxNonRotated->top14) + mPosition;
  mBoundBox->bot11 = (rotM * mBoundBoxNonRotated->bot11) + mPosition;
  mBoundBox->bot12 = (rotM * mBoundBoxNonRotated->bot12) + mPosition;
  mBoundBox->bot13 = (rotM * mBoundBoxNonRotated->bot13) + mPosition;
  mBoundBox->bot14 = (rotM * mBoundBoxNonRotated->bot14) + mPosition;
  // move to separate function?
  mBoundBoxEdges->Ax = mBoundBox->bot12 - mBoundBox->bot11;
  mBoundBoxEdges->Ay = mBoundBox->top11 - mBoundBox->bot11;
  mBoundBoxEdges->Az = mBoundBox->bot14 - mBoundBox->bot11;
  // they have to be unit vectors
  mBoundBoxEdges->Ax = glm::normalize(mBoundBoxEdges->Ax);
  mBoundBoxEdges->Ay = glm::normalize(mBoundBoxEdges->Ay);
  mBoundBoxEdges->Az = glm::normalize(mBoundBoxEdges->Az);
}


