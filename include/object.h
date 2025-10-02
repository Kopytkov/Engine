#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <glm/glm.hpp>

struct Object {
    std::string name;
    glm::vec3 pos;
    glm::vec3 scale;
    std::string texture;
};

#endif