#pragma once

#include <map>
#include <string>
#include "object.h"
#include "textureManager.h"
#include "camera.h"
#include "skybox.h"
#include "line.h"

struct ColPointDbg
{
  static const size_t frameCntInit = 5;

  ColPointDbg(Camera const& camera);
  
  size_t    mFrameCnt = 0;
  Line      mColVec;
  
  void SetColPoint(glm::vec3& p, glm::vec3& dir);
  void Draw(glm::mat4& projection, bool const isPaused);
};

class Scene
{
public:
  
  Scene(float const gridsize, Camera const& camera, uint32_t const& width, uint32_t const& height);

  void UpdateScene(float const deltaTime);
  void ForceFrameForward(float deltaTime);
  void FrameBackward();
  void StupidDebug();
  void TogglePause() { _isPaused = !_isPaused; };
  bool IsPaused() const  { return _isPaused; };
  void ModifyTime(float const modifier) { _timeMultiplier *= modifier; }
  void SaveScene();
  void RestoreScene();
  void ToggleFrameForwardDebug() { _frameForwardDebug = !_frameForwardDebug; }

  void PrintSummedVelAndRot() const;
  void AddObject(Object* obj, TextureManager::Texture textureEnum = TextureManager::Texture::kDan);
  void setColPointDebugObject(Object* obj);
  void SetColNormalDebugObject(std::vector<Object*>& obj, size_t index);
  void PrintInfoForSelected() const;

private:

  // using ObjectPtrType = std::unique_ptr<Object>;

  enum class CollisionDetectionStop 
  {
    kStopOnFirst    = 1,
    kTrackAllLSAs   = 2,
    kHandleAll      = 3
  };

  float                                         _gridsize;
  bool                                          _isPaused;
  float                                         _timeMultiplier = 1.0f; // speed time up or down
  std::vector<std::pair<ObjectState, Object*>>  _sceneSnapshot;
  std::vector<Object*>                          _objects;
  std::vector<Object*>                          _selectedObjects;
  Object*                                       _colDebugObject = nullptr;
  std::vector<Object*>                          _colNormalDebugObjectVector;
  std::vector<Object*>                          _colNormalDebugObjectVector2;
  std::map<std::string, LastSeparatingAxis>     _lastSeparatingAxis;
  TextureManager                                _textureManager;
  Skybox                                        _skybox;
  Camera const&                                 _camera;
  uint32_t const&                               _width;
  uint32_t const&                               _height;
  Shader                                        _shader;
  Shader                                        _shaderSingleColor;
  ColPointDbg                                   _colPointDbg;
  bool                                          _stupidDebugThingStopOnFirst = false;
  bool                                          _frameForwardDebug = false;

  // helper function 
  void InsertLSA(std::string& concName, LastSeparatingAxis& lsa);
  void DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(glm::vec3& colPoint, glm::vec3& colNormal);
  void DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT2(glm::vec3& colPoint, glm::vec3& colNormal);
  bool DetectCollisions(CollisionDetectionStop const stopWhen);
  void UpdatePos(float deltaTime);
  void FrameForward(float deltaTime);
  void FrameForwardNormal(float const deltaTime);
  void FrameForwardDebug(float const deltaTime);
  void DrawObjects();
  void DrawDebugObject();
  void DrawOverlay();
  void StepBackObjects();
  // not completely finished - there are edge cases where we don't select the right element!
  // (example: big object that is nearer, but has part of it behind the intended object, and the ray passing through this part)
  void SelectNearestObjectPointedAt();
};

