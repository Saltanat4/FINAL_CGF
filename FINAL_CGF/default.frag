#version 330 core

#define MAX_WAVES 48

in vec3 vLocalPos;
in vec3 vWorldPos;

out vec4 FragColor;

uniform int uWaveCount;
uniform float uTime;
uniform float uWaveSpeed;

// A: xyz = источник, w = время старта
// B: x = макс. радиус, y = яркость
uniform vec4 uWaveA[MAX_WAVES];
uniform vec4 uWaveB[MAX_WAVES];

uniform vec3 uScale;
uniform float uFadeTime;

const float EDGE_WIDTH = 0.02;
const float RING_WIDTH = 0.2;

void main()
{
    // Расстояние в метрах до граней по каждой оси
    vec3 d = (vec3(0.5) - abs(vLocalPos)) * uScale;

    vec3 nearEdge = 1.0 - smoothstep(
        vec3(EDGE_WIDTH * 0.5), vec3(EDGE_WIDTH), d);

    float edgeMask = max(
        nearEdge.x * nearEdge   .y,
        max(nearEdge.y * nearEdge.z, nearEdge.x * nearEdge.z));

    float shape = edgeMask;

    float ringLight = 0.0;
    float edgeLight = 0.0;

    for (int i = 0; i < uWaveCount; ++i)
    {
        vec4 A = uWaveA[i];
        vec4 B = uWaveB[i];

        float age = uTime - A.w;

        if (age < 0.0)
        {
            continue;
        }

        float dist = distance(vWorldPos, A.xyz);

        if (dist > B.x)
        {
            continue;
        }

        float strength = B.y * (1.0 - dist / B.x);
        float radius = age * uWaveSpeed;

        // Белое кольцо на поверхности
        float ring = 1.0 - smoothstep(0.0, RING_WIDTH, abs(dist - radius));
        ringLight = max(ringLight, ring * strength);

        // Рёбра и вершины, которых волна уже коснулась
        if (dist <= radius)
        {
            float since = age - dist / uWaveSpeed;
            float fade = 1.0 - clamp(since / uFadeTime, 0.0, 1.0);

            edgeLight = max(edgeLight, strength * fade * fade);
        }
    }

    float brightness = max(edgeLight * shape, ringLight);

    FragColor = vec4(vec3(brightness), 1.0);
}