#version 330 core

#define MAX_WAVES 48

in vec3 vLocalPos;
in vec3 vWorldPos;
in vec3 vBary;

out vec4 FragColor;

uniform int uWaveCount;
uniform float uTime;
uniform float uWaveSpeed;

uniform vec4 uWaveA[MAX_WAVES];
uniform vec4 uWaveB[MAX_WAVES];

uniform vec3 uScale;
uniform float uFadeTime;
uniform int uEdgeMode; 

uniform vec3 uColor;

uniform int uSonarEnabled;
uniform vec3 uLightDir;

const float EDGE_WIDTH = 0.03;
const float RING_WIDTH = 0.2;

void main()
{
    if (uSonarEnabled == 0)
    {
        vec3 n = normalize(cross(dFdx(vWorldPos), dFdy(vWorldPos)));
        float diffuse = max(dot(n, -normalize(uLightDir)), 0.0);

        FragColor = vec4(uColor * (0.18 + 0.82 * diffuse), 1.0);
        return;
    }

    float shape;

    if (uEdgeMode == 0)
    {
        vec3 d = (vec3(0.5) - abs(vLocalPos)) * uScale;

        vec3 nearEdge = 1.0 - smoothstep(
            vec3(EDGE_WIDTH * 0.5), vec3(EDGE_WIDTH), d);

        shape = max(
            nearEdge.x * nearEdge.y,
            max(nearEdge.y * nearEdge.z, nearEdge.x * nearEdge.z));
    }
    else
    {
        float m = min(vBary.x, min(vBary.y, vBary.z));
        float w = max(fwidth(m) * 1.5, 0.00001);

        shape = 1.0 - smoothstep(0.0, w, m);
    }

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

        float ring = 1.0 - smoothstep(0.0, RING_WIDTH, abs(dist - radius));
        ringLight = max(ringLight, ring * strength);

        if (dist <= radius)
        {
            float since = age - dist / uWaveSpeed;
            float fade = 1.0 - clamp(since / uFadeTime, 0.0, 1.0);

            edgeLight = max(edgeLight, strength * fade * fade);
        }
    }

    float brightness = max(edgeLight * shape, ringLight);

    FragColor = vec4(uColor * brightness, 1.0);
}