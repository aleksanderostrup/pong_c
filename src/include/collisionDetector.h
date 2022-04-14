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

  bool BoundingSphereOverlaps(Object const& A, Object const& B) const;

  // TODO: only copy-by-value if we need to ALTER the objects
  bool DetectCollision(DetectStop const stopWhen, std::vector<Object*> objects);
  
  // TODO: can we extract the essentials from the Objects and take them as values?
  std::pair<bool, std::optional<glm::mat3>> CheckCollision(Object* A, Object* B, LastSeparatingAxis* lsa, bool* withinSphere);

private:
  void InsertLSA(std::string& concName, LastSeparatingAxis& lsa) const;

  mutable std::map<std::string, LastSeparatingAxis>     _lastSeparatingAxis;
};