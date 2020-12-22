#include "../include/cube.h"

Cube::Cube(glm::vec3 position, float scale, Shader* shader, const char* name, float mass) : Box(position, glm::vec3(scale, scale, scale), shader, name, mass)
{

}
