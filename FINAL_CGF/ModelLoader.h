#pragma once

#include <memory>
#include <string>

#include "Mesh.h"

class ModelLoader
{
public:
    static std::shared_ptr<Mesh> Load(const std::string& path);
};