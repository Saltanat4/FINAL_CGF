#pragma once

#include <vector>
#include <glm/glm.hpp>

struct EchoWave
{
    glm::vec3 origin;
    float startTime;
    float maxRadius;
    float strength;
};

class EchoSystem
{
public:
    static constexpr int kMaxWaves = 48;
    static constexpr float kWaveSpeed = 12.0f;
    static constexpr float kMaxFadeTime = 8.0f;

    void Spawn(
        const glm::vec3& origin,
        float time,
        float maxRadius,
        float strength);

    void Update(float time);

    void Clear();

    const std::vector<EchoWave>& GetWaves() const;

private:
    std::vector<EchoWave> waves;
};