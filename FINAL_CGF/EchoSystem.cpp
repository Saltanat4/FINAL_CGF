#include "EchoSystem.h"

#include <algorithm>

void EchoSystem::Spawn(
    const glm::vec3& origin,
    float time,
    float maxRadius,
    float strength)
{
    if (static_cast<int>(waves.size()) >= kMaxWaves)
    {
        waves.erase(waves.begin());
    }

    waves.push_back(EchoWave{ origin, time, maxRadius, strength });
}

void EchoSystem::Update(float time)
{
    waves.erase(
        std::remove_if(
            waves.begin(),
            waves.end(),
            [time](const EchoWave& wave)
            {
                return (time - wave.startTime) >
                    wave.maxRadius / kWaveSpeed + kMaxFadeTime;
            }),
        waves.end());
}

void EchoSystem::Clear()
{
    waves.clear();
}

const std::vector<EchoWave>& EchoSystem::GetWaves() const
{
    return waves;
}