#include "ModelLoader.h"

#include <array>
#include <cmath>
#include <fstream>
#include <map>
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
    std::vector<std::array<int, 3>> triangles;

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
                triangles.push_back({ face[0], face[i], face[i + 1] });
            }
        }
    }

    if (positions.empty() || triangles.empty())
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

    for (glm::vec3& p : positions)
    {
        p = (p - center) / extent;
    }

    // Нормали треугольников и список треугольников для каждой стороны
    std::vector<glm::vec3> normals(triangles.size(), glm::vec3(0.0f));
    std::map<std::pair<int, int>, std::vector<int>> sideTriangles;

    for (std::size_t t = 0; t < triangles.size(); ++t)
    {
        const auto& ids = triangles[t];

        glm::vec3 n = glm::cross(
            positions[ids[1]] - positions[ids[0]],
            positions[ids[2]] - positions[ids[0]]);

        const float length = glm::length(n);

        if (length > 0.000000001f)
        {
            normals[t] = n / length;
        }

        for (int e = 0; e < 3; ++e)
        {
            int a = ids[e];
            int b = ids[(e + 1) % 3];

            if (a > b)
            {
                std::swap(a, b);
            }

            sideTriangles[{ a, b }].push_back(static_cast<int>(t));
        }
    }

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (std::size_t t = 0; t < triangles.size(); ++t)
    {
        const auto& ids = triangles[t];

        // hide[c] = 1: сторона напротив вершины c не рисуется
        float hide[3] = { 0.0f, 0.0f, 0.0f };

        for (int e = 0; e < 3; ++e)
        {
            int a = ids[e];
            int b = ids[(e + 1) % 3];

            if (a > b)
            {
                std::swap(a, b);
            }

            const std::vector<int>& shared = sideTriangles[{ a, b }];

            // Соседи лежат в одной плоскости: это диагональ триангуляции
            if (shared.size() == 2 &&
                std::abs(glm::dot(
                    normals[shared[0]],
                    normals[shared[1]])) > 0.999f)
            {
                hide[(e + 2) % 3] = 1.0f;
            }
        }

        for (int k = 0; k < 3; ++k)
        {
            const glm::vec3& p = positions[ids[k]];

            vertices.push_back(p.x);
            vertices.push_back(p.y);
            vertices.push_back(p.z);

            for (int c = 0; c < 3; ++c)
            {
                vertices.push_back(
                    (c == k || hide[c] > 0.0f) ? 1.0f : 0.0f);
            }

            indices.push_back(static_cast<GLuint>(indices.size()));
        }
    }

    return std::make_shared<Mesh>(
        std::move(vertices),
        std::move(indices),
        true);
}