#include "collisionDetector.h"

static inline float Sign(float const v)
{
  return (v < 0 ? -1.0f : 1.0f);
}

static float GetSigma(const glm::vec3& V, glm::vec3 const& D)
{
  return Sign(glm::dot(V, D));
}

class Counter
{
public:
  Counter(
    int const wrapVal,
    int const startVal        = 0,
    int const step            = 1,
    int const incrementEvery  = 1
  )
    : _wrapVal            (wrapVal)
    , _val                (startVal)
    , _step               (step)
    , _incrementEvery     (incrementEvery)
    , _inrementCountdown  (incrementEvery)
  {}

  Counter& operator++() // prefix increment
  {
    if (!(--_inrementCountdown))
    {
      _val += _step;
      _val %= _wrapVal;
      _inrementCountdown = _incrementEvery;
    }
    return *this;
  }

  operator int() const { return _val; }

private:
  int const   _wrapVal;
  int         _val  = 0;
  int const   _step = 1;
  int const   _incrementEvery = 1;
  int         _inrementCountdown;
};


void CollisionDetector::InsertLSA(std::string& concName, LastSeparatingAxis& lsa) const
{
  if (false == _lastSeparatingAxis.insert({concName, lsa}).second) // if already inserted...
    _lastSeparatingAxis[concName] = lsa;                           // ...overwrite
}

// TODO: implement with much more encapsulation and only necessary details
// TODO: this class should not ALTER the objects
// - CONST Collision detection -> input 2 objects and minDeltaT for coll time, output time of collision (within input delteT)
//                           [what is the essential information from the objects that is needed?]
//                           - this function can be constant


bool CollisionDetector::DetectCollision(DetectStop const stopWhen, std::vector<Object*> objects)
{
  LastSeparatingAxis lsa;
  bool withinSphere;
  std::string concName;
  /* 
    OPTIMIZE:
    record the last 2 objects that collided, and start with them for major speed up
    (at least in case we're in scenario: stopWhen = DetectStop::kStopOnFirst)
  */

  for (auto it = objects.begin() ; it != objects.end(); ++it)
  {
    // only check all objects that's after the one we're currently checking (to avoid double checks)
    // this also takes care of not counting any collision twice
    for (auto it2 = (it + 1); it2 != objects.end(); ++it2)
    {
      // consider having a separate vector where this is filtered out when added
      if ((*it)->IgnoreCollision() || (*it2)->IgnoreCollision())
      {
        continue;
      }

      // objects shall have CheckCollision that takes in an object
      if (auto const [collision, C_ij] = /* (*it)-> */CheckCollision(*it, *it2, &lsa, &withinSphere) ; collision )
      {
        if (DetectStop::kStopOnFirst == stopWhen)
        {
          return true;
        }
        concName = std::string((*it)->GetName()) + std::string((*it2)->GetName());

        std::cout << "Retrieveing ConcName = " << concName << " and normal = " << glm::to_string(_lastSeparatingAxis[concName].normal) << "\n";
        auto colPoint = (*it)->GetCollisionPoint(_lastSeparatingAxis[concName], C_ij.value(), *it2);
        // DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(colPoint.p_abs, _lastSeparatingAxis[concName].normal);
        
        (*it)->CalcCollision(*it2, colPoint, _lastSeparatingAxis[concName].normal);
        return true;
      }
      // start tracking last separating axis when inside sphere
      else if (withinSphere)
      {
        concName = std::string((*it)->GetName()) + std::string((*it2)->GetName());
        glm::vec3 tmpPos = (*it)->GetPosition();
        // DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT2(tmpPos, lsa.normal);
        // DrawDebugObject();
        InsertLSA(concName, lsa);
      }
      
    }
  }
  return false;
}



// TODO we need to be able to copy "Objects", so we can be const by making copies
// float CollisionDetector::DetectFirstCollisionTime(DetectStop const stopWhen, std::vector<Object*> objects)
float CollisionDetector::DetectFirstCollisionTime(float const deltaT, std::vector<Object*> objects)
{
  // ! breakdown of algorithm
  /* 
    0. UpdatePos(deltaTime);
    1. find all pairs of objects that collides within deltaT
    2. from all pairs find the objects that collides first
    3. return the time when this happens
  */

  // UpdatePos(deltaT);
  std::vector<Object*> collidingObjects;
  return 0;


 // LastSeparatingAxis lsa;
  // bool withinSphere;
  // std::string concName;
  // /* 
  //   OPTIMIZE:
  //   record the last 2 objects that collided, and start with them for major speed up
  //   (at least in case we're in scenario: stopWhen = DetectStop::kStopOnFirst)
  // */

  // for (auto it = objects.begin() ; it != objects.end(); ++it)
  // {

  //   (*it)->UpdateBoundBox();

  //   // only check all objects that's after the one we're currently checking (to avoid double checks)
  //   // this also takes care of not counting any collision twice
  //   for (auto it2 = (it + 1); it2 != objects.end(); ++it2)
  //   {
  //     // consider having a separate vector where this is filtered out when added
  //     if ((*it)->IgnoreCollision() || (*it2)->IgnoreCollision())
  //     {
  //       continue;
  //     }

  //     (*it2)->UpdateBoundBox();

  //     // objects shall have CheckCollision that takes in an object
  //     if (auto const [collision, C_ij] = CheckCollisionNew(*it, *it2, &lsa, &withinSphere) ; collision )
  //     {
  //       if (DetectStop::kStopOnFirst == stopWhen)
  //       {
  //         return true;
  //       }
  //       concName = std::string((*it)->GetName()) + std::string((*it2)->GetName());

  //       std::cout << "Retrieveing ConcName = " << concName << " and normal = " << glm::to_string(_lastSeparatingAxis[concName].normal) << "\n";
  //       auto colPoint = (*it)->GetCollisionPoint(_lastSeparatingAxis[concName], C_ij.value(), *it2);
  //       // DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT(colPoint.p_abs, _lastSeparatingAxis[concName].normal);
        
  //       (*it)->CalcCollision(*it2, colPoint, _lastSeparatingAxis[concName].normal);
  //       return true;
  //     }
  //     // start tracking last separating axis when inside sphere
  //     else if (withinSphere)
  //     {
  //       concName = std::string((*it)->GetName()) + std::string((*it2)->GetName());
  //       glm::vec3 tmpPos = (*it)->GetPosition();
  //       // DEBUGONLY_SET_DEBUGOBJ_TO_COLPOINT2(tmpPos, lsa.normal);
  //       // DrawDebugObject();
  //       InsertLSA(concName, lsa);
  //     }
      
  //   }
  // }
  // return false;
}


static auto CalcRArrays(glm::mat3 const& C_ij, std::array<glm::vec3, 15> const& L, glm::vec3 const& D, glm::vec3 const& aScale, glm::vec3 const& bScale)
{
  // a_i and b_i is the length of the edges
  glm::vec3 const a_i { 0.5f * aScale }; // extent in [DCD Table 1, p. 5] is only half of the side length
  glm::vec3 const b_i { 0.5f * bScale };
  
  const size_t noOfDims   {  3 };
  const size_t noOfTests  { 15 };
  std::array<float, noOfTests> R0;
  std::array<float, noOfTests> R1;
  std::array<float, noOfTests> R;

  glm::mat3 C_ij_abs(1.0f);

  // OPTIMIZE: this should be done on the dot product instead!
  for (size_t n = 0; n < noOfDims; n++)
  {
    C_ij_abs[n][0] = glm::abs(C_ij[n][0]);
    C_ij_abs[n][1] = glm::abs(C_ij[n][1]);
    C_ij_abs[n][2] = glm::abs(C_ij[n][2]);
  }

  // note that col / row order is switched for glm lib compared to ref [DCD].
  R0[0]  = a_i[0];
  R0[1]  = a_i[1];
  R0[2]  = a_i[2];
  // We can access with [] or .x, .y, .z (fx, a_i[0] = a_i.x, etc.)
  R0[3]  = a_i[0] * C_ij_abs[0][0] + a_i[1] * C_ij_abs[0][1] + a_i[2] * C_ij_abs[0][2];
  R0[4]  = a_i[0] * C_ij_abs[1][0] + a_i[1] * C_ij_abs[1][1] + a_i[2] * C_ij_abs[1][2];
  R0[5]  = a_i[0] * C_ij_abs[2][0] + a_i[1] * C_ij_abs[2][1] + a_i[2] * C_ij_abs[2][2];
  R0[6]  = a_i[1] * C_ij_abs[0][2] + a_i[2] * C_ij_abs[0][1];
  R0[7]  = a_i[1] * C_ij_abs[1][2] + a_i[2] * C_ij_abs[1][1];
  R0[8]  = a_i[1] * C_ij_abs[2][2] + a_i[2] * C_ij_abs[2][1];
  R0[9]  = a_i[0] * C_ij_abs[0][2] + a_i[2] * C_ij_abs[0][0];
  R0[10] = a_i[0] * C_ij_abs[1][2] + a_i[2] * C_ij_abs[1][0];
  R0[11] = a_i[0] * C_ij_abs[2][2] + a_i[2] * C_ij_abs[2][0];
  R0[12] = a_i[0] * C_ij_abs[0][1] + a_i[1] * C_ij_abs[0][0];
  R0[13] = a_i[0] * C_ij_abs[1][1] + a_i[1] * C_ij_abs[1][0];
  R0[14] = a_i[0] * C_ij_abs[2][1] + a_i[1] * C_ij_abs[2][0];

  R1[0]  = b_i[0] * C_ij_abs[0][0] + b_i[1] * C_ij_abs[1][0] + b_i[2] * C_ij_abs[2][0];
  R1[1]  = b_i[0] * C_ij_abs[0][1] + b_i[1] * C_ij_abs[1][1] + b_i[2] * C_ij_abs[2][1];
  R1[2]  = b_i[0] * C_ij_abs[0][2] + b_i[1] * C_ij_abs[1][2] + b_i[2] * C_ij_abs[2][2];
  R1[3]  = b_i[0];
  R1[4]  = b_i[1];
  R1[5]  = b_i[2];
  R1[6]  = b_i[1] * C_ij_abs[2][0] + b_i[2] * C_ij_abs[1][0];
  R1[7]  = b_i[0] * C_ij_abs[2][0] + b_i[2] * C_ij_abs[0][0];
  R1[8]  = b_i[0] * C_ij_abs[1][0] + b_i[1] * C_ij_abs[0][0];
  R1[9]  = b_i[1] * C_ij_abs[2][1] + b_i[2] * C_ij_abs[1][1];
  R1[10] = b_i[0] * C_ij_abs[2][1] + b_i[2] * C_ij_abs[0][1];
  R1[11] = b_i[0] * C_ij_abs[1][1] + b_i[1] * C_ij_abs[0][1];
  R1[12] = b_i[1] * C_ij_abs[2][2] + b_i[2] * C_ij_abs[1][2];
  R1[13] = b_i[0] * C_ij_abs[2][2] + b_i[2] * C_ij_abs[0][2];
  R1[14] = b_i[0] * C_ij_abs[1][2] + b_i[1] * C_ij_abs[0][2];

  std::array<float, 6> dotProductArray;
  for (std::size_t i = 0; i < dotProductArray.size(); i++)
  {
    dotProductArray[i]  = glm::dot(L[i], D);
    R[i]                = glm::abs(dotProductArray[i]);
  }
  Counter counter1(3, 1, 1, 3);
  Counter counter2(3, 2, 1, 3);
  for (std::size_t i = 6; i < noOfTests; ++i, ++counter1, ++counter2)
  {
    R[i] = glm::abs((C_ij[i%3][counter1] * dotProductArray[counter2]) - 
                    (C_ij[i%3][counter2] * dotProductArray[counter1]));
  }

  return std::tuple( R0, R1, R );
}


bool CollisionDetector::BoundingSphereOverlaps(Object const& A, Object const& B) const
{
  // skip test if spheres not overlapping
  float const minR = std::pow(A.ContainingRadius() + B.ContainingRadius(), 2);
  glm::vec3 const C0 = { A.GetObjectState().position };  // center of A
  glm::vec3 const C1 = { B.GetObjectState().position };  // center of B
  glm::vec3 const D  = { C1 - C0 };             // [DCD p. 6]
  float const actualR = glm::dot(D, D);         // by convention we do not divide by 4 in these calcs to speed up computation
  // if we are not within sphere, abort now
  return (actualR <= minR);
}


/* 
  notes on coordinate system
  The camera points along the z-axis (moving forward in the beginning is going to NEGATIVE z).
  This means that the horizontal (ground) plane is the xz-plane. Looking from "below" we see the following:

              (top14)____________(top13)
  z+       y+    /               /|
  ^        ^    /               / |
  |       /    /               /  |
  |      /(bot14)__________(bot13)|
  |     /    |         c0     |   /
  |    /     |          |_____|_ /
  |   /      |         /      | /
  |  /       |        /       |/
  | /     (bot11)____/_____(bot12)
  |/
  +- - - - - - - - - - - - - - - - - -> x+

  from the top it looks like this (notice the negative z axis).
  so the convention is that:
  top/bot11 is the lowest z and lowest x. in this way the bot / top xx is mirrored for all xx.
  note that this is before any rotation of the coordinates!
  
  z-
  ^
  |
  |      (top11) -------- (top12)
  |          |               |
  |          |               |
  |          |               |
  |          |               |
  |      (top14) -------- (top13)
  |
  +- - - - - - - - - - - - - - - - - - > x+

  the collision will use the algorithm (and naming!) described here:
  https://www.geometrictools.com/Documentation/DynamicCollisionDetection.pdf [DCD]
  it is also in the docs/ folder (at the project root dir). Doc will be referenced below.
*/
std::pair<bool, std::optional<glm::mat3>> CollisionDetector::CheckCollision(Object* A, Object* B, LastSeparatingAxis* lsa, bool* withinSphere)
{
  // skip test if spheres not overlapping
  glm::vec3 const C0 = { A->GetObjectState().position };  // center of A
  glm::vec3 const C1 = { B->GetObjectState().position };  // center of B
  glm::vec3 const D  = { C1 - C0 };             // [DCD p. 6]
  // if we are not within sphere, abort now
  if (!(*withinSphere = BoundingSphereOverlaps(*A, *B)))
  {
    return {false, {}};
  }

  // TODO: factor out these calls (can they be done before this). Then make this function const
  // check that BoundBox is not null here
  A->UpdateBoundBox();
  B->UpdateBoundBox();
  
  // generate the 15 axis [DCD Table 1, p. 7]
  // OPTIMIZE: later on, start with the easy ones (non-cross products)
  // and then generate one at a time and check - no reason to calculate if
  // one before fails
  std::array<glm::vec3, 15> L;
  int i = 0;
  L[i++] = A->mBoundBoxEdges.Ax;
  L[i++] = A->mBoundBoxEdges.Ay;
  L[i++] = A->mBoundBoxEdges.Az;
  L[i++] = B->mBoundBoxEdges.Ax;
  L[i++] = B->mBoundBoxEdges.Ay;
  L[i++] = B->mBoundBoxEdges.Az;
  const size_t noOfDims = 3; // also offset in L[] to get to the corresponding axis in the B object
  for (size_t n = 0; n < noOfDims; n++)
  {
    for (size_t m = 0; m < noOfDims; m++)
    {
      L[i++] = glm::cross(L[n], L[m + noOfDims]);
    }
  }

  // calculate c_ij from orthonormal edges [DCD p. 6]
  // OPTIMIZE: by only calculating c_ij as needed with A_i.dot(B_j)  [DCD p. 6]
  //  this can done by pre-allocating an array of floats (doubles?) and fill in as needed
  glm::mat3 const AEdges = glm::mat3(L[0], L[1], L[2]);
  glm::mat3 const BEdges = glm::mat3(L[3], L[4], L[5]);
  // matrix access is [col][row]
  glm::mat3 const C_ij { glm::transpose(AEdges) * BEdges };
  const size_t noOfTests = 15;
  // TODO: calculate and then test for each step. if we have a separating axis we can abondon immediately
  auto const [R0, R1, R] = CalcRArrays(C_ij, L, D, A->mScale, B->mScale);

  // OPTIMIZE: if any of the unit vectors of the 2 objects are aligned
  //           we do not need to check those axes again!
  for (size_t n = 0; n < noOfTests; n++)
  {
    if (R[n] > (R0[n] + R1[n]))
    {
      lsa->normal = L[n];
      lsa->index = n;
      return {false, {}};
    }
  }
  
  return {true, C_ij};
}


std::pair<bool, std::optional<glm::mat3>> CollisionDetector::CheckCollisionNew(Object* A, Object* B, LastSeparatingAxis* lsa, bool* withinSphere) const
{
  // skip test if spheres not overlapping
  glm::vec3 const C0 = { A->GetObjectState().position };  // center of A
  glm::vec3 const C1 = { B->GetObjectState().position };  // center of B
  glm::vec3 const D  = { C1 - C0 };             // [DCD p. 6]
  // if we are not within sphere, abort now
  if (!(*withinSphere = BoundingSphereOverlaps(*A, *B)))
  {
    return {false, {}};
  }
  
  // generate the 15 axis [DCD Table 1, p. 7]
  // OPTIMIZE: later on, start with the easy ones (non-cross products)
  // and then generate one at a time and check - no reason to calculate if
  // one before fails
  std::array<glm::vec3, 15> L;
  int i = 0;
  L[i++] = A->mBoundBoxEdges.Ax;
  L[i++] = A->mBoundBoxEdges.Ay;
  L[i++] = A->mBoundBoxEdges.Az;
  L[i++] = B->mBoundBoxEdges.Ax;
  L[i++] = B->mBoundBoxEdges.Ay;
  L[i++] = B->mBoundBoxEdges.Az;
  const size_t noOfDims = 3; // also offset in L[] to get to the corresponding axis in the B object
  for (size_t n = 0; n < noOfDims; n++)
  {
    for (size_t m = 0; m < noOfDims; m++)
    {
      L[i++] = glm::cross(L[n], L[m + noOfDims]);
    }
  }

  // calculate c_ij from orthonormal edges [DCD p. 6]
  // OPTIMIZE: by only calculating c_ij as needed with A_i.dot(B_j)  [DCD p. 6]
  //  this can done by pre-allocating an array of floats (doubles?) and fill in as needed
  glm::mat3 const AEdges = glm::mat3(L[0], L[1], L[2]);
  glm::mat3 const BEdges = glm::mat3(L[3], L[4], L[5]);
  // matrix access is [col][row]
  glm::mat3 const C_ij { glm::transpose(AEdges) * BEdges };
  const size_t noOfTests = 15;
  // TODO: calculate and then test for each step. if we have a separating axis we can abondon immediately
  auto const [R0, R1, R] = CalcRArrays(C_ij, L, D, A->mScale, B->mScale);

  // OPTIMIZE: if any of the unit vectors of the 2 objects are aligned
  //           we do not need to check those axes again!
  for (size_t n = 0; n < noOfTests; n++)
  {
    if (R[n] > (R0[n] + R1[n]))
    {
      lsa->normal = L[n];
      lsa->index = n;
      return {false, {}};
    }
  }
  
  return {true, C_ij};
}
