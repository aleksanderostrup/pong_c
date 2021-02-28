#include "../include/inputprocess.h"
#include <string>

InputProcess::InputProcess(GLFWwindow* window, Camera* camera) :
  mWindow (window),
  mCamera (camera)
{
}

void InputProcess::handleKeyPressedFirstTime(bool& key, bool& action, int glfwKey)
{
  if (glfwGetKey(mWindow, glfwKey) == GLFW_PRESS) 
  {
    if (!key) 
    {
      key = true;
      if (action)
      {
        action = false;
      }
      else 
      {
        action = true;
      }
    }
    else 
    {
      action = false;
    }
  }
  else if (key)
  {
    key = false;
    action = false;
  }
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
  // print debug handling
  handleKeyPressedFirstTime(keysPressed.isPrintDbgPressed, keyActions.printDebug, GLFW_KEY_P);
  handleKeyPressedFirstTime(keysPressed.isFrameForwardPressed, keyActions.frameForward, GLFW_KEY_RIGHT);
}
