#pragma once

#include "camera.h"
#include <GLFW/glfw3.h>
#include <map>
#include "scene.h"

class InputProcess
{
  public:

    struct KeyboardActions {
      bool togglePause   = false;
      bool printDebug    = false;
      bool frameForward  = false;
      bool fAction       = false;
      bool tAction       = false;
      bool keyUpAction   = false;
      bool keyDownAction = false;
    } keyActions;
    
    InputProcess(GLFWwindow* window, Camera* camera, Scene& scene);

    using KeyActionCallback = void(*)(Scene&);

    void BindKeyToAction(int const key, KeyActionCallback const cb);
    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    void processAllInput(float const deltaTime);

  private:
    struct KeyboardPressHandler
    {
      KeyboardPressHandler(KeyActionCallback cb) : _cb(cb) {}
      void operator ()(Scene& scene) {
        if (!_isPressed)
        {
          _cb(scene);
          _isPressed = true;
        }
      }
      void release()
      {
        if (_isPressed)
        {
          _isPressed = false;
        }
      }
    private:
      KeyActionCallback _cb;
      bool _isPressed = false;
    };

    void processKeyboardInput       (float const deltaTime);
    void handleKeyPressedFirstTime  (bool& key, bool& action, int glfwKey);
    std::map<int, KeyboardPressHandler> _registeredKeyMap;

    GLFWwindow* mWindow;
    Camera*     mCamera;
    Scene&      mScene;
    // bool        mPauseKeyPressed    = false;
    // bool        mPrintDbgKeyPressed = false;
    struct KeyboardPressed {
      bool isPausePressed        = false;
      bool isPrintDbgPressed     = false;
      bool isFrameForwardPressed = false;
      bool isFrameFPressed       = false;
      bool isFPressed            = false;
      bool isTPressed            = false;
      bool isUpKeyPressed        = false;
      bool isDownKeyPressed      = false;

    } keysPressed;
};