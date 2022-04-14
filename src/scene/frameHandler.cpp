
#include "frameHandler.h"

// void UpdateScene(float const deltaTime);
void FrameHandler::ForceForward(float deltaTime)
{

}

// break down everything into much simpler pieces

// - CONST Collision detection -> input 2 objects and minDeltaT for coll time, output time of collision (within input delteT)
//                           [what is the essential information from the objects that is needed?]
//                           - this function can be constant

// - CONST Collision points -> Separate out from detection? Again, this should not update anything, but simply 

// - NON-CONST Updating objects

// - draw objects
                        


void FrameHandler::Backward()
{
  // TODO: IsPaused should be moved to scene 
  // if (IsPaused())
  // {
    // TogglePause();
    // RestoreScene();
    // TogglePause();
  // }
}

// using ObjectPtrType = std::unique_ptr<Object>;

void FrameHandler::Forward(float deltaTime)
{
  if (_frameForwardDebug)
  {
    ForwardDebug(deltaTime);
  }
  else
  {
    ForwardNormal(deltaTime);
  }
}

void FrameHandler::ForwardNormal(float deltaTime)
{
  // SaveScene();
  // size_t totalLoops = 0;
  // deltaTime *= _timeMultiplier;
  // UpdatePos(deltaTime);
  // // do not calculate, just check
  // if (DetectCollisions(CollisionDetectionStop::kStopOnFirst))
  // {
  //   bool colDetected;
  //   float timeLeft = deltaTime;
  //   float updateTimeGran = deltaTime;
  //   // controls the detection granularity
  //   // could be made dependent on fastest relative collision speed for even better performance
  //   size_t const divDepthMax = 6;
  //   size_t maxTries = 50; // if we somehow get stuck
  //   size_t resolvedNumberForDebug = 0; // if we somehow get stuck

  //   float const updateTimeGranMin = updateTimeGran / std::pow(2, divDepthMax);

  //   while (true)
  //   {
  //     // reset depth
  //     size_t divDepth = 0; // 0 = full time step
  //     // bool breakNow = false; // FOR DEBUGGING
  //     do
  //     {
  //       totalLoops++; // DEBUG TEST
  //       // 1) go to previous position
  //       StepBackObjects();
  //       // 2) sub-divide
  //       updateTimeGran = std::max(updateTimeGranMin, updateTimeGran / 2);
        
  //       divDepth++;
  //       // 3) move objects forward again, but only with half the timestep
  //       UpdatePos(updateTimeGran);
  //       std::cout << "Div depth is : " << divDepth << " dt = " << updateTimeGran << std::endl;
  //     }
  //     while ((colDetected = DetectCollisions(CollisionDetectionStop::kStopOnFirst)) && // keep dividing while we have a collision AND
  //             (divDepth != divDepthMax));                       // not reached the max binary depth
  //     if (timeLeft <= updateTimeGran)
  //     {
  //       // the whole timesteps (deltaTime) has been updated
  //       break;
  //     }
  //     timeLeft -= updateTimeGran;
  //     std::cout << "Time left: " << timeLeft << std::endl;
  //     if (colDetected)
  //     {
  //       resolvedNumberForDebug++;
  //       // resolve the collisions
  //       if (!DetectCollisions(CollisionDetectionStop::kHandleAll))
  //       {
  //         std::cout << "UNEXPECTED!\n";
  //       }
  //       else
  //       {
  //         float colStartTime = (deltaTime - timeLeft);
  //         std::cout << "Collision between " << colStartTime << " - " << (colStartTime + updateTimeGran) << std::endl;
  //       }
  //     }
  //     updateTimeGran = timeLeft;
  //     // try to move to the end of the timestep
  //     UpdatePos(timeLeft);
  //     if (!DetectCollisions(CollisionDetectionStop::kStopOnFirst))
  //     {
  //       // if no collisions to resolve, exit at the final time step
  //       break;
  //     }
  //     if (maxTries-- == 0)
  //     {
  //       // TODO: if we're frame forwarding we should NOT set this
  //       _isPaused = true;
  //       std::cout << "EMERGENCY BREAK!! Timeleft: " << timeLeft << std::endl;
  //       // emergency break - something went wrong!
  //       break;
  //     }
  //   }
  //   std::cout << "Total loops: " <<  totalLoops << "  RESOLVED " << resolvedNumberForDebug << "\n";
  // }
}
  
void FrameHandler::ForwardDebug(float const deltaTime)
{
  // static FrameForwardState stateFF;
  // using State = FrameForwardState::State;
  // // SaveScene();

  // switch (stateFF.state)
  // {
  //   case State::Stopped:
  //   {
  //     std::cout << "Starting machine\n";
  //     deltaTime *= _timeMultiplier;
  //     std::cout << "Updating position\n";
  //     stateFF.Reset(deltaTime);
  //     UpdatePos(deltaTime);
  //     // if no detection, we resolve
  //     stateFF.SetResolved(!DetectCollisions(CollisionDetectionStop::kStopOnFirst));
    
  //     stateFF.state = stateFF.InProgress() ? State::TimeDividing_Step1 : State::Stopped;
  //     break;
  //   }
  //   case State::TimeDividing_Step1:
  //   {
  //     std::cout << "Resetting depth\n";
  //     stateFF.ResetDivDepth();
  //     stateFF.state = State::TimeDividing_Step2;
  //   } [[fallthrough]];
  //   case State::TimeDividing_Step2:
  //   {
  //     std::cout << "TimeDividing_Step2 -- stepping back objects\n";
  //     // 1) go to previous position
  //     StepBackObjects();
  //     // 2) sub-divide
  //     // updateTimeGran /= 2;
  //     // divDepth++;
  //     stateFF.DivDepth();
  //     std::cout << "Updating position\n";
  //     // 3) move objects forward again, but only with half the timestep
  //     UpdatePos(stateFF.GetTimeGranularity());
  //     std::cout << "Div depth is : " << stateFF.GetDivDepth() << " dt = " << stateFF.GetTimeGranularity() << std::endl;
  //     stateFF.SetResolved(!DetectCollisions(CollisionDetectionStop::kStopOnFirst));

  //     if (!stateFF.InProgress() ||        // keep dividing while we have a collision AND
  //         (stateFF.MaxDepthReached()))    // not reached the max binary depth
  //     {
        
  //       stateFF.state = State::CheckGraunlarity;
  //     }
  //     break;
  //   }
  //   case State::CheckGraunlarity:
  //   {
  //     std::cout << "Checking granularity\n";
  //     if (stateFF.TimeLeftLessThanGranularity())
  //     {
  //       std::cout << "Resolved here\n";
  //       stateFF.SetResolved(true);
  //       // the whole timesteps (deltaTime) has been updated
  //       stateFF.state = State::Stopped;
  //     }
  //     else
  //     {
  //       stateFF.state = State::HandleAllCollisions;
  //       stateFF.UpdateTime();
  //     }
  //     break;
  //   }
  //   case State::HandleAllCollisions:
  //   {
  //     std::cout << "HandleAllCollisions\n";
  //     if (stateFF.InProgress())
  //     {
  //       stateFF.allWasHandled = true;
  //       // resolvedNumberForDebug++;
  //       // resolve the collisions
  //       if (!DetectCollisions(CollisionDetectionStop::kHandleAll))
  //       {
  //         std::cout << "UNEXPECTED!\n";
  //       }

  //     }
  //     stateFF.state = State::CheckFinished;
  //     break;
  //   }
  //   case State::CheckFinished:
  //   {
  //     std::cout << "Check finished\n";
  //     // updateTimeGran = timeLeft;
  //     stateFF.SetUpdateTimeGranToTimeLeft();
  //     std::cout << "Updating position\n";
  //     // try to move to the end of the timestep
  //     UpdatePos(stateFF.GetTimeLeft());
  //     if (!DetectCollisions(CollisionDetectionStop::kStopOnFirst))
  //     {
  //       stateFF.SetResolved(true);
  //       stateFF.state = State::Stopped;
  //     }
  //     else if (stateFF.UpdateMaxTries())
  //     {
  //       std::cout << "EMERGENCY BREAK!! Timeleft: " << stateFF.GetTimeLeft() << std::endl;
  //       stateFF.SetResolved(true); // TODO: set with error?
  //       stateFF.state = State::Stopped;
  //       if (!stateFF.allWasHandled)
  //       {
  //         std::cout << "Wasn't handled!\n";
  //         exit(0);
  //       }
  //     }
  //     else
  //     {
  //       stateFF.state = State::TimeDividing_Step1;
  //       stateFF.allWasHandled = false;
  //     }
  //     break;
  //   }
  // }
}
