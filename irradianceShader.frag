#version 430 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube environmentMap; // Input cube map texture

const float PI = 3.14159265359;

void main() {
    vec3 N = normalize(localPos); // normal for hemisphere's orientation 
    vec3 irradiance = vec3(0.0);

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

    // Convolve the environment map for diffuse lighting
    float sampleDelta = 0.025;
    float sampleCount = 0;

    for(float phi = 0.0; phi<2.0*PI; phi+=sampleDelta){
        for(float theta = 0.0; theta<0.5*PI; theta+=sampleDelta){

            // spherical to cartesian coordinates in tangent space 
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            
            // tangent space to world space coordinates
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            // sample the envMap and add it to irradiance
            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            sampleCount++;
        }
    }

    irradiance = irradiance * (PI / float(sampleCount)); // Normalize by sample count

    FragColor = vec4(irradiance, 1.0); // Output irradiance
}