#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 camMatrix;

out vec3 localPos;

void main() {
    localPos = aPos; // Pass vertex positions to fragment shader
    gl_Position = camMatrix * vec4(aPos, 1.0); // Transform cube vertices
}