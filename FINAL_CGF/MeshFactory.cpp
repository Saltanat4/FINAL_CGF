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
        0, 2, 1,  2, 0, 3,   // задняя  (-Z)
        4, 5, 6,  6, 7, 4,   // передняя (+Z)

        0, 4, 7,  7, 3, 0,   // левая   (-X)
        1, 6, 5,  6, 1, 2,   // правая  (+X)

        0, 1, 5,  5, 4, 0,   // нижняя  (-Y)
        3, 6, 2,  6, 3, 7    // верхняя (+Y)
    };

    auto mesh = std::make_shared<Mesh>(
        std::move(vertices),
        std::move(indices)
    );

    mesh->boxEdges = true;

    return mesh;
}   