#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vLocalPos;
out vec3 vWorldPos;

uniform mat4 uMVP;
uniform mat4 uModel;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);

    vLocalPos = aPos;
    vWorldPos = vec3(uModel * vec4(aPos, 1.0));
}