#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum class Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // constructor with vectors
    Camera(glm::vec3 const position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 const up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float const yaw = YAW, 
           float const pitch = PITCH);
    // constructor with scalar values
    Camera(float const posX,
           float const posY,
           float const posZ,
           float const upX,
           float const upY,
           float const upZ,
           float const yaw,
           float const pitch);

    float             GetMovementSpeed      () const { return _movementSpeed;    }
    float             GetMouseSensitivity   () const { return _mouseSensitivity; }
    float             GetZoom               () const { return _zoom;             }
    glm::vec3 const&  GetPosition           () const { return _position;         }
    glm::vec3 const&  GetFront              () const { return _front;            }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const;
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera::Movement const direction, float const deltaTime);
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean const constrainPitch = true);
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float const yoffset);

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
    glm::mat4 LookAtThisLookAtFunction(glm::vec3 const target) const;

    // Default camera values
    static constexpr float  YAW         { -90.0f };
    static constexpr float  PITCH       {  0.0f  };
    static constexpr float  SPEED       {  2.5f  };
    static constexpr float  SENSITIVITY {  0.1f  };
    static constexpr float  ZOOM        {  45.0f };

    // camera Attributes
    glm::vec3 _position;
    glm::vec3 _front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 _up;
    glm::vec3 _right;
    glm::vec3 _worldUp;
    // euler Angles
    float _yaw;
    float _pitch;
    // camera options
    float _movementSpeed     = SPEED;
    float _mouseSensitivity  = SENSITIVITY;
    float _zoom              = ZOOM;


};
#endif
