#pragma once

#include <memory>
#include <vector>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class Mesh
{
public:
    Mesh(
        std::vector<GLfloat> vertices,
        std::vector<GLuint> indices
    );

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Draw();

private:
    VAO vao;

    std::unique_ptr<VBO> vbo;
    std::unique_ptr<EBO> ebo;

    GLsizei indexCount = 0;
};