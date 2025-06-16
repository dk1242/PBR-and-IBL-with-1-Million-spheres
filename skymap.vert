#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;

out vec3 localPos;

void main()
{
    localPos = aPos;
    vec4 pos = camMatrix * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Depth is set far away
}