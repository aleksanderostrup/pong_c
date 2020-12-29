#pragma once

#include "camera.h"
#include <GLFW/glfw3.h>

class InputProcess
{
  public:

    struct KeyboardActions {
      bool pause = false;
      bool printDebug = false;
    } keyActions;
    
    InputProcess(GLFWwindow* window, Camera* camera);

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    void processAllInput(float deltaTime);

  private:
    void processKeyboardInput(float deltaTime);

    GLFWwindow* mWindow;
    Camera*     mCamera;
    // bool        mPauseKeyPressed    = false;
    // bool        mPrintDbgKeyPressed = false;
    struct KeyboardPressed {
      bool isPausePressed = false;
      bool isPrintDbgPressed = false;
    } keysPressed;
};