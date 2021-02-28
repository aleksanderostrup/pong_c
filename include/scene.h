#pragma once

#include "object.h"
#include <map>
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

    void updateScene(float deltaTime, bool isPaused)
    {

      if (!isPaused) 
      {
        UpdatePos(deltaTime, isPaused);
        // do not calculate, just check
        if (detectCollisions(true))
        {
          float timeLeft = deltaTime;
          float updateTimeGran = deltaTime;
          // controls the detection granularity
          // could be made dependent on fastest relative collision speed for even better performance
          const size_t divDepthMax = 5;
          while (true)
          {
            // reset depth
            size_t divDepth = 0; // 0 = full time step
            do 
            {
              // 1) go to previous position
              stepBackObjects();
              // sub-divide
              updateTimeGran /= 2;
              divDepth++;
              // move objects forward, but with only half the time step
              UpdatePos(updateTimeGran, isPaused);
              std::cout << "Div depth is : " << divDepth << std::endl;
            }
            // keep dividing while we have a collision or reached the binary depth
            while (detectCollisions(true) && (divDepth != divDepthMax));
            if (timeLeft <= updateTimeGran)
            {
              // the whole timesteps (deltaTime) has been updated
              break;
            }
            timeLeft -= updateTimeGran;
            // resolve the collisions
            detectCollisions(false);
            updateTimeGran = timeLeft;
            // try to move to the end of the timestep
            UpdatePos(updateTimeGran, isPaused);
            if (!detectCollisions(true))
            {
              // if no collisions to resolve, exit at the final time step
              break;
            }
          }
        }
      }

      drawObjects();
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

  private:

    float                                 gridsize;
    std::vector<Object*>                  objects;
    std::map<string, sLastSeparatingAxis> lastSeparatingAxis;

    // TODO : parallelize if we have more than N objects
    //        join threads in the end
    bool detectCollisions(bool abortOnFirst)
    {
      sLastSeparatingAxis lsa;
      bool withinSphere;
      std::string concName;
      glm::mat3 C_ij;

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
            if (abortOnFirst)
            {
              return true;
            }
            concName = std::string((*it)->getName()) + std::string((*it2)->getName());
            std::cout << "collision!" <<  std::endl;
            sCollisionPoint colPoint;
            (*it)->getCollisionPoint(lastSeparatingAxis[concName], colPoint, C_ij, *it2);
            (*it)->calcCollision(*it2, colPoint, lastSeparatingAxis[concName].normal);
            return true;
          }
          // start tracking last separating axis when inside sphere
          else if (withinSphere)
          {
            concName = std::string((*it)->getName()) + std::string((*it2)->getName());
            std::pair<std::map<std::string, sLastSeparatingAxis>::iterator, bool> retPair;
            retPair = lastSeparatingAxis.insert(std::pair<std::string, sLastSeparatingAxis>(concName, lsa));
             
            if ((retPair.second == false) &&                // if already inserted AND
                (retPair.first->second.index != lsa.index)) // new LSA
            {
              lastSeparatingAxis[concName] = lsa; // overwrite
            }
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

    void stepBackObjects()
    {
      for (auto& o : objects)
      {
        o->goToPrevPosRot();
      }
    }
};
