#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform vec2 objOffset;

void main()
{
    vec3 newPos = aPos;
    newPos.x += objOffset.x;
    newPos.y += objOffset.y;
    
    gl_Position = vec4(newPos, 1.0);
    color = aColor;
}