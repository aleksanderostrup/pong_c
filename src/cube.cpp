#include "../include/cube.h"

Cube::Cube(glm::vec3 position, float scale, const char* name, float mass) : Box(position, glm::vec3(scale, scale, scale), name, mass)
{

}
