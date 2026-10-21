#include "Mesh.h"

#include <limits>
#include <stdexcept>

Mesh::Mesh(
    std::vector<GLfloat> vertices,
    std::vector<GLuint> indices)
{
    constexpr std::size_t floatsPerVertex = 6;

    if (vertices.empty() ||
        vertices.size() % floatsPerVertex != 0)
    {
        vao.Delete();

        throw std::invalid_argument(
            "Mesh vertices must use the XYZ RGB format"
        );
    }

    if (indices.empty() || indices.size() % 3 != 0)
    {
        vao.Delete();

        throw std::invalid_argument(
            "Mesh indices must describe triangles"
        );
    }

    if (indices.size() >
        static_cast<std::size_t>(
            (std::numeric_limits<GLsizei>::max)()))
    {
        vao.Delete();

        throw std::invalid_argument(
            "Mesh has too many indices"
        );
    }

    const std::size_t vertexCount =
        vertices.size() / floatsPerVertex;

    for (GLuint index : indices)
    {
        if (index >= vertexCount)
        {
            vao.Delete();

            throw std::invalid_argument(
                "Mesh index is outside the vertex array"
            );
        }
    }

    try
    {
        vao.Bind();

        vbo = std::make_unique<VBO>(
            vertices.data(),
            static_cast<GLsizeiptr>(
                vertices.size() * sizeof(GLfloat))
        );

        ebo = std::make_unique<EBO>(
            indices.data(),
            static_cast<GLsizeiptr>(
                indices.size() * sizeof(GLuint))
        );

        vao.LinkAttrib(
            *vbo,
            0,
            3,
            GL_FLOAT,
            6 * sizeof(GLfloat),
            nullptr
        );

        vao.LinkAttrib(
            *vbo,
            1,
            3,
            GL_FLOAT,
            6 * sizeof(GLfloat),
            reinterpret_cast<void*>(3 * sizeof(GLfloat))
        );

        vao.Unbind();

        vbo->Unbind();

        indexCount = static_cast<GLsizei>(indices.size());
    }
    catch (...)
    {
        vao.Unbind();
        vao.Delete();

        if (ebo)
        {
            ebo->Delete();
        }

        if (vbo)
        {
            vbo->Delete();
        }

        throw;
    }
}

Mesh::~Mesh()
{
    vao.Delete();

    if (ebo)
    {
        ebo->Delete();
    }

    if (vbo)
    {
        vbo->Delete();
    }
}

void Mesh::Draw()
{
    vao.Bind();

    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_INT,
        nullptr
    );

    vao.Unbind();
}