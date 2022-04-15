#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// for debug / print
#include <glm/gtx/string_cast.hpp>
#include "textureManager.h"
#include "shader_s.h"
#include <stb_image.h>
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <optional>

// static float debugFunc(glm::vec3 L, glm::vec3 A0, glm::vec3 A1, glm::vec3 A2, glm::vec3 extent)
// {
//   float   R0 =  extent.x * (glm::dot(L, A0));
//           R0 += extent.y * (glm::dot(L, A1));
//           R0 += extent.z * (glm::dot(L, A2));
//   return  R0;
// }

/* 
  rotated unit vectors aligned with the objects bound box
*/
struct BoundBoxEdges
{
  glm::vec3 Ax;
  glm::vec3 Ay;
  glm::vec3 Az;
  glm::vec3 const& operator[](size_t idx) const
  {
    switch (idx)
    {
      case 0: return Ax;
      case 1: return Ay;
      case 2: return Az;
      default: 
        std::cout << "Error occured accessing BoundBoxEdges[]. Exiting!" << std::endl;
        exit(0);
    }
  }
  void toString()
  {
    std::cout 
      << "Ax:" << glm::to_string(Ax) << "\n"
      << "Ay:" << glm::to_string(Ay) << "\n"
      << "Az:" << glm::to_string(Az) << "\n";
  }
};

// TODO: should only be in collisionDetector
struct LastSeparatingAxis
{
  uint32_t index;
  glm::vec3 normal; // collision normal = last separating axis
};

struct CollisionPoint
{
  glm::vec3 x_rel; // relative to object A
  glm::vec3 y_rel; // relative to object B
  glm::vec3 p_abs; // absolute coordinates
};

// holds the complete state of the object
struct ObjectState
{
  glm::vec3 velocity;         // dx/dt
  glm::vec3 position;         //  x
  glm::vec3 rotationVelocity; // dw/dt
  glm::vec3 rotation;         //  w
};

class Object
{
public:

  enum class CollisionType
  {
    kPoint,
    kEdgeFace,
    kFaceFace
  };

  Object(glm::vec3 position, glm::vec3 scale, const char* name, float mass = 1.0f);

protected:

  ObjectState     mState;
  ObjectState     mRecState;
  glm::vec3       mScale;
  glm::vec3       mScaleRec;
  glm::mat4       mModel;
  glm::mat3       mInertiaTensor;
  const float     mMass;
  bool            mDebugOutputOn;
  bool            mIgnoreCollision;
  bool            mIsSelected = false;
  const char*     mName;
  BoundBoxEdges   mBoundBoxEdges; // optional ? - for selecting we really need this for now!
  TextureManager::Texture mTextureEnum{TextureManager::Texture::kInvalid};

  // utility function for loading a 2D texture from file
  // ---------------------------------------------------
  unsigned int loadTexture(char const * path);

public:
  // pure abstracts
  virtual void  Draw(Shader& shader)      = 0;
  virtual void  DrawInit()                = 0;
  virtual float ContainingRadius() const  = 0;

  // getters
  glm::vec3 GetPosition()           const { return mState.position; };
  glm::vec3 GetVelocity()           const { return mState.velocity; };
  glm::vec3 GetRotationVelocity()   const { return mState.rotationVelocity; };
  ObjectState GetObjectState()      const { return mState; }
  glm::vec3 const& GetEdge(uint32_t index) const;
  CollisionType GetCollisionType(const glm::mat3& C_ij, const LastSeparatingAxis& lsa) const;
  CollisionPoint GetCollisionPoint(LastSeparatingAxis const& lsa, glm::mat3 const& C_ij, Object const* obj) const;

  void CheckForErrors() const;
  bool IgnoreCollision();
  void printObject();
  void UpdateModel();
  void UpdatePosition(float const dt);
  void UpdateRotation(float const dt);
  // sets a fixed rotation, does not set or rotation velocity
  // rotation is a vector about which to rotate, and the magnitue the 
  // radians to rotate. Set as 0 length vector to set as non-rotated.
  void SetRotation(const glm::vec3& rotation)                   { mState.rotation = rotation;                 }
  void setVelocity(const glm::vec3& velocity)                   { mState.velocity = velocity;                 }
  void SetObjectState(ObjectState const& state)                 { mState = state;                             }
  void SetScale(const glm::vec3& scale)                         { mScale = scale;                             }
  void SetRelativeScale(const float scale)                      { mScale *= scale;                            }
  void RecordScale()                                            { mScaleRec = mScale;                         }
  void RestoreScale()                                           { mScale = mScaleRec;                         }
  void SetRotationVelocity(const glm::vec3& rotationVelocity)   { mState.rotationVelocity = rotationVelocity; }
  void SetPosition(glm::vec3 pos)                               { mState.position = pos;                      } // debug only!
  void SetDebugInfo(bool on);
  std::pair<bool, std::optional<glm::mat3>> CheckCollision(Object* obj, LastSeparatingAxis* lsa, bool* withinSphere);
  void CalcCollision(Object* obj, CollisionPoint& colPoint, glm::vec3& normal);
  bool CheckRayVsBoxCollision(glm::vec3 const& pRay, glm::vec3 const& dRay);
  const char* GetName() const { return mName; };
  void RecordState()     { mRecState = mState; }
  void RestoreRecState() { mState = mRecState; }
  bool IsSelected()      { return mIsSelected; }
  void SetIsSelected(bool const selected) { mIsSelected = selected; }
  auto ActivateTextureId(TextureManager* textureManager) { return textureManager->ActivateTexture(mTextureEnum); }
  bool SetTextureId(TextureManager::Texture textureEnum, TextureManager* textureManager);
  glm::mat4& GetModel()  { return mModel; }
  inline void CalcAbsPos(glm::vec3& p_abs, glm::vec3 const& p_rel, const Object* obj) const;

protected:
    glm::mat3 GetRotationMatrix() const;

private:
  // TODO: delete once refactor finished
  friend class CollisionDetector;

  virtual void UpdateBoundBox() = 0;

  glm::mat3 GetRotatedTensor() const;
};
