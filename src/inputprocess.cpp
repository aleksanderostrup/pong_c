#include "../include/inputprocess.h"
#include <string>

InputProcess::InputProcess(GLFWwindow* window, Camera* camera) :
  mWindow (window),
  mCamera (camera)
{
}

void InputProcess::processAllInput(float deltaTime)
{
  processKeyboardInput(deltaTime);
  // processConsoleInput();
}

void InputProcess::processKeyboardInput(float deltaTime)
{
  if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(mWindow, true);

  if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
    mCamera->ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
    mCamera->ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
    mCamera->ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
    mCamera->ProcessKeyboard(RIGHT, deltaTime);
  if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) 
  {
    if (!keysPressed.isPausePressed)
    {
      keysPressed.isPausePressed = true;
      keyActions.pause = !keyActions.pause; // toggle pause
    }
  }
  else 
  {
    keysPressed.isPausePressed = false;
  }
  if (glfwGetKey(mWindow, GLFW_KEY_P) == GLFW_PRESS) 
  {
    if (!keysPressed.isPrintDbgPressed) 
    {
      keysPressed.isPrintDbgPressed = true;
      if (keyActions.printDebug)
      {
        keyActions.printDebug = false;
      }
      else 
      {
        keyActions.printDebug = true;
      }
    }
    else 
    {
      keyActions.printDebug = false;
    }
  }
  else if (keysPressed.isPrintDbgPressed)
  {
    keysPressed.isPrintDbgPressed = false;
    keyActions.printDebug = false;
  }
}
