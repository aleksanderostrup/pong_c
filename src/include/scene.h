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

  ColPointDbg(Camera const& camera)
    : mColVec(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 3.0f, camera)
  {}
  
  size_t    mFrameCnt = 0;
  Line      mColVec;
  
  /* ADD SETLENGTH() FNC? */

  void SetColPoint(glm::vec3& p, glm::vec3& dir)
  {
    mColVec.SetPosition(p);
    mColVec.setDirection(dir);
    mFrameCnt = frameCntInit;
  }

  void Draw(glm::mat4& projection, bool isPaused)
  {
    if (mFrameCnt)
    {
      mColVec.Draw(nullptr, projection);
      if (!isPaused)
        mFrameCnt--;
    }
  }
};

class Scene
{
public:

  Scene(float const gridsize, Camera const& camera, uint32_t const& width, uint32_t const& height)
   : mSkybox(camera, width, height)
   , mCamera(camera)
   , mWidth(width)
   , mHeight(height)
   , mShader("../shaders/shader.vs", "../shaders/shader.fs")
   , mShaderSingleColor("../shaders/shader.vs", "../shaders/singlecolorshader.fs")
   , mColPointDbg(camera)
  //  , mLine(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, camera)
  {
    this->gridsize = gridsize;
    mShader.use();
    mShader.setInt("texture1", 0);

  }

  Shader& GetShader()
  {
    return mShader;
  }

  void updateScene(const float deltaTime, bool isPaused)
  {
    glm::mat4 projection = glm::perspective(glm::radians(mCamera.GetZoom()), (float)mWidth / (float)mHeight, 0.1f, 100.0f);
    glm::mat4 view = mCamera.GetViewMatrix();

    mShaderSingleColor.use();
    mShaderSingleColor.setMat4("view", view);
    mShaderSingleColor.setMat4("projection", projection);

    mShader.use();
    mShader.setMat4("projection", projection);
    mShader.setMat4("view", view);

    size_t totalLoops = 0;
    if (!isPaused) 
    {
      UpdatePos(deltaTime, isPaused);
      // do not calculate, just check
      if (DetectCollisions(CollisionDetectionStop::kStopOnFirst))
      {
        bool colDetected;
        float timeLeft = deltaTime;
        float updateTimeGran = deltaTime;
        // controls the detection granularity
        // could be made dependent on fastest relative collision speed for even better performance
        const size_t divDepthMax = 6;
        size_t maxTries = 50; // if we somehow get stuck
        size_t resolvedNumberForDebug = 0; // if we somehow get stuck

      float minDeltaTime = deltaTime;
        // MIN TIME STEP:
        for (size_t i = 0; i < divDepthMax; i++)
        {
          minDeltaTime /= 2;
        }

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
            // sub-divide
            updateTimeGran /= 2;
            // DEBUG /DEVELOP CODE!
            // if (updateTimeGran < minDeltaTime)
            // {
            //   updateTimeGran = minDeltaTime;
            //   if (updateTimeGran > timeLeft)
            //   {
            //     updateTimeGran = timeLeft;
            //   }
            //   breakNow = true;
            // }
            divDepth++;
            // move objects forward, but with only half the time step
            UpdatePos(updateTimeGran, isPaused);
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
          UpdatePos(timeLeft, isPaused);
          if (!DetectCollisions(CollisionDetectionStop::kStopOnFirst))
          {
            // if no collisions to resolve, exit at the final time step
            break;
          }
          if (maxTries-- == 0)
          {
            std::cout << "EMERGENCY BREAK!! Timeleft: " << timeLeft << std::endl;
            // emergency break - something went wrong!
            break;
          }
        }
        std::cout << "Total loops: " <<  totalLoops << "  RESOLVED " << resolvedNumberForDebug << "\n";
      }
    }
    
    DrawObjects();
    mColPointDbg.Draw(projection, isPaused);
    mShader.use();
    DrawDebugObject(); // TODO: implement more elegantly than there!
    mShader.use();
    DrawOverlay();
    

    // Draw skybox as last
    mSkybox.Draw();
  }

  void StupidDebug()
  {
    // stupidDebugThingStopOnFirst = !stupidDebugThingStopOnFirst;
    // std::cout << "Cam.. Pos: " << glm::to_string(mCamera.Position) << " Front: " << glm::to_string(mCamera.Front) << "\n";
    SelectNearestObjectPointedAt();
  }

  void PrintSummedVelAndRot() const
  {
    glm::vec3 totalVel(0.0f);
    glm::vec3 totalRotVel(0.0f);
    for (auto const& o : objects)
    {
      totalVel    += o->GetVelocity();
      totalRotVel += o->GetRotationVelocity();
    }
    std::cout << "Total vel     = " << glm::to_string(totalVel)     << "|" << std::to_string(glm::length(totalVel))     << "|\n";
    std::cout << "Total rot_vel = " << glm::to_string(totalRotVel)  << "|" << std::to_string(glm::length(totalRotVel))  << "|\n";
  }

protected:

public:
  void AddObject(Object* obj, TextureManager::EnumTexture textureEnum = TextureManager::kDan)
  {
    std::string concName;
    for (auto it = this->objects.begin() ; it != this->objects.end(); ++it)
    {
      if (strcmp((*it)->GetName(), obj->GetName()) == 0)
      {
        std::cout << "Error, name " << obj->GetName() << " already exists - aborting program." << std::endl;
        exit(0);
      }
    }
    obj->SetTextureId(textureEnum, &textureManager);
    objects.push_back(obj);
  }

  void setColPointDebugObject(Object* obj)
  {
    // FOR DEBUGGING - FIND MORE ELEGANT WAY TO DO THIS
    colDebugObject = obj;
    colDebugObject->SetTextureId(TextureManager::kMetal, &textureManager);
  }

  void SetColNormalDebugObject(std::vector<Object*>& obj, size_t index)
  {
    // FOR DEBUGGING - FIND MORE ELEGANT WAY TO DO THIS
    switch (index)
    {
      case 0: colNormalDebugObjectVector = obj;
      case 1: colNormalDebugObjectVector2 = obj;
    }
  }

  void PrintInfoForSelected()
  {
    std::cout << "\n----- ************************ -----\n"
              <<   "----- Info for selected object -----\n"
              <<   "----- ************************ -----\n";
    for (auto& o : selectedObjects)
    {
      o->printObject();
    }
  }

  

private:

  enum class CollisionDetectionStop {
    kStopOnFirst    = 1,
    kTrackAllLSAs   = 2,
    kHandleAll      = 3
  };

  float                                      gridsize;
  std::vector<Object*>                       objects;
  std::vector<Object*>                       selectedObjects;
  Object*                                    colDebugObject = nullptr;
  std::vector<Object*>                       colNormalDebugObjectVector;
  std::vector<Object*>                       colNormalDebugObjectVector2;
  std::map<std::string, sLastSeparatingAxis> lastSeparatingAxis;
  TextureManager                             textureManager;
  Skybox                                     mSkybox;
  Camera const&                              mCamera;
  uint32_t const&                            mWidth;
  uint32_t const&                            mHeight;
  Shader                                     mShader;
  Shader                                     mShaderSingleColor;
  ColPointDbg                                mColPointDbg;
  // Line                                       mLine;

  bool                                       stupidDebugThingStopOnFirst = false;          

  // helper function 
  void InsertLSA(std::string& concName, sLastSeparatingAxis& lsa)
  {
    std::pair<std::map<std::string, sLastSeparatingAxis>::iterator, bool> retPair;
    retPair = lastSeparatingAxis.insert(std::pair<std::string, sLastSeparatingAxis>(concName, lsa));
    if (retPair.second == false)  // if already inserted...
    {
      lastSeparatingAxis[concName] = lsa; // ...overwrite
    }
  }

  void DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(glm::vec3& colPoint, glm::vec3& colNormal)
  {
    mColPointDbg.SetColPoint(colPoint, colNormal);
    if (colDebugObject != nullptr)
    {
      colDebugObject->SetPosition(colPoint);
      colDebugObject->UpdateModel();
      float normalLengthMax = 3.0f;
      float normalStep = normalLengthMax / colNormalDebugObjectVector.size();
      float normalCur = 0.0f;
      for (auto& pObj : colNormalDebugObjectVector)
      { 
        pObj->SetPosition(colPoint + normalCur * colNormal);
        pObj->UpdateModel();
        normalCur += normalStep;
      }
    }
  }

  void DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT2(glm::vec3& colPoint, glm::vec3& colNormal)
  {
    float normalLengthMax = 3.0f;
    float normalStep = normalLengthMax / colNormalDebugObjectVector.size();
    float normalCur = 0.0f;
    for (auto& pObj : colNormalDebugObjectVector2)
    { 
      pObj->SetPosition(colPoint + normalCur * colNormal);
      pObj->UpdateModel();
      normalCur += normalStep;
    }
  }


  // TODO : parallelize if we have more than N objects
  //        join threads in the end
  bool DetectCollisions(CollisionDetectionStop const stopWhen)
  {
    sLastSeparatingAxis lsa;
    bool withinSphere;
    std::string concName;
    glm::mat3 C_ij;
    /* 
      OPTIMIZE:
      record the last 2 objects that collided, and start with them for major speed up
      (at least in case we're in scenario: stopWhen = CollisionDetectionStop::kStopOnFirst)
    */

    for (auto it = this->objects.begin() ; it != this->objects.end(); ++it)
    {
      // only check all objects that's after the one we're currently checking (to avoid double checks)
      // this also takes care of not counting any collision twice
      for (auto it2 = (it + 1); it2 != this->objects.end(); ++it2)
      {
        // consider having a separate vector where this is filtered out when added
        if ((*it)->IgnoreCollision() || (*it2)->IgnoreCollision())
        {
          continue;
        }

        // objects shall have checkCollision that takes in an object
        if ((*it)->checkCollision(*it2, &lsa, &withinSphere, C_ij))
        {
          if (CollisionDetectionStop::kStopOnFirst == stopWhen)
          {
            return true;
          }
          concName = std::string((*it)->GetName()) + std::string((*it2)->GetName());

          sCollisionPoint colPoint;
          std::cout << "Retrieveing ConcName = " << concName << " and normal = " << glm::to_string(lastSeparatingAxis[concName].normal) << "\n";
          (*it)->getCollisionPoint(lastSeparatingAxis[concName], colPoint, C_ij, *it2);
          DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(colPoint.p_abs, lastSeparatingAxis[concName].normal);
          
          (*it)->calcCollision(*it2, colPoint, lastSeparatingAxis[concName].normal);
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

  void UpdatePos(float deltaTime, bool isPaused)
  {
    for (auto& o : objects)
    {
      if (!isPaused) 
      {
        // SOME OBJECTS CAN BE NON-MOVABLE -> NO NEED TO CHANGE THEIR POSITION. THIS SHOULD BE IMPLEMENTED - FX WITH A BOOL OR mass == INFINITY?
        o->RecordState();
        o->UpdatePosition(deltaTime);
        o->UpdateRotation(deltaTime);
      }
    }
  }
  
  void DrawObjects()
  {
    const float selectedScale = 0.97;
    // glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor

    glStencilFunc(GL_ALWAYS, 1, 0xFF); 
    glStencilMask(0xFF); 
    // scaled down FOR STENCIL / "SELECTED "
    for (auto& o : objects)
    {
      if (o->isSelected())
      {
        o->RecordScale(); // record scale before changing (restored below)
        o->SetRelativeScale(selectedScale); // shrink object a little to Draw select stencil below
        o->UpdateModel(); // update model to reflect scaling
      }
      o->DrawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
      o->ActivateTextureId(&textureManager);
      o->Draw(mShader);
      o->CheckForErrors();
    }
    
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    // glDisable(GL_DEPTH_TEST);
    // switch to single color to Draw selected objects select frame
    mShaderSingleColor.use();

    for (auto& o : objects)
    {
      if (o->isSelected())
      {
        o->RestoreScale(); // restore object to original size
        o->UpdateModel();
        o->DrawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
        // o->ActivateTextureId(&textureManager);
        o->Draw(mShaderSingleColor);
        o->CheckForErrors();
      }
    }
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

  }

  void DrawDebugObject()
  {
    if (nullptr != colDebugObject)
    {
      colDebugObject->DrawInit();
      colDebugObject->ActivateTextureId(&textureManager);
      colDebugObject->Draw(mShader);
      for (auto& pObj : colNormalDebugObjectVector)
        pObj->Draw(mShader);
      for (auto& pObj : colNormalDebugObjectVector2)
        pObj->Draw(mShader);
    }
  }

  void DrawOverlay()
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

  void StepBackObjects()
  {
    for (auto& o : objects)
    {
      o->RestoreRecState();
    }
  }

  // not completely finished - there are edge cases where we don't select the right element!
  // (example: big object that is nearer, but has part of it behind the intended object, and the ray passing through this part)
  void SelectNearestObjectPointedAt()
  {
    using objDist = std::pair<Object*, float>;
    std::vector<objDist> pointedAt;
    auto const& pRay = mCamera.GetPosition();
    auto const& dRay = mCamera.GetFront(); // must be normalized!
    float minDist = INFINITY;
    Object* objPointedAt;
    // dRay = glm::normalize(dRay);
    
    for (auto& o : objects)
    {
      if (o->CheckRayVsBoxCollision(pRay, dRay))
      {
        float dist = glm::length(pRay - o->GetPosition());
        // keep track of nearest (simple / naive version)
        if (dist < minDist)
        {
          minDist = dist;
          objPointedAt = o;
        }
      }
    }


    bool isSel = objPointedAt->isSelected();
    objPointedAt->SetIsSelected(!isSel); // toggle selection
    if (isSel)
    {
      for (size_t cnt = 0; cnt < selectedObjects.size(); cnt++)
      {
        if (selectedObjects[cnt] == objPointedAt)
        {
          selectedObjects.erase (selectedObjects.begin() + cnt); // delete de-selected obj
          break; // found, so exit
        }
      }
    }
    else 
    {
      selectedObjects.push_back(objPointedAt);
    }
  }

};

