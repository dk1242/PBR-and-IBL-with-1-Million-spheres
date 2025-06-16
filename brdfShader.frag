#version 330 core
out vec2 FragColor;

in vec2 TexCoords;

const float PI = 3.14159265359;

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H; // Halfway Vector in tangent space
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness;
    float k = (r * r) / 2.0; // for IBL, k is different
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
    const uint SAMPLE_COUNT = 2048u;
    vec3 V;
    V.y = 0.0; // vector lies on the xz-plane
    V.z = NdotV; // z-coordinate to cosine of angle between N and V
    V.x = sqrt(1.0 - NdotV * NdotV); // x can be calculated using pythagorean theorem
    
    float A = 0.0;
    float B = 0.0; 

    vec3 N = vec3(0.0, 0.0, 1.0); // Normal coming out of screen
        
    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = vec2(float(i) / float(SAMPLE_COUNT), fract(sin(float(i) * 12.9898) * 43758.5453123));
        vec3 H = ImportanceSampleGGX(Xi, N, roughness); // halfway vector
        vec3 L = normalize(2.0 * dot(V, H) * H - V); // reflection of V about H

        float NdotL = max(L.z, 0.0); // cosine of angle between N and L, L.z used as N is aligned with z-axis
        float NdotH = max(H.z, 0.0); // similar to NdotL
        float VdotH = max(dot(V, H), 0.0); // dot product of V and H

        if(NdotL > 0.0) {
            float G = GeometrySmith(N, V, L, roughness); // Geometry term
            float G_Vis = (G * VdotH) / (NdotH * NdotV + 0.001);  // Visibility term combining geometry and Fresnel effects
            float Fc = pow(1.0 - VdotH, 5.0); // Fresnel term

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

void main() 
{
    vec2 uv = TexCoords;
    float NdotV = uv.x; // x-axis defining the angle between N & V
    float roughness = uv.y; // y-axis defining the roughness
    vec2 integratedBRDF = IntegrateBRDF(NdotV, roughness);
    FragColor = integratedBRDF;
}