#include "inputprocess.h"
#include <string>

InputProcess::InputProcess(GLFWwindow* window, Camera* camera, Scene& scene) :
  mWindow (window),
  mCamera (camera),
  mScene  (scene)
{
}

void InputProcess::handleKeyPressedFirstTime(bool& key, bool& action, int glfwKey)
{
  if (glfwGetKey(mWindow, glfwKey) == GLFW_PRESS) 
  {
    if (!key) 
    {
      key = true;
      action = !action;
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

void InputProcess::BindKeyToAction(int const key, KeyActionCallback const cb)
{
  _registeredKeyMap.insert_or_assign(key, cb);
}

void InputProcess::processAllInput(float const deltaTime)
{
  processKeyboardInput(deltaTime);
  // processConsoleInput();
}

void InputProcess::processKeyboardInput(float const deltaTime)
{
  if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(mWindow, true);

  if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
    mCamera->ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
  if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
    mCamera->ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
  if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
    mCamera->ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
  if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
    mCamera->ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);


  for (auto& [key, pressHandler] : _registeredKeyMap)
  {
    // TODO: we can drop the "release" part and just use GLFW_TOGGLE or RELEASE or similar
    if (glfwGetKey(mWindow, key) == GLFW_PRESS)
      pressHandler(mScene);
    else
      pressHandler.release();
  }

  // print debug handling
  // handleKeyPressedFirstTime(keysPressed.isPausePressed,         keyActions.togglePause,   GLFW_KEY_SPACE);
  // handleKeyPressedFirstTime(keysPressed.isPrintDbgPressed,      keyActions.printDebug,    GLFW_KEY_P);
  handleKeyPressedFirstTime(keysPressed.isFrameForwardPressed,  keyActions.frameForward,  GLFW_KEY_RIGHT);
  // handleKeyPressedFirstTime(keysPressed.isFPressed,             keyActions.fAction,       GLFW_KEY_F);
  // handleKeyPressedFirstTime(keysPressed.isTPressed,             keyActions.tAction,       GLFW_KEY_T);
  // handleKeyPressedFirstTime(keysPressed.isUpKeyPressed,         keyActions.keyUpAction,   GLFW_KEY_UP);
  // handleKeyPressedFirstTime(keysPressed.isDownKeyPressed,       keyActions.keyDownAction, GLFW_KEY_DOWN);
  

}
