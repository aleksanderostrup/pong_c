#include "plane.h"

int noOfPlanes = 0;
static unsigned int planeVAO, planeVBO;
// this should be selectable via a map
static unsigned int planeTexture;

Plane::Plane(glm::vec3 position, glm::vec2 xyscale, const char* name, float mass) : 
  Object(position, glm::vec3(xyscale.x, 1.0f, xyscale.y), name, mass)
{

  updateModel();
  noOfPlanes++;
  mIgnoreCollision = true;

  // first plane
  if (noOfPlanes == 1)
  {
    std::cout << "Prepare VBO + VAO for Plane class" << std::endl;

    const float planeVertices[] = {
      // positions (x,z,y) // texture Coords 
         1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f,

         1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
         1.0f, 0.0f, -1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //set default texture for this
    planeTexture = loadTexture("../textures/metal.png");
  }
}

Plane::~Plane()
{
  noOfPlanes--;
  if (noOfPlanes == 0) 
  {
    std::cout << "Clean up data (VBO + VAO)" << std::endl;
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
  }
}


/*
  make a selection between relative and absoulte draw (relative should be to previously drawn! - we need to make model member and reset it to 1.0f on absolute)

  also, we can re-use texture after first call... this should be called from updateScene
*/
void Plane::drawInit()
{
  // planes
  glBindVertexArray(planeVAO);
}

void Plane::draw(Shader& shader)
{
  glBindTexture(GL_TEXTURE_2D, planeTexture);
  // model = glm::mat4(1.0f);
  // MODEL CAN BE SCALED - FIND OUT THE BEST WAY TO DO THIS -> MAYBE HANDLE THIS IN object.h, like with Box
  // model = glm::scale(model, glm::vec3(10.0f));
  shader.setMat4("model", mModel);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Plane::UpdateBoundBox()
{
  // glm::vec3 halfSide = 0.5f * this->mScale;
  // todo: find correct way of calculating this!
  // botLeft  = mPosition - halfSide;
  // topRight = mPosition + halfSide;
}

float Plane::containingRadius()
{
  // determine better way to calc
  return 50000.0f;
}