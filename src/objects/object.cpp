#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object.h"
#include <array>


//! delete (moved to collisionDetector)
static inline float Sign(float const v)
{
  return (v < 0 ? -1.0f : 1.0f);
}

//! delete (moved to collisionDetector)
static float GetSigma(const glm::vec3& V, glm::vec3 const& D)
{
  return Sign(glm::dot(V, D));
}

//! delete (moved to collisionDetector)
class Counter
{
public:
  Counter(
    int const wrapVal,
    int const startVal        = 0,
    int const step            = 1,
    int const incrementEvery  = 1
  )
    : _wrapVal            (wrapVal)
    , _val                (startVal)
    , _step               (step)
    , _incrementEvery     (incrementEvery)
    , _inrementCountdown  (incrementEvery)
  {}

  Counter& operator++() // prefix increment
  {
    if (!(--_inrementCountdown))
    {
      _val += _step;
      _val %= _wrapVal;
      _inrementCountdown = _incrementEvery;
    }
    return *this;
  }

  operator int() const { return _val; }

private:
  int const   _wrapVal;
  int         _val  = 0;
  int const   _step = 1;
  int const   _incrementEvery = 1;
  int         _inrementCountdown;
};

Object::Object(glm::vec3 position, glm::vec3 scale, const char* name, float mass/*  = 1.0f */)
  : mScale             (scale)
  , mScaleRec          (scale)
  , mInertiaTensor     (glm::mat3(1.0f)) // good for cubes, but must be overwritten for other objects
  , mMass              (mass)
  , mDebugOutputOn     (false)
  , mIgnoreCollision   (false)
  , mName              (name)
{
  mState.velocity          = glm::vec3(0.0f);
  mState.position          = position;
  mState.rotationVelocity  = glm::vec3(0.0f, 0.0f, 0.0f);
  mState.rotation          = glm::vec3(0.0f, 0.0f, 0.0f);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int Object::loadTexture(char const * path)
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

void Object::CheckForErrors() const
{
  static const char* offending = "The offending command is ignored and has no other side effect than to set the error flag.";
  GLenum err;
  while((err = glGetError()) != GL_NO_ERROR)
  {
    std::cout << "Error found for " << mName << std::endl;
    switch (err)
    {
      // case GL_NO_ERROR:
      //   std::cout << "GL_NO_ERROR - No error has been recorded. The value of this symbolic constant is guaranteed to be 0." << std::endl;
      //   break;            
      case GL_INVALID_ENUM:
        std::cout << "GL_INVALID_ENUM - An unacceptable value is specified for an enumerated argument. " << offending << std::endl;
        break;            
      case GL_INVALID_VALUE:
        std::cout << "GL_INVALID_VALUE - A numeric argument is out of range. " << offending << std::endl;
        break;            
      case GL_INVALID_OPERATION:
        std::cout << "GL_INVALID_OPERATION - The specified operation is not allowed in the current state. " << offending << std::endl;
        break;            
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION - The framebuffer object is not complete. " << offending << std::endl;
        break;            
      case GL_OUT_OF_MEMORY:
        std::cout << "GL_OUT_OF_MEMORY - There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded." << std::endl;
        break;            
        // case GL_STACK_UNDERFLOW:
        //   std::cout << "GL_STACK_UNDERFLOW - An attempt has been made to perform an operation that would cause an internal stack to underflow." << std::endl;
        //   break;            
        // case GL_STACK_OVERFLOW:
        //   std::cout << "GL_STACK_OVERFLOW - An attempt has been made to perform an operation that would cause an internal stack to overflow.." << std::endl;
        //   break;
        default:
          std::cout << "Error (" << err << ") unknown." << std::endl;
    }
  }
}

bool Object::IgnoreCollision()
{
  return mIgnoreCollision;
}

void Object::printObject()
{
  std::string modelFormatted {glm::to_string(mModel)};
  const std::regex  re {R"(\)\,\s\()"};
  const std::string replStr = ")\n(";
  const std::regex  reStart {R"(\(\()"};
  const std::string replStrStart = "\n\(\(";
  modelFormatted = std::regex_replace(modelFormatted, re,      replStr);
  modelFormatted = std::regex_replace(modelFormatted, reStart, replStrStart);

  std::string boundBoxStr = "\n";
  boundBoxStr += "Ax: " + glm::to_string(mBoundBoxEdges.Ax) + "\n";
  boundBoxStr += "Ay: " + glm::to_string(mBoundBoxEdges.Ay) + "\n";
  boundBoxStr += "Az: " + glm::to_string(mBoundBoxEdges.Az) + "\n";

  std::cout << "name: "             <<  mName                                   << "\n"
            << "position: "         <<  glm::to_string(mState.position)         << "\n"
            << "scale: "            <<  glm::to_string(mScale)                  << "\n"
            << "velocity: "         <<  glm::to_string(mState.velocity)         << "\n"
            << "rotation: "         <<  glm::to_string(mState.rotation)         << "\n"
            << "rotationVelocity: " <<  glm::to_string(mState.rotationVelocity) << "\n"
            << "mass: "             <<  mMass                                   << "\n"
            << "model: "            <<  modelFormatted                          << "\n"
            << "debugOutputOn: "    <<  mDebugOutputOn                          << "\n"
            << "boundbox:\n"        <<  boundBoxStr                             << "\n"
            << std::endl;
}

void Object::UpdateModel()
{
  mModel = glm::mat4(1.0f);
  mModel = glm::translate(mModel, mState.position);
  float rotMag = glm::length(mState.rotation);
  if (rotMag > 0.0f)
  {
    mModel = glm::rotate(mModel, rotMag, glm::normalize(mState.rotation));
  }
  mModel = glm::scale(mModel, mScale);
}

void Object::UpdatePosition(float const dt)
{
  mState.position += mState.velocity * dt;
  if (mDebugOutputOn)
  {
    std::cout << glm::to_string(mState.position) << std::endl;
  }
  UpdateModel();
}

void Object::UpdateRotation(float const dt)
{
  mState.rotation += mState.rotationVelocity * dt;
  UpdateModel();
}

void Object::SetDebugInfo(bool on)
{

}

glm::vec3 const& Object::GetEdge(uint32_t index) const
{
  return mBoundBoxEdges[index];
}

// private func?
// check to see if the collision is face-face or edge-face
// TODO: WE NEED MORE CHECKING -for FACEFACE
// AS IT IS NOW, IT WILL NOT RECOGNIZE IF THE OBJECT IS ROTATED
// AROUND THE NORMAL TO THE FACE! 
// SHOULD WE CHECK THE LSA AGAINST THE C_ji MATRIX OR
// THE OTHER OBJECTS NORMALS? IF TWO OF THESE ARE PARALLEL, WE HAVE 
// A FACE FACE!
Object::CollisionType Object::GetCollisionType(const glm::mat3& C_ij, const LastSeparatingAxis& lsa) const
{
  for (int col = 0; col < C_ij.length(); col++)
  {
    for (int row = 0; row < C_ij[0].length(); row++)
    {
      // if any element is 0, it is either edge or face collision
      if (C_ij[col][row] == 0.0f)
      {
        for (int colCheck = 0; colCheck < 3; colCheck++)
        {
          // if last separating axis is parallel to any column in C_ij, it is a face-face collsiion
          if ((C_ij[colCheck] ==  lsa.normal) || 
              (C_ij[colCheck] == -lsa.normal))
          {
            std::cout << "Face-Face collision!" << std::endl;
            return CollisionType::kFaceFace;
          }
        }
        // if not it is an edge (but edge on WHICH?!)
        std::cout << "Edge-Face collision!" << std::endl;
        return CollisionType::kEdgeFace;
      }
    }
  }
  std::cout << "Point collision!" << std::endl;
  return CollisionType::kPoint;
}

// following inline functions are helper function for getting
// a collision point they
// should be private and maybe even in a separate class
// it is only valid for calculation done in [CDC]
inline void getijFromIndex(uint32_t index, size_t& i, size_t& j)
{
  if (index < 6)
  {
    return;
  }
  i = (index - 6) / 3;
  j = index % 3;
}

inline size_t getPerm(const size_t i, const size_t i0)
{
  const size_t sizeCols = 3;
  const size_t sizeRows = 3;
  if ((i0 >= sizeCols) || (i0 >= sizeRows) ||
      (i  >= sizeCols) || (i  >= sizeRows))
  {
    // HANDLE / REPORT ERROR HERE
    return 0;
  }
  const size_t permSet[sizeCols][sizeRows] = {
    {0, 1, 2},
    {1, 0, 2},
    {2, 1, 0}
  };
  return permSet[i][i0];
}

inline float& getPermValueLV(glm::vec3& V, const size_t i, const size_t i0)
{
  std::cout << getPerm(i, i0) << "\n";
  return V[getPerm(i, i0)];
}

inline float getPermValue(const glm::mat3& C_ij, const size_t i, const size_t i0, const size_t j, const size_t j0)
{
  const size_t i_tmp = getPerm(i, i0);
  const size_t j_tmp = getPerm(j, j0);
  // std::cout << "Input is: i,i0, j,j0: " << i << "," << i0 << "," << j << "," << j0 << "," << "\n";
  std::cout << "i,j: " << i_tmp << "," << j_tmp << "\n";
  std::cout << "Getting C_ij = C_" << i_tmp << "," << j_tmp << "\n";
  // we switch i and j compared to [CDC] because of its notation is opposite of the glm lib
  return C_ij[j_tmp][i_tmp];
}

inline float getPermValue(const glm::vec3& V, const size_t i, const size_t i0)
{
  return V[getPerm(i, i0)];
}

inline glm::vec3 const& getPermValue(BoundBoxEdges const& edges, const size_t i, const size_t i0)
{
  return edges[getPerm(i, i0)];
}

// sign as defined in [CDC p.6, bottom] - here we look up Sign(i, j)
inline float getSign(const size_t i, const size_t j)
{
  // Signs are
  // +1 for:
  // Sign(0,1)
  // Sign(1,2)
  // Sign(2,0)
  // -1 for:
  // Sign(1,0)
  // Sign(2,1)
  // Sign(0,2)
  if ((i == 0 && j == 1) ||
      (i == 1 && j == 2) ||
      (i == 2 && j == 0))
  {
    return 1.0f;
  }
  else if  ((j == 0 && i == 1) ||
            (j == 1 && i == 2) ||
            (j == 2 && i == 0))
  {
    return -1.0f;
  }
  else 
  {
    // HANDLE / REPORT ERROR HERE
    return 1.0f;
  }
}

inline float getPermSign(const size_t i, const size_t i0, const size_t j, const size_t j0)
{
  std::cout << "Input is: i,i0, j,j0: " << i << "," << i0 << "," << j << "," << j0 << "," << "\n";
  std::cout << "Perm is: " << getPerm(i, i0) << "," << getPerm(j, j0) << "\n";
  std::cout << "Sign is: " << getSign(getPerm(i, i0), getPerm(j, j0)) << "\n";
  return getSign(getPerm(i, i0), getPerm(j, j0));
}

inline void Object::CalcAbsPos(glm::vec3& p_abs, glm::vec3 const& p_rel, const Object* obj) const
{
  p_abs  = obj->mState.position;
  p_abs += p_rel[0] * obj->GetEdge(0);
  p_abs += p_rel[1] * obj->GetEdge(1);
  p_abs += p_rel[2] * obj->GetEdge(2);
}

/* 
  calculates the collision point
*/
CollisionPoint Object::GetCollisionPoint(LastSeparatingAxis const& lsa, glm::mat3 const& C_ij, Object const* obj) const
{
  const Object* A = this;
  const Object* B = obj;
  CollisionType collisionType = GetCollisionType(C_ij, lsa);
  CollisionPoint colPoint;

  // glm::vec3 a_i = 0.5f * A->mScale; // extent in [DCD Table 1, p. 5] is only half of the side length
  // glm::vec3 b_i = 0.5f * B->mScale;
  glm::vec3 const b_i = 0.5f * A->mScale; // extent in [DCD Table 1, p. 5] is only half of the side length
  glm::vec3 const a_i = 0.5f * B->mScale;
  glm::vec3 const C0 = A->mState.position; // center of A
  glm::vec3 const C1 = B->mState.position; // center of B
  glm::vec3 const D  = C1 - C0; // [DCD p. 6]
  std::cout << "LSA: " << lsa.index << std::endl;
  std::cout << "C_ij: " << glm::to_string(C_ij) << std::endl;
  uint32_t index = lsa.index % 3;
  float sigma = GetSigma(lsa.normal, D);
  if (lsa.index < 3)
  {
    
    colPoint.y_rel[0] = -sigma * Sign(C_ij[0][index]) * b_i[0];
    colPoint.y_rel[1] = -sigma * Sign(C_ij[1][index]) * b_i[1];
    colPoint.y_rel[2] = -sigma * Sign(C_ij[2][index]) * b_i[2];
    if ((CollisionType::kFaceFace == collisionType) || (CollisionType::kEdgeFace == collisionType))
    {
      float yj_min;
      float yj_max;
      float tmp;
      for (int j = 0; j < 3; j++)
      {
        tmp = -glm::dot(B->GetEdge(j), D);
        yj_min = tmp;
        yj_max = tmp;
        for (int k = 0; k < 3; k++)
        {
          tmp = glm::abs(C_ij[j][k]) * a_i[k];
          yj_min -= tmp;
          yj_max += tmp;
        }
        if (b_i[j] <= yj_max)
        {
          colPoint.y_rel[j] = b_i[j];
        }
        else if (-b_i[j] >= yj_min)
        {
          colPoint.y_rel[j] = -b_i[j];
        }
        else
        {
          colPoint.y_rel[j] = yj_min;
        } 
      }
    }
    CalcAbsPos(colPoint.p_abs, colPoint.y_rel, B);
    
    std::cout << "Collision point abs: "<< glm::to_string(colPoint.p_abs) << std::endl;
    std::cout << "Collision point rel: "<< glm::to_string(colPoint.y_rel) << std::endl;
  }
  else if (lsa.index < 6)
  {
    std::cout << "a_i: "<< glm::to_string(a_i) << std::endl;
    colPoint.x_rel.x = sigma * Sign(C_ij[index][0]) * a_i[0];
    colPoint.x_rel.y = sigma * Sign(C_ij[index][1]) * a_i[1];
    colPoint.x_rel.z = sigma * Sign(C_ij[index][2]) * a_i[2];
    CalcAbsPos(colPoint.p_abs, colPoint.x_rel, A);

    GetCollisionType(C_ij, lsa);
    std::cout << "Collision point abs: "<< glm::to_string(colPoint.p_abs) << std::endl;
    std::cout << "Collision point rel: "<< glm::to_string(colPoint.x_rel) << std::endl;
  }
  else  if (lsa.index < 15)
  {
    // CAN BE OPTIMIZED WITH A CONST EXPRESSION FOR ALL CASES -- MAKE ALL INLINE HELPER FUNCTION CONST EXPR
    size_t i, j;
    BoundBoxEdges const& A_edge = A->mBoundBoxEdges;
    BoundBoxEdges const& B_edge = B->mBoundBoxEdges;
    getijFromIndex(lsa.index, i, j);
    std::cout << "Before: Collision point rel: "<< glm::to_string(colPoint.x_rel) << std::endl;
    std::cout << "Before: Collision point abs: "<< glm::to_string(colPoint.p_abs) << std::endl;

    float sigma = GetSigma(lsa.normal, D);
    std::cout << "xi0 is: x";
    float& xi0 = getPermValueLV(colPoint.x_rel, i, 0);
    std::cout << "xi1 is: x";
    float& xi1 = getPermValueLV(colPoint.x_rel, i, 1);
    std::cout << "xi2 is: x";
    float& xi2 = getPermValueLV(colPoint.x_rel, i, 2);
    std::cout << "yj1 is: y";
    float& yj1 = getPermValueLV(colPoint.y_rel, j, 1);
    std::cout << "yj2 is: y";
    float& yj2 = getPermValueLV(colPoint.y_rel, j, 2);

    xi1 = getPermSign(i, 1, i, 0) * sigma * Sign(getPermValue(C_ij, i, 2, j, 0)) * getPermValue(a_i, i, 1);
    xi2 = getPermSign(i, 2, i, 0) * sigma * Sign(getPermValue(C_ij, i, 1, j, 0)) * getPermValue(a_i, i, 2);
    yj1 = getPermSign(j, 1, j, 0) * sigma * Sign(getPermValue(C_ij, i, 0, j, 2)) * getPermValue(b_i, j, 1);
    yj2 = getPermSign(j, 2, j, 0) * sigma * Sign(getPermValue(C_ij, i, 0, j, 1)) * getPermValue(b_i, j, 2);
    // large term, so we split it for better overview - starting with the inner most terms
    xi0  = -glm::dot(getPermValue(B_edge, j, 0), D) + getPermValue(C_ij, i, 1, j, 0) * xi1 + getPermValue(C_ij, i, 2, j, 0) * xi2;
    xi0 *=  getPermValue(C_ij, i, 0, j, 0); // note: multiplying
    xi0 +=  glm::dot(getPermValue(A_edge, i, 0) , D);
    xi0 +=  getPermValue(C_ij, i, 0, j, 1) * yj1;
    xi0 +=  getPermValue(C_ij, i, 0, j, 2) * yj2;
    std::cout << "denom simple: " << getPermValue(C_ij, i, 0, j, 0) << "\n";
    std::cout << "denom: " << (1.0f - pow(getPermValue(C_ij, i, 0, j, 0), 2)) << "\n";
    float tmp = 1.0f - pow(getPermValue(C_ij, i, 0, j, 0), 2);
    if (tmp)
    {
      xi0 /= tmp;
    }

    CalcAbsPos(colPoint.p_abs, colPoint.x_rel, A);
    std::cout << "After: Collision point rel: "<< glm::to_string(colPoint.x_rel) << std::endl;
    std::cout << "After: Collision point abs: "<< glm::to_string(colPoint.p_abs) << std::endl;
  }
  else
  {
    // ERROR HANDLING HERE
  }
  return colPoint;
}


static auto CalcRArrays(glm::mat3 const& C_ij, std::array<glm::vec3, 15> const& L, glm::vec3 const& D, glm::vec3 const& aScale, glm::vec3 const& bScale)
{
  // a_i and b_i is the length of the edges
  glm::vec3 const a_i { 0.5f * aScale }; // extent in [DCD Table 1, p. 5] is only half of the side length
  glm::vec3 const b_i { 0.5f * bScale };
  
  const size_t noOfDims   {  3 };
  const size_t noOfTests  { 15 };
  std::array<float, noOfTests> R0;
  std::array<float, noOfTests> R1;
  std::array<float, noOfTests> R;

  glm::mat3 C_ij_abs(1.0f);

  // OPTIMIZE: this should be done on the dot product instead!
  for (size_t n = 0; n < noOfDims; n++)
  {
    C_ij_abs[n][0] = glm::abs(C_ij[n][0]);
    C_ij_abs[n][1] = glm::abs(C_ij[n][1]);
    C_ij_abs[n][2] = glm::abs(C_ij[n][2]);
  }

  // note that col / row order is switched for glm lib compared to ref [DCD].
  R0[0]  = a_i[0];
  R0[1]  = a_i[1];
  R0[2]  = a_i[2];
  // We can access with [] or .x, .y, .z (fx, a_i[0] = a_i.x, etc.)
  R0[3]  = a_i[0] * C_ij_abs[0][0] + a_i[1] * C_ij_abs[0][1] + a_i[2] * C_ij_abs[0][2];
  R0[4]  = a_i[0] * C_ij_abs[1][0] + a_i[1] * C_ij_abs[1][1] + a_i[2] * C_ij_abs[1][2];
  R0[5]  = a_i[0] * C_ij_abs[2][0] + a_i[1] * C_ij_abs[2][1] + a_i[2] * C_ij_abs[2][2];
  R0[6]  = a_i[1] * C_ij_abs[0][2] + a_i[2] * C_ij_abs[0][1];
  R0[7]  = a_i[1] * C_ij_abs[1][2] + a_i[2] * C_ij_abs[1][1];
  R0[8]  = a_i[1] * C_ij_abs[2][2] + a_i[2] * C_ij_abs[2][1];
  R0[9]  = a_i[0] * C_ij_abs[0][2] + a_i[2] * C_ij_abs[0][0];
  R0[10] = a_i[0] * C_ij_abs[1][2] + a_i[2] * C_ij_abs[1][0];
  R0[11] = a_i[0] * C_ij_abs[2][2] + a_i[2] * C_ij_abs[2][0];
  R0[12] = a_i[0] * C_ij_abs[0][1] + a_i[1] * C_ij_abs[0][0];
  R0[13] = a_i[0] * C_ij_abs[1][1] + a_i[1] * C_ij_abs[1][0];
  R0[14] = a_i[0] * C_ij_abs[2][1] + a_i[1] * C_ij_abs[2][0];

  R1[0]  = b_i[0] * C_ij_abs[0][0] + b_i[1] * C_ij_abs[1][0] + b_i[2] * C_ij_abs[2][0];
  R1[1]  = b_i[0] * C_ij_abs[0][1] + b_i[1] * C_ij_abs[1][1] + b_i[2] * C_ij_abs[2][1];
  R1[2]  = b_i[0] * C_ij_abs[0][2] + b_i[1] * C_ij_abs[1][2] + b_i[2] * C_ij_abs[2][2];
  R1[3]  = b_i[0];
  R1[4]  = b_i[1];
  R1[5]  = b_i[2];
  R1[6]  = b_i[1] * C_ij_abs[2][0] + b_i[2] * C_ij_abs[1][0];
  R1[7]  = b_i[0] * C_ij_abs[2][0] + b_i[2] * C_ij_abs[0][0];
  R1[8]  = b_i[0] * C_ij_abs[1][0] + b_i[1] * C_ij_abs[0][0];
  R1[9]  = b_i[1] * C_ij_abs[2][1] + b_i[2] * C_ij_abs[1][1];
  R1[10] = b_i[0] * C_ij_abs[2][1] + b_i[2] * C_ij_abs[0][1];
  R1[11] = b_i[0] * C_ij_abs[1][1] + b_i[1] * C_ij_abs[0][1];
  R1[12] = b_i[1] * C_ij_abs[2][2] + b_i[2] * C_ij_abs[1][2];
  R1[13] = b_i[0] * C_ij_abs[2][2] + b_i[2] * C_ij_abs[0][2];
  R1[14] = b_i[0] * C_ij_abs[1][2] + b_i[1] * C_ij_abs[0][2];

  std::array<float, 6> dotProductArray;
  for (std::size_t i = 0; i < dotProductArray.size(); i++)
  {
    dotProductArray[i]  = glm::dot(L[i], D);
    R[i]                = glm::abs(dotProductArray[i]);
  }
  Counter counter1(3, 1, 1, 3);
  Counter counter2(3, 2, 1, 3);
  for (std::size_t i = 6; i < noOfTests; ++i, ++counter1, ++counter2)
  {
    R[i] = glm::abs((C_ij[i%3][counter1] * dotProductArray[counter2]) - 
                    (C_ij[i%3][counter2] * dotProductArray[counter1]));
  }

  return std::tuple( R0, R1, R );
}

void Object::CalcCollision(Object* obj, CollisionPoint& colPoint, glm::vec3& normal)
{
  double e = 1.0; // coefficient of restitution. Max 1 (perfect elastic). Min 0 (all kinetic energi lost as heat or deformation) 
  // see article 4 in docs/ and https://en.wikipedia.org/wiki/Collision_response#Impulse-based_contact_model
        
  // CoM (i.e., linear) velocities
  glm::vec3 v_lin_p1 = this->GetVelocity();
  glm::vec3 v_lin_p2 = obj->GetVelocity();
  // get center to point of contact vector
  glm::vec3 r_1  = colPoint.p_abs - this->mState.position;
  glm::vec3 r_2  = colPoint.p_abs - obj->mState.position;
  // get angular velocities
  glm::vec3 w_1  = this->mState.rotationVelocity;
  glm::vec3 w_2  = obj->mState.rotationVelocity;

  // get masses
  const float m1 = this->mMass;
  const float m2 = obj->mMass;

  // calculate v_p1 and v_p2 AT THE POINT OF CONTACT, so if they are rotating we have:
  glm::vec3 v_p1 = v_lin_p1 + glm::cross(w_1, r_1);
  glm::vec3 v_p2 = v_lin_p2 + glm::cross(w_2, r_2);

  // calculate relative velocity vr = v_p1 - v_p2
  glm::vec3 vr = v_p2 - v_p1;
  glm::vec3 n = glm::normalize(normal);
  
  // calculate the impulse magnitude nominator
  double jr_nom = -(1 + e) * (glm::dot(vr, n));
  // calculate the impulse magnitude denominator
  double jr_denom = 1 / m1 + 1 / m2;
  glm::mat3 const I1 = this->GetRotatedTensor();
  glm::mat3 const I2 = obj->GetRotatedTensor();
  glm::vec3 const I1_calc = glm::inverse(I1) * (glm::cross(r_1, n));
  glm::vec3 const I2_calc = glm::inverse(I2) * (glm::cross(r_2, n));
  glm::vec3 tmp = glm::cross(I1_calc, r_1);
  tmp          += glm::cross(I2_calc, r_2);
  jr_denom     += glm::dot(tmp, n);
  double const jr = jr_nom / jr_denom;
  
  glm::vec3 vjr = ((float)jr) * n;

  // calculate new velocities
  glm::vec3 v_p1a = v_lin_p1 - vjr / m1;
  glm::vec3 v_p2a = v_lin_p2 + vjr / m2;

  // calculate new angular velocities
  glm::vec3 w_1a = w_1 - ((float) jr) * I1_calc;
  glm::vec3 w_2a = w_2 + ((float) jr) * I2_calc;

  std::cout << "Collision between: "  << this->mName << " and " << obj->mName << std::endl;
  std::cout << "colpoint: "           << glm::to_string(colPoint.p_abs) << std::endl;
  std::cout << "normal: "             << glm::to_string(n) << std::endl;
  std::cout << "vr: "                 << glm::to_string(vr) << std::endl;
  std::cout << "r1: "                 << glm::to_string(r_1) << std::endl;
  std::cout << "r2: "                 << glm::to_string(r_2) << std::endl;
  std::cout << "w1: "                 << glm::to_string(w_1) << std::endl;
  std::cout << "w2: "                 << glm::to_string(w_2) << std::endl;
  std::cout << "w1a: "                << glm::to_string(w_1a) << std::endl;
  std::cout << "w2a: "                << glm::to_string(w_2a) << std::endl;
  std::cout << "jr: "                 << jr << std::endl;
  std::cout << "I1_c: "               << glm::to_string(I1_calc) << std::endl;
  std::cout << "I2_c: "               << glm::to_string(I2_calc) << std::endl;

  // update linear velocities
  this->setVelocity(v_p1a);
  obj->setVelocity(v_p2a);

  // update angular velocities
  this->SetRotationVelocity(w_1a);
  obj->SetRotationVelocity(w_2a);

}

bool Object::CheckRayVsBoxCollision(glm::vec3 const& pRay, glm::vec3 const& dRay)
{
  // TODO: factor out this call (can they be done before this). Then make this function const
  UpdateBoundBox();
  glm::vec3 PmC = pRay - mState.position;
  float DdU[3];
  float ADdU[3];
  float PmCdU[3];
  float APmCdU[3];
  // notation from https://www.geometrictools.com/Documentation/IntersectionLineBox.pdf
  glm::vec3 a_i     = 0.5f * mScale; // extent
  BoundBoxEdges& U = mBoundBoxEdges;
  
  // The ray = specific tests .
  for ( int i = 0 ; i < 3 ; ++i )
  {
    DdU[i]    = glm::dot(dRay, U[i]);
    ADdU[i]   = glm::abs(DdU[i]);
    PmCdU[i]  = glm::dot(PmC, U[i]);
    APmCdU[i] = glm::abs(PmCdU[i]);
    if (APmCdU[i] > a_i[i] && PmCdU[i] * DdU[i] >= 0)
    {
    return false;
    }
  }
  // The line = specific tests.
  glm::vec3 DxPmC = glm::cross (dRay, PmC);
  float ADxPmCdU[3];
  ADxPmCdU[0] = glm::abs(glm::dot(DxPmC, U[0] ));
  if (ADxPmCdU[0] > a_i[1] * ADdU[2] + a_i[2] * ADdU[1] )
  {
    return false;
  }
  ADxPmCdU[1] = glm::abs(glm::dot(DxPmC, U[1] ));
  if (ADxPmCdU[1] > a_i[0] * ADdU[2] + a_i[2] * ADdU[0] )
  {
    return false;
  }
  ADxPmCdU[2] = glm::abs(glm::dot(DxPmC, U[2] ));
  if (ADxPmCdU[2] > a_i[0] * ADdU[1] + a_i[1] * ADdU[0] )
  {
    return false;
  }
  return true;
}

bool Object::SetTextureId(TextureManager::Texture textureEnum, TextureManager* textureManager)
{
  if (mTextureEnum != TextureManager::Texture::kInvalid)
  {
    textureManager->DeregisterTexture(textureEnum);
  }
  mTextureEnum = textureEnum;
  return textureManager->RegisterTexture(textureEnum);
}

glm::mat3 Object::GetRotationMatrix() const
{
  float rotMag = glm::length(mState.rotation);
  glm::mat4 rotM = glm::mat4(1.0f);
  if (rotMag > 0.0f)
  {
    rotM = glm::rotate(rotM, rotMag, glm::normalize(mState.rotation));
  }
  return glm::mat3(rotM);
}

glm::mat3 Object::GetRotatedTensor() const
{
  glm::mat3 rotM { GetRotationMatrix() };
  return (rotM * mInertiaTensor * glm::transpose(rotM));
}