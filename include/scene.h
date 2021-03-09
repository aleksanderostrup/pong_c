#pragma once

#include "object.h"
#include <map>
#include <string>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>
// #include <stb_image.h>
// #include <iostream>

class Scene
{
public:

  Scene(float gridsize)
  {
    this->gridsize = gridsize;
  }

  void updateScene(const float deltaTime, bool isPaused)
  {
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
            // if (stupidDebugThingStopOnFirst)
            // {
            //   return;
            // }
            // if (breakNow)
            // {
            //   break;
            // }
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
    drawDebugObject(); // TODO: implement more elegantly than there!
  }

  void stupidDebug()
  {
    stupidDebugThingStopOnFirst = !stupidDebugThingStopOnFirst;
  }

protected:

public:
  void addObject(Object* obj)
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
    objects.push_back(obj);
  }

  void setColPointDebugObject(Object* obj)
  {
    // FOR DEBUGGING - FIND MORE ELEGANT WAY TO DO THIS
    colDebugObject = obj;
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

  

private:

  enum CollisionDetectionStop {
    kStopOnFirst    = 1,
    kTrackAllLSAs   = 2,
    kHandleAll      = 3
  };

  float                                      gridsize;
  std::vector<Object*>                       objects;
  Object*                                    colDebugObject = nullptr;
  std::vector<Object*>                       colNormalDebugObjectVector;
  std::vector<Object*>                       colNormalDebugObjectVector2;
  std::map<std::string, sLastSeparatingAxis> lastSeparatingAxis;

  bool                                       stupidDebugThingStopOnFirst = false;          

  // helper function 
  void insertLSA(std::string& concName, sLastSeparatingAxis& lsa)
  {
    std::pair<std::map<std::string, sLastSeparatingAxis>::iterator, bool> retPair;
    retPair = lastSeparatingAxis.insert(std::pair<std::string, sLastSeparatingAxis>(concName, lsa));
    std::cout << "Inserting LSA (key=" << concName << "): " << lsa.index << " = " << glm::to_string(lsa.normal) << "\n";
    if (retPair.second == false)  // if already inserted...
    {
      lastSeparatingAxis[concName] = lsa; // ...overwrite
    }
  }

  void DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(glm::vec3& colPoint, glm::vec3& colNormal)
  {
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
      o->drawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
      o->draw();
      o->checkForErrors();
    }
  }
  
  void drawObjects()
  {
    for (auto& o : objects)
    {
      o->drawInit(); // CAN BE OPTIMIZED: SHOULD ONLY BE CALLED ONCE FOR EACH OBJECT TYPE (e.g., once for box, once for sphere, once for plane, etc.)
      o->draw();
      o->checkForErrors();
    }
  }

  void drawDebugObject()
  {
    if (nullptr != colDebugObject)
    {
      colDebugObject->drawInit();
      colDebugObject->draw();
      for (auto& pObj : colNormalDebugObjectVector)
        pObj->draw();
      for (auto& pObj : colNormalDebugObjectVector2)
        pObj->draw();
    }
  }

  void stepBackObjects()
  {
    for (auto& o : objects)
    {
      o->restoreRecState();
    }
  }
};
