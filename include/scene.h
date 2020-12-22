#pragma once

#include "object.h"
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
        detectCollisions();
      }
      drawObjectsAndUpdatePos(deltaTime, isPaused);
    }

  protected:

  public:
    void addObject(Object* obj)
    {
        objects.push_back(obj);
    }

  private:

    float                 gridsize;
    std::vector<Object*>  objects;

    // simple brute force collision detection algorithm
    // -- can easily be improved (see fx literature or just pick low-hanging fruits to get major improvements)
    // notes: right now we have a n^2 runtime. If we do a local search instead this can be minimized by moving objects into localized subspaces.
    // there should be an optimized number of subspaces. downsize is that we have to handle moves between subspaces and that objects can be in multiple subspaces
    // see also: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
    void detectCollisions()
    {
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
          if ((*it)->checkCollision(* it2))
          {
            std::cout << "collision!!" <<  std::endl;
            (*it)->calcCollision(*it2);
          }
        }
      } 
    }

    void drawObjectsAndUpdatePos(float deltaTime, bool isPaused)
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
};
