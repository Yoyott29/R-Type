#pragma once

#include "Structs.hpp"
#include <string>

class ProjectBuilder
{
  public:
    static std::string build(const std::string &projectPath, const SceneObjects &sceneObjects);
};
