#version 430 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube environmentMap; // Input high-resolution cubemap
uniform float roughness;            // Roughness for current mipmap level

const float PI = 3.14159265359;
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    return a2 / (PI * (NdotH2 * (a2 - 1.0) + 1.0) * (NdotH2 * (a2 - 1.0) + 1.0));
}
// GGX Importance Sampling for hemisphere
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness; // Roughness squared
    float a2 = a * a;

    // Spherical coordinates for sampling hemisphere
    float phi = 2.0 * PI * Xi.x; // Azimuthal angle (Xi.x is in range [0, 1))
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a2 - 1.0) * Xi.y)); // Polar angle based on GGX distribution
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Cartesian coordinates in tangent space
    vec3 H = vec3(
        sinTheta * cos(phi), // x
        sinTheta * sin(phi), // y
        cosTheta             // z
    );

    // Transform H to world space using TBN matrix
    vec3 tangent = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0); // Arbitrary tangent vector
    vec3 bitangent = normalize(cross(N, tangent)); // Bitangent vector
    mat3 TBN = mat3(tangent, bitangent, N);        // Tangent-to-world matrix

    return normalize(TBN * H); // Return normalized sample direction in world space
}

void main() {
    vec3 N = normalize(localPos); // Surface normal
    vec3 R = N;                    // Reflection vector (ideal)
    vec3 V = R;                    // View direction (aligned with reflection)

    const int sampleCount = 1024;   // Number of samples
    vec3 prefilteredColor = vec3(0.0); // Accumulated reflected color
    float totalWeight = 0.0;

    for (int i = 0; i < sampleCount; ++i) {
        // Generate random sample direction using GGX importance sampling
        vec2 Xi = vec2(
            fract(sin(float(i) * 12.9898) * 43758.5453), // Random value 1
            fract(sin(float(i + 1) * 78.233) * 12345.6789) // Random value 2
        );

        vec3 H = ImportanceSampleGGX(Xi, N, roughness); // Half vector in world space
        vec3 L = normalize(reflect(-V, H));             // Reflect view direction over H

        // Weight based on the alignment of N and L
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            // sample from the environment's mip level based on roughness/pdf
            float D   = DistributionGGX(N, H, roughness); // standard PDF from PBR blog
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution); // solid angle per texel, 4*PI because of cubemap and 6 faces
            float saSample = 1.0 / (float(sampleCount) * pdf + 0.0001); // solid angle per sample

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);  
            
            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL; // Accumulate reflected color
            totalWeight += NdotL; // Accumulate weight
        }
    }
    if (totalWeight > 0.0) {
        prefilteredColor = prefilteredColor / totalWeight; // Normalize color by total weight
    }
    FragColor = vec4(prefilteredColor, 1.0); // Output prefiltered color
}