#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <vector>

// constructor with vectors
Camera::Camera(glm::vec3 const position/*  = glm::vec3(0.0f, 0.0f, 0.0f) */, 
               glm::vec3 const up/*  = glm::vec3(0.0f, 1.0f, 0.0f) */, 
               float const yaw/*  = YAW */, 
               float const pitch/*  = PITCH */) 
            : _position         (position)
            , _worldUp          (up)
            , _yaw              (yaw)
            , _pitch            (pitch)
{
    UpdateCameraVectors();
}
// constructor with scalar values
Camera::Camera(float const posX,
               float const posY,
               float const posZ,
               float const upX,
               float const upY,
               float const upZ,
               float const yaw,
               float const pitch)
            : Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ))
{
    UpdateCameraVectors();
}

glm::mat4 Camera::LookAtThisLookAtFunction(glm::vec3 const target) const
{
    glm::vec3 cameraDirection   = glm::normalize(_position - target);
    glm::vec3 cameraRight       = glm::normalize(glm::cross(_up, cameraDirection));
    glm::vec3 cameraUp          = glm::cross(cameraDirection, cameraRight);
    glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
    glm::mat4 rotation    = glm::mat4(1.0f); // Identity matrix by default
    translation[3][0] = -_position.x;
    translation[3][1] = -_position.y;
    translation[3][2] = -_position.z;
    rotation[0][0]    = cameraRight.x;
    rotation[1][0]    = cameraRight.y;
    rotation[2][0]    = cameraRight.z;
    rotation[0][1]    = cameraUp.x;
    rotation[1][1]    = cameraUp.y;
    rotation[2][1]    = cameraUp.z;
    rotation[0][2]    = cameraDirection.x;
    rotation[1][2]    = cameraDirection.y;
    rotation[2][2]    = cameraDirection.z;

    return (rotation * translation);

}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() const
{
    // return glm::lookAt(_position, _position + Front, Up);
    return LookAtThisLookAtFunction(_position + _front);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera::Movement const direction, float const deltaTime)
{
    float velocity = _movementSpeed * deltaTime;
    if (direction == Movement::FORWARD)
        _position += _front * velocity;
    if (direction == Movement::BACKWARD)
        _position -= _front * velocity;
    if (direction == Movement::LEFT)
        _position -= _right * velocity;
    if (direction == Movement::RIGHT)
        _position += _right * velocity;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean const constrainPitch/*  = true */)
{
    xoffset *= _mouseSensitivity;
    yoffset *= _mouseSensitivity;

    _yaw   += xoffset;
    _pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (_pitch > 89.0f)
            _pitch = 89.0f;
        if (_pitch < -89.0f)
            _pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float const yoffset)
{
    _zoom -= (float)yoffset;
    if (_zoom < 1.0f)
        _zoom = 1.0f;
    if (_zoom > 45.0f)
        _zoom = 45.0f;
}


// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    _right = glm::normalize(glm::cross(_front, _worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up    = glm::normalize(glm::cross(_right, _front));
}
