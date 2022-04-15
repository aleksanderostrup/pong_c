#pragma once

#include "object.h"

class CollisionDetector
{
  
public:

  enum class DetectStop
  {
    kStopOnFirst    = 1,
    kTrackAllLSAs   = 2,
    kHandleAll      = 3
  };

  // TODO: only copy-by-value if we need to ALTER the objects
  bool DetectCollision(DetectStop const stopWhen, std::vector<Object*> objects);
  float DetectFirstCollisionTime(float const deltaT, std::vector<Object*> objects);


private:
  void InsertLSA(std::string& concName, LastSeparatingAxis& lsa) const;

  // TODO: can we extract the essentials from the Objects and take them as values?
  std::pair<bool, std::optional<glm::mat3>> CheckCollision(Object* A, Object* B, LastSeparatingAxis* lsa, bool* withinSphere);
  std::pair<bool, std::optional<glm::mat3>> CheckCollisionNew(Object* A, Object* B, LastSeparatingAxis* lsa, bool* withinSphere) const;
  bool BoundingSphereOverlaps(Object const& A, Object const& B) const;

  mutable std::map<std::string, LastSeparatingAxis>     _lastSeparatingAxis;
};