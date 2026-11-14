#include "MeshFactory.h"

#include <utility>
#include <vector>

std::shared_ptr<Mesh> MeshFactory::CreateCube()
{
    std::vector<GLfloat> vertices = {
        // Позиция 

        -0.5f, -0.5f, -0.5f,    
         0.5f, -0.5f, -0.5f,    
         0.5f,  0.5f, -0.5f,   
        -0.5f,  0.5f, -0.5f,    

        -0.5f, -0.5f,  0.5f,   
         0.5f, -0.5f,  0.5f,   
         0.5f,  0.5f,  0.5f,    
        -0.5f,  0.5f,  0.5f,    
    };

    std::vector<GLuint> indices = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,

        0, 4, 7,  7, 3, 0,
        1, 5, 6,  6, 2, 1,

        0, 1, 5,  5, 4, 0,
        3, 2, 6,  6, 7, 3
    };

    auto mesh = std::make_shared<Mesh>(
        std::move(vertices),
        std::move(indices)
    );

    mesh->boxEdges = true;

    return mesh;
}   