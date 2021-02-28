#include "../include/glfw_setup.h"
#include <iostream>

static GLFWwindow* _window = nullptr;
static Camera*     _camera = nullptr;
static float       _lastX;
static float       _lastY;
static bool        _firstMouse = true;


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (_firstMouse)
    {
        _lastX = xpos;
        _lastY = ypos;
        _firstMouse = false;
    }

    float xoffset = xpos - _lastX;
    float yoffset = _lastY - ypos; // reversed since y-coordinates go from bottom to top

    _lastX = xpos;
    _lastY = ypos;

    _camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    _camera->ProcessMouseScroll(yoffset);
}

GLFWwindow* glfw_init(Camera* camera, unsigned int scr_width, unsigned int scr_height)
{
    _camera = camera;
    _lastX = (float)scr_width / 2.0;
    _lastY = (float)scr_height / 2.0;

    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit()) 
    {
        std::cout << "Failed to init GLFW" << std::endl;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    _window = glfwCreateWindow(scr_width, scr_height, "Aleks Physics Engine", NULL, NULL);
    if (_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return _window;
    }

    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(_window, mouse_callback);
    glfwSetScrollCallback(_window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return _window;
}