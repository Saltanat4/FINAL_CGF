#pragma once

#include <memory>

#include "Mesh.h"

namespace MeshFactory
{
    std::shared_ptr<Mesh> CreateCube();
}