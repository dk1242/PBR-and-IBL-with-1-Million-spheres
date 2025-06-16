#version 330 core
in vec3 localPos;

uniform samplerCube skybox;

out vec4 FragColor;

void main()
{
    vec3 color = texture(skybox, localPos).rgb;

    // Gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}