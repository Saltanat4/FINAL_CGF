#include "ModelLoader.h"

#include <array>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

std::shared_ptr<Mesh> ModelLoader::Load(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Cannot open model: " + path);
    }

    std::vector<glm::vec3> positions;
    std::vector<GLuint> indices;

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream stream(line);

        std::string tag;
        stream >> tag;

        if (tag == "v")
        {
            glm::vec3 p(0.0f);
            stream >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (tag == "f")
        {
            std::vector<int> face;
            std::string token;

            while (stream >> token)
            {
                int index = std::stoi(token.substr(0, token.find('/')));

                index = index < 0
                    ? static_cast<int>(positions.size()) + index
                    : index - 1;

                if (index < 0 ||
                    index >= static_cast<int>(positions.size()))
                {
                    throw std::runtime_error(
                        "Bad face index in model: " + path);
                }

                face.push_back(index);
            }

            for (std::size_t i = 1; i + 1 < face.size(); ++i)
            {
                indices.push_back(static_cast<GLuint>(face[0]));
                indices.push_back(static_cast<GLuint>(face[i]));
                indices.push_back(static_cast<GLuint>(face[i + 1]));
            }
        }
    }

    if (positions.empty() || indices.empty())
    {
        throw std::runtime_error("Model has no geometry: " + path);
    }

    // Вписываем в куб [-0.5; 0.5] по каждой оси
    glm::vec3 minP = positions[0];
    glm::vec3 maxP = positions[0];

    for (const glm::vec3& p : positions)
    {
        minP = glm::min(minP, p);
        maxP = glm::max(maxP, p);
    }

    const glm::vec3 center = (minP + maxP) * 0.5f;
    glm::vec3 extent = maxP - minP;

    for (int c = 0; c < 3; ++c)
    {
        if (extent[c] < 0.000001f)
        {
            extent[c] = 1.0f;
        }
    }

    std::vector<GLfloat> vertices;
    vertices.reserve(positions.size() * 3);

    for (const glm::vec3& p : positions)
    {
        const glm::vec3 q = (p - center) / extent;

        vertices.push_back(q.x);
        vertices.push_back(q.y);
        vertices.push_back(q.z);
    }

    return std::make_shared<Mesh>(
        std::move(vertices),
        std::move(indices));
}