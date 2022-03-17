#pragma once

#include "object.h"
#include <map>
#include <string>
#include "../include/textureManager.h"
#include "../include/camera.h"
#include "../include/skybox.h"
#include "../include/line.h"

struct ColPointDbg
{
  static const size_t frameCntInit = 5;

  ColPointDbg(Camera& camera)
    : mColVec(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 3.0f, camera)
  {}
  
  size_t    mFrameCnt = 0;
  Line      mColVec;
  
  /* ADD SETLENGTH() FNC? */

  void setColPoint(glm::vec3& p, glm::vec3& dir)
  {
    mColVec.setPosition(p);
    mColVec.setDirection(dir);
    mFrameCnt = frameCntInit;
  }

  void draw(glm::mat4& projection, bool isPaused)
  {
    if (mFrameCnt)
    {
      mColVec.draw(nullptr, projection);
      if (!isPaused)
        mFrameCnt--;
    }
  }
};

class Scene
{
public:

  Scene(float gridsize, Camera& camera, uint32_t& width, uint32_t& height)
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

  Shader& getShader()
  {
    return mShader;
  }

  void updateScene(const float deltaTime, bool isPaused)
  {
    glm::mat4 projection = glm::perspective(glm::radians(mCamera.Zoom), (float)mWidth / (float)mHeight, 0.1f, 100.0f);
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
      if (detectCollisions(kStopOnFirst))
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
            stepBackObjects();
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
          while ((colDetected = detectCollisions(kStopOnFirst)) && // keep dividing while we have a collision AND
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
            if (!detectCollisions(kHandleAll))
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
          if (!detectCollisions(kStopOnFirst))
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
    
    drawObjects();
    mColPointDbg.draw(projection, isPaused);
    mShader.use();
    drawDebugObject(); // TODO: implement more elegantly than there!
    mShader.use();
    drawOverlay();
    

    // draw skybox as last
    mSkybox.draw();
  }

  void stupidDebug()
  {
    // stupidDebugThingStopOnFirst = !stupidDebugThingStopOnFirst;
    // std::cout << "Cam.. Pos: " << glm::to_string(mCamera.Position) << " Front: " << glm::to_string(mCamera.Front) << "\n";
    selectNearestObjectPointedAt();
  }

protected:

public:
  void addObject(Object* obj, TextureManager::EnumTexture textureEnum = TextureManager::kDan)
  {
    std::string concName;
    for (auto it = this->objects.begin() ; it != this->objects.end(); ++it)
    {
      if (strcmp((*it)->getName(), obj->getName()) == 0)
      {
        std::cout << "Error, name " << obj->getName() << " already exists - aborting program." << std::endl;
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

  void setColNormalDebugObject(std::vector<Object*>& obj, size_t index)
  {
    // FOR DEBUGGING - FIND MORE ELEGANT WAY TO DO THIS
    switch (index)
    {
      case 0: colNormalDebugObjectVector = obj;
      case 1: colNormalDebugObjectVector2 = obj;
    }
  }

  void printInfoForSelected()
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

  enum CollisionDetectionStop {
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
  Camera&                                    mCamera;
  uint32_t&                                  mWidth;
  uint32_t&                                  mHeight;
  Shader                                     mShader;
  Shader                                     mShaderSingleColor;
  ColPointDbg                                mColPointDbg;
  // Line                                       mLine;

  bool                                       stupidDebugThingStopOnFirst = false;          

  // helper function 
  void insertLSA(std::string& concName, sLastSeparatingAxis& lsa)
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
    mColPointDbg.setColPoint(colPoint, colNormal);
    if (colDebugObject != nullptr)
    {
      colDebugObject->setPosition(colPoint);
      colDebugObject->updateModel();
      float normalLengthMax = 3.0f;
      float normalStep = normalLengthMax / colNormalDebugObjectVector.size();
      float normalCur = 0.0f;
      for (auto& pObj : colNormalDebugObjectVector)
      { 
        pObj->setPosition(colPoint + normalCur * colNormal);
        pObj->updateModel();
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
      pObj->setPosition(colPoint + normalCur * colNormal);
      pObj->updateModel();
      normalCur += normalStep;
    }
  }


  // TODO : parallelize if we have more than N objects
  //        join threads in the end
  bool detectCollisions(CollisionDetectionStop stopWhen)
  {
    sLastSeparatingAxis lsa;
    bool withinSphere;
    std::string concName;
    glm::mat3 C_ij;
    /* 
      OPTIMIZE:
      record the last 2 objects that collided, and start with them major for speed up
      (at least in case we're in scenario: stopWhen = kStopOnFirst)
    */

    for (auto it = this->objects.begin() ; it != this->objects.end(); ++it)
    {
      // only check all objects that's after the one we're currently checking (to avoid double checks)
      // this also takes care of not counting any collision twice
      for (auto it2 = (it + 1); it2 != this->objects.end(); ++it2)
      {
        // consider having a separate vector where this is filtered out when added
        if ((*it)->ignoreColl() || (*it2)->ignoreColl())
        {
          continue;
        }

        // objects shall have checkCollision that takes in an object
        if ((*it)->checkCollision(*it2, &lsa, &withinSphere, C_ij))
        {
          if (kStopOnFirst == stopWhen)
          {
            return true;
          }
          concName = std::string((*it)->getName()) + std::string((*it2)->getName());

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
          concName = std::string((*it)->getName()) + std::string((*it2)->getName());
          glm::vec3 tmpPos = (*it)->getPosition();
          DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT2(tmpPos, lsa.normal);
          drawDebugObject();
          insertLSA(concName, lsa);
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
        o->recordState();
        o->updatePosition(deltaTime);
        o->updateRotation(deltaTime);
      }
    }
  }
  
  void drawObjects()
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
        o->recordScale(); // record scale before changing (restored below)
        o->setRelativeScale(selectedScale); // shrink object a little to draw select stencil below
        o->updateModel(); // update model to reflect scaling
      }
      o->drawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
      o->ActivateTextureId(&textureManager);
      o->draw(mShader);
      o->checkForErrors();
    }
    
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    // glDisable(GL_DEPTH_TEST);
    // switch to single color to draw selected objects select frame
    mShaderSingleColor.use();

    for (auto& o : objects)
    {
      if (o->isSelected())
      {
        o->restoreScale(); // restore object to original size
        o->updateModel();
        o->drawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
        // o->ActivateTextureId(&textureManager);
        o->draw(mShaderSingleColor);
        o->checkForErrors();
      }
    }
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

  }

  void drawDebugObject()
  {
    if (nullptr != colDebugObject)
    {
      colDebugObject->drawInit();
      colDebugObject->ActivateTextureId(&textureManager);
      colDebugObject->draw(mShader);
      for (auto& pObj : colNormalDebugObjectVector)
        pObj->draw(mShader);
      for (auto& pObj : colNormalDebugObjectVector2)
        pObj->draw(mShader);
    }
  }

  void drawOverlay()
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

  void stepBackObjects()
  {
    for (auto& o : objects)
    {
      o->restoreRecState();
    }
  }

  // not completely finished - there are edge cases where we don't select the right element!
  // (example: big object that is nearer, but has part of it behind the intended object, and the ray passing through this part)
  void selectNearestObjectPointedAt()
  {
    using objDist = std::pair<Object*, float>;
    std::vector<objDist> pointedAt;
    glm::vec3& pRay = mCamera.Position;
    glm::vec3& dRay = mCamera.Front; // must be normalized!
    float minDist = INFINITY;
    Object* objPointedAt;
    // dRay = glm::normalize(dRay);
    
    for (auto& o : objects)
    {
      if (o->checkRayVsBoxCollision(pRay, dRay))
      {
        float dist = glm::length(pRay - o->getPosition());
        // keep track of nearest (simple / naive version)
        if (dist < minDist)
        {
          minDist = dist;
          objPointedAt = o;
        }
      }
    }


    bool isSel = objPointedAt->isSelected();
    objPointedAt->setIsSelected(!isSel); // toggle selection
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

