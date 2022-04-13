#include "scene.h"


ColPointDbg::ColPointDbg(Camera const& camera)
  : mColVec(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 3.0f, camera)
{}
  
void ColPointDbg::SetColPoint(glm::vec3& p, glm::vec3& dir)
{
  mColVec.SetPosition(p);
  mColVec.setDirection(dir);
  mFrameCnt = frameCntInit;
}

void ColPointDbg::Draw(glm::mat4& projection, bool const isPaused)
{
  if (mFrameCnt)
  {
    mColVec.Draw(nullptr, projection);
    if (!isPaused)
      mFrameCnt--;
  }
}

Scene::Scene(float const gridsize, Camera const& camera, uint32_t const& width, uint32_t const& height)
  : _skybox            (camera, width, height)
  , _camera            (camera)
  , _width             (width)
  , _height            (height)
  , _shader            ("../shaders/shader.vs", "../shaders/shader.fs")
  , _shaderSingleColor ("../shaders/shader.vs", "../shaders/singlecolorshader.fs")
  , _colPointDbg       (camera)
  , _gridsize          (gridsize)
  , _isPaused          (false)
{
  _shader.use();
  _shader.setInt("texture1", 0);
}

void Scene::ForceFrameForward(float const deltaTime)
{
  if (IsPaused())
  {
    TogglePause();
    FrameForward(deltaTime);
    TogglePause();
  }
}

void Scene::SaveScene()
{
  _sceneSnapshot.clear();
  for (auto const& o : _objects)
    _sceneSnapshot.push_back({o->GetObjectState(), o});
}

void Scene::RestoreScene()
{
  for (auto& [state, o] : _sceneSnapshot)
  {
    o->SetObjectState(state);
    o->UpdateModel();
  }
}

void Scene::FrameBackward()
{
  if (IsPaused())
  {
    TogglePause();
    RestoreScene();
    TogglePause();
  }
}


void Scene::FrameForward(float const deltaTime)
{
  if (_frameForwardDebug)
  {
    FrameForwardDebug(deltaTime);
  }
  else
  {
    FrameForwardNormal(deltaTime);
  }
}

struct FrameForwardState
{

  enum class State
  {
    Stopped,
    TimeDividing_Step1,
    TimeDividing_Step2,
    CheckGraunlarity,
    HandleAllCollisions,
    CheckFinished
  };

  void Reset(float const deltaTime)
  {
    inProgress      = true;
    timeLeft        = deltaTime;
    updateTimeGran  = deltaTime;
    maxTries        = 50;
    resolvedNumberForDebug = 0;
    allWasHandled   = false;
  }

  // getters
  bool  InProgress() const { return inProgress; }
  float GetTimeGranularity() const { return updateTimeGran; }
  bool  MaxDepthReached() const { return (divDepth == divDepthMax); }
  bool  TimeLeftLessThanGranularity() const { return (timeLeft <= updateTimeGran); }
  float GetTimeLeft() const { return timeLeft; }
  float GetDivDepth() const { return divDepth; }

  void ResetDivDepth() { divDepth = 0; }
  void IncrDivDepth() {  divDepth++; }
  void SetResolved(bool const resolved)   { inProgress = !resolved; }
  void DivDepth()
  {
    updateTimeGran /= 2;
    divDepth++;
  }
  void UpdateTime()
  {
    timeLeft -= updateTimeGran;
    std::cout << "Time left: " << timeLeft << std::endl;
  }
  void SetUpdateTimeGranToTimeLeft()
  {
    updateTimeGran = timeLeft;
  }
  bool UpdateMaxTries()
  {
    return (maxTries-- == 0);
  }

  State state = State::Stopped;
  bool  allWasHandled; // for checking if we did the HandleAllCollisions state

private:
    bool inProgress = false;
    bool colDetected;
    float timeLeft;
    float updateTimeGran;
    

    size_t const divDepthMax = 6;
    size_t maxTries;                    // if we somehow get stuck
    size_t resolvedNumberForDebug = 0;  // if we somehow get stuck
    size_t divDepth;
};

// TODO: we should disable or at least handle frameBackwards
void Scene::FrameForwardDebug(float deltaTime)
{
  static FrameForwardState stateFF;
  using State = FrameForwardState::State;
  // SaveScene();

  switch (stateFF.state)
  {
    case State::Stopped:
    {
      std::cout << "Starting machine\n";
      deltaTime *= _timeMultiplier;
      std::cout << "Updating position\n";
      stateFF.Reset(deltaTime);
      UpdatePos(deltaTime);
      // if no detection, we resolve
      stateFF.SetResolved(!DetectCollisions(CollisionDetectionStop::kStopOnFirst));
    
      stateFF.state = stateFF.InProgress() ? State::TimeDividing_Step1 : State::Stopped;
      break;
    }
    case State::TimeDividing_Step1:
    {
      std::cout << "Resetting depth\n";
      stateFF.ResetDivDepth();
      stateFF.state = State::TimeDividing_Step2;
    } [[fallthrough]];
    case State::TimeDividing_Step2:
    {
      std::cout << "TimeDividing_Step2 -- stepping back objects\n";
      // 1) go to previous position
      StepBackObjects();
      // 2) sub-divide
      // updateTimeGran /= 2;
      // divDepth++;
      stateFF.DivDepth();
      std::cout << "Updating position\n";
      // 3) move objects forward again, but only with half the timestep
      UpdatePos(stateFF.GetTimeGranularity());
      std::cout << "Div depth is : " << stateFF.GetDivDepth() << " dt = " << stateFF.GetTimeGranularity() << std::endl;
      stateFF.SetResolved(!DetectCollisions(CollisionDetectionStop::kStopOnFirst));

      if (!stateFF.InProgress() ||        // keep dividing while we have a collision AND
          (stateFF.MaxDepthReached()))    // not reached the max binary depth
      {
        
        stateFF.state = State::CheckGraunlarity;
      }
      break;
    }
    case State::CheckGraunlarity:
    {
      std::cout << "Checking granularity\n";
      if (stateFF.TimeLeftLessThanGranularity())
      {
        std::cout << "Resolved here\n";
        stateFF.SetResolved(true);
        // the whole timesteps (deltaTime) has been updated
        stateFF.state = State::Stopped;
      }
      else
      {
        stateFF.state = State::HandleAllCollisions;
        stateFF.UpdateTime();
      }
      break;
    }
    case State::HandleAllCollisions:
    {
      std::cout << "HandleAllCollisions\n";
      if (stateFF.InProgress())
      {
        stateFF.allWasHandled = true;
        // resolvedNumberForDebug++;
        // resolve the collisions
        if (!DetectCollisions(CollisionDetectionStop::kHandleAll))
        {
          std::cout << "UNEXPECTED!\n";
        }

      }
      stateFF.state = State::CheckFinished;
      break;
    }
    case State::CheckFinished:
    {
      std::cout << "Check finished\n";
      // updateTimeGran = timeLeft;
      stateFF.SetUpdateTimeGranToTimeLeft();
      std::cout << "Updating position\n";
      // try to move to the end of the timestep
      UpdatePos(stateFF.GetTimeLeft());
      if (!DetectCollisions(CollisionDetectionStop::kStopOnFirst))
      {
        stateFF.SetResolved(true);
        stateFF.state = State::Stopped;
      }
      else if (stateFF.UpdateMaxTries())
      {
        std::cout << "EMERGENCY BREAK!! Timeleft: " << stateFF.GetTimeLeft() << std::endl;
        stateFF.SetResolved(true); // TODO: set with error?
        stateFF.state = State::Stopped;
        if (!stateFF.allWasHandled)
        {
          std::cout << "Wasn't handled!\n";
          exit(0);
        }
      }
      else
      {
        stateFF.state = State::TimeDividing_Step1;
        stateFF.allWasHandled = false;
      }
      break;
    }
  }
}

void Scene::FrameForwardNormal(float deltaTime)
{
  SaveScene();
  size_t totalLoops = 0;
  deltaTime *= _timeMultiplier;
  UpdatePos(deltaTime);
  // do not calculate, just check
  if (DetectCollisions(CollisionDetectionStop::kStopOnFirst))
  {
    bool colDetected;
    float timeLeft = deltaTime;
    float updateTimeGran = deltaTime;
    // controls the detection granularity
    // could be made dependent on fastest relative collision speed for even better performance
    size_t const divDepthMax = 6;
    size_t maxTries = 50; // if we somehow get stuck
    size_t resolvedNumberForDebug = 0; // if we somehow get stuck

    while (true)
    {
      // reset depth
      size_t divDepth = 0; // 0 = full time step
      // bool breakNow = false; // FOR DEBUGGING
      do
      {
        totalLoops++; // DEBUG TEST
        // 1) go to previous position
        StepBackObjects();
        // 2) sub-divide
        updateTimeGran /= 2;
        divDepth++;
        // 3) move objects forward again, but only with half the timestep
        UpdatePos(updateTimeGran);
        std::cout << "Div depth is : " << divDepth << " dt = " << updateTimeGran << std::endl;
      }
      while ((colDetected = DetectCollisions(CollisionDetectionStop::kStopOnFirst)) && // keep dividing while we have a collision AND
              (divDepth != divDepthMax));                       // not reached the max binary depth
      if (timeLeft <= updateTimeGran)
      {
        // the whole timesteps (deltaTime) has been updated
        break;
      }
      timeLeft -= updateTimeGran;
      std::cout << "Time left: " << timeLeft << std::endl;
      if (colDetected)
      {
        resolvedNumberForDebug++;
        // resolve the collisions
        if (!DetectCollisions(CollisionDetectionStop::kHandleAll))
        {
          std::cout << "UNEXPECTED!\n";
        }
        else
        {
          float colStartTime = (deltaTime - timeLeft);
          std::cout << "Collision between " << colStartTime << " - " << (colStartTime + updateTimeGran) << std::endl;
        }
      }
      updateTimeGran = timeLeft;
      // try to move to the end of the timestep
      UpdatePos(timeLeft);
      if (!DetectCollisions(CollisionDetectionStop::kStopOnFirst))
      {
        // if no collisions to resolve, exit at the final time step
        break;
      }
      if (maxTries-- == 0)
      {
        // TODO: if we're frame forwarding we should NOT set this
        _isPaused = true;
        std::cout << "EMERGENCY BREAK!! Timeleft: " << timeLeft << std::endl;
        // emergency break - something went wrong!
        break;
      }
    }
    std::cout << "Total loops: " <<  totalLoops << "  RESOLVED " << resolvedNumberForDebug << "\n";
  }
}

void Scene::UpdateScene(float const deltaTime)
{
  if (!_isPaused) 
  {
    FrameForward(deltaTime);
  }

  glm::mat4 projection = glm::perspective(glm::radians(_camera.GetZoom()), (float)_width / (float)_height, 0.1f, 100.0f);
  glm::mat4 view = _camera.GetViewMatrix();

  _shaderSingleColor.use();
  _shaderSingleColor.setMat4("view", view);
  _shaderSingleColor.setMat4("projection", projection);

  _shader.use();
  _shader.setMat4("projection", projection);
  _shader.setMat4("view", view);

  
  DrawObjects();
  _colPointDbg.Draw(projection, _isPaused);
  _shader.use();
  DrawDebugObject(); // TODO: implement more elegantly than there!
  _shader.use();
  DrawOverlay();
  

  // Draw skybox as last
  _skybox.Draw();
}

void Scene::StupidDebug()
{
  // stupidDebugThingStopOnFirst = !stupidDebugThingStopOnFirst;
  // std::cout << "Cam.. Pos: " << glm::to_string(_camera.Position) << " Front: " << glm::to_string(_camera.Front) << "\n";
  SelectNearestObjectPointedAt();
}

void Scene::PrintSummedVelAndRot() const
{
  glm::vec3 totalVel(0.0f);
  glm::vec3 totalRotVel(0.0f);
  for (auto const& o : _objects)
  {
    totalVel    += o->GetVelocity();
    totalRotVel += o->GetRotationVelocity();
  }
  std::cout << "Total vel     = " << glm::to_string(totalVel)     << "|" << std::to_string(glm::length(totalVel))     << "|\n";
  std::cout << "Total rot_vel = " << glm::to_string(totalRotVel)  << "|" << std::to_string(glm::length(totalRotVel))  << "|\n";
}

void Scene::AddObject(Object* obj, TextureManager::Texture textureEnum/*  = TextureManager::kDan */)
{
  std::string concName;
  for (auto it = _objects.begin() ; it != _objects.end(); ++it)
  {
    if (strcmp((*it)->GetName(), obj->GetName()) == 0)
    {
      std::cout << "Error, name " << obj->GetName() << " already exists - aborting program." << std::endl;
      exit(0);
    }
  }
  obj->SetTextureId(textureEnum, &_textureManager);
  _objects.push_back(obj);
}

void Scene::setColPointDebugObject(Object* obj)
{
  // FOR DEBUGGING - FIND MORE ELEGANT WAY TO DO THIS
  _colDebugObject = obj;
  _colDebugObject->SetTextureId(TextureManager::Texture::kMetal, &_textureManager);
}

void Scene::SetColNormalDebugObject(std::vector<Object*>& obj, size_t index)
{
  // FOR DEBUGGING - FIND MORE ELEGANT WAY TO DO THIS
  switch (index)
  {
    case 0: _colNormalDebugObjectVector = obj;
    case 1: _colNormalDebugObjectVector2 = obj;
  }
}

void Scene::PrintInfoForSelected() const
{
  std::cout << "\n----- ************************ -----\n"
            <<   "----- Info for selected object -----\n"
            <<   "----- ************************ -----\n";
  for (auto& o : _selectedObjects)
  {
    o->printObject();
  }
}

// helper function 
void Scene::InsertLSA(std::string& concName, LastSeparatingAxis& lsa)
{
  if (false == _lastSeparatingAxis.insert({concName, lsa}).second) // if already inserted...
    _lastSeparatingAxis[concName] = lsa;                           // ...overwrite
}

void Scene::DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(glm::vec3& colPoint, glm::vec3& colNormal)
{
  _colPointDbg.SetColPoint(colPoint, colNormal);
  if (_colDebugObject != nullptr)
  {
    _colDebugObject->SetPosition(colPoint);
    _colDebugObject->UpdateModel();
    float normalLengthMax = 3.0f;
    float normalStep = normalLengthMax / _colNormalDebugObjectVector.size();
    float normalCur = 0.0f;
    for (auto& pObj : _colNormalDebugObjectVector)
    { 
      pObj->SetPosition(colPoint + normalCur * colNormal);
      pObj->UpdateModel();
      normalCur += normalStep;
    }
  }
}

void Scene::DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT2(glm::vec3& colPoint, glm::vec3& colNormal)
{
  float normalLengthMax = 3.0f;
  float normalStep = normalLengthMax / _colNormalDebugObjectVector.size();
  float normalCur = 0.0f;
  for (auto& pObj : _colNormalDebugObjectVector2)
  { 
    pObj->SetPosition(colPoint + normalCur * colNormal);
    pObj->UpdateModel();
    normalCur += normalStep;
  }
}


// TODO : parallelize if we have more than N objects
//        join threads in the end
bool Scene::DetectCollisions(CollisionDetectionStop const stopWhen) //! THIS SHOULD BE CONST!
{
  LastSeparatingAxis lsa;
  bool withinSphere;
  std::string concName;
  /* 
    OPTIMIZE:
    record the last 2 objects that collided, and start with them for major speed up
    (at least in case we're in scenario: stopWhen = CollisionDetectionStop::kStopOnFirst)
  */

  for (auto it = _objects.begin() ; it != _objects.end(); ++it)
  {
    // only check all objects that's after the one we're currently checking (to avoid double checks)
    // this also takes care of not counting any collision twice
    for (auto it2 = (it + 1); it2 != _objects.end(); ++it2)
    {
      // consider having a separate vector where this is filtered out when added
      if ((*it)->IgnoreCollision() || (*it2)->IgnoreCollision())
      {
        continue;
      }

      // objects shall have CheckCollision that takes in an object
      if (auto const [collision, C_ij] = (*it)->CheckCollision(*it2, &lsa, &withinSphere) ; collision )
      {
        if (CollisionDetectionStop::kStopOnFirst == stopWhen)
        {
          return true;
        }
        concName = std::string((*it)->GetName()) + std::string((*it2)->GetName());

        std::cout << "Retrieveing ConcName = " << concName << " and normal = " << glm::to_string(_lastSeparatingAxis[concName].normal) << "\n";
        auto colPoint = (*it)->GetCollisionPoint(_lastSeparatingAxis[concName], C_ij.value(), *it2);
        DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(colPoint.p_abs, _lastSeparatingAxis[concName].normal);
        
        (*it)->CalcCollision(*it2, colPoint, _lastSeparatingAxis[concName].normal);
        return true;
      }
      // start tracking last separating axis when inside sphere
      else if (withinSphere)
      {
        concName = std::string((*it)->GetName()) + std::string((*it2)->GetName());
        glm::vec3 tmpPos = (*it)->GetPosition();
        DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT2(tmpPos, lsa.normal);
        DrawDebugObject();
        InsertLSA(concName, lsa);
      }
      
    }
  }
  return false;
}

void Scene::UpdatePos(float deltaTime)
{
  if (_isPaused)
    return;
  for (auto& o : _objects)
  {
      // SOME OBJECTS CAN BE NON-MOVABLE -> NO NEED TO CHANGE THEIR POSITION. THIS SHOULD BE IMPLEMENTED - FX WITH A BOOL OR mass == INFINITY?
      o->RecordState();
      o->UpdatePosition(deltaTime);
      o->UpdateRotation(deltaTime);
  }
}

void Scene::DrawObjects()
{
  const float selectedScale = 0.97;
  // glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor

  glStencilFunc(GL_ALWAYS, 1, 0xFF); 
  glStencilMask(0xFF); 
  // scaled down FOR STENCIL / "SELECTED "
  for (auto& o : _objects)
  {
    if (o->IsSelected())
    {
      o->RecordScale(); // record scale before changing (restored below)
      o->SetRelativeScale(selectedScale); // shrink object a little to Draw select stencil below
      o->UpdateModel(); // update model to reflect scaling
    }
    o->DrawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
    o->ActivateTextureId(&_textureManager);
    o->Draw(_shader);
    o->CheckForErrors();
  }
  
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilMask(0x00);
  // glDisable(GL_DEPTH_TEST);
  // switch to single color to Draw selected objects select frame
  _shaderSingleColor.use();

  for (auto& o : _objects)
  {
    if (o->IsSelected())
    {
      o->RestoreScale(); // restore object to original size
      o->UpdateModel();
      o->DrawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
      // o->ActivateTextureId(&_textureManager);
      o->Draw(_shaderSingleColor);
      o->CheckForErrors();
    }
  }
  glStencilMask(0xFF);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glEnable(GL_DEPTH_TEST);

}

void Scene::DrawDebugObject()
{
  if (nullptr != _colDebugObject)
  {
    _colDebugObject->DrawInit();
    _colDebugObject->ActivateTextureId(&_textureManager);
    _colDebugObject->Draw(_shader);
    for (auto& pObj : _colNormalDebugObjectVector)
      pObj->Draw(_shader);
    for (auto& pObj : _colNormalDebugObjectVector2)
      pObj->Draw(_shader);
  }
}

void Scene::DrawOverlay()
{
  static Shader s("../shaders/overlayshader.vs", "../shaders/overlayshader.fs");
  static bool init = true;
  static unsigned int overlayVAO;
  static unsigned int overlayVBO;
  if (init)
  {
    float scale = 0.005f;
    const float overlayVertices[] = 
    {
      // positions          // texture Coords
      -scale, -scale, -scale,  //0.0f, 0.0f,
       scale, -scale, -scale,  //1.0f, 0.0f,
       scale,  scale, -scale,  //1.0f, 1.0f,
       scale,  scale, -scale,  //1.0f, 1.0f,
      -scale,  scale, -scale,  //0.0f, 1.0f,
      -scale, -scale, -scale   //0.0f, 0.0f,
    };

    // box VAO
    glGenVertexArrays(1, &overlayVAO);
    glGenBuffers(1, &overlayVBO);
    glBindVertexArray(overlayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, overlayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVertices), &overlayVertices, GL_STATIC_DRAW);
    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // texture attribute
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    init = false;
  }

  s.use();
  glBindVertexArray(overlayVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

}

void Scene::StepBackObjects()
{
  for (auto& o : _objects)
  {
    o->RestoreRecState();
  }
}

// not completely finished - there are edge cases where we don't select the right element!
// (example: big object that is nearer, but has part of it behind the intended object, and the ray passing through this part)
void Scene::SelectNearestObjectPointedAt()
{
  using objDist = std::pair<Object*, float>;
  std::vector<objDist> pointedAt;
  auto const& pRay = _camera.GetPosition();
  auto const& dRay = _camera.GetFront(); // must be normalized!
  float minDist = INFINITY;
  Object* objPointedAt;
  bool objectFound = false;
  // dRay = glm::normalize(dRay);
  
  for (auto& o : _objects)
  {
    if (o->CheckRayVsBoxCollision(pRay, dRay))
    {
      float dist = glm::length(pRay - o->GetPosition());
      // keep track of nearest (simple / naive version)
      if (dist < minDist)
      {
        minDist = dist;
        objPointedAt = o;
        objectFound = true;
      }
    }
  }

  if (!objectFound)
    return;

  bool isSel = objPointedAt->IsSelected();
  objPointedAt->SetIsSelected(!isSel); // toggle selection
  if (isSel)
  {
    for (size_t cnt = 0; cnt < _selectedObjects.size(); cnt++)
    {
      if (_selectedObjects[cnt] == objPointedAt)
      {
        _selectedObjects.erase (_selectedObjects.begin() + cnt); // delete de-selected obj
        break; // found, so exit
      }
    }
  }
  else 
  {
    _selectedObjects.push_back(objPointedAt);
  }
}
