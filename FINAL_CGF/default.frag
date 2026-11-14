#version 330 core

in vec3 vLocalPos;

out vec4 FragColor;

void main()
{
    vec3 gradient = mix(vec3(0.3), vec3(1.0), vLocalPos + 0.5);

    FragColor = vec4(gradient, 1.0);
}