#version 430 core

out vec4 FragColor;

in vec3 Albedo; // = vec3(0.01);
in vec3 Normal;
in vec3 currPos;
in float METALNESS;
in float ROUGHNESS;

uniform float time; // for periods
uniform vec3 camPos; // camera position

// IBL maps
uniform samplerCube irradianceMap; // Diffuse irradiance map
uniform samplerCube prefilteredEnvMap; // Prefiltered environment map
uniform sampler2D brdfLUT; // BRDF LUT

// direct lights
const int NUM_LIGHTS = 9;
vec3 lightPositions[NUM_LIGHTS];
vec3 lightColors[NUM_LIGHTS];

uniform float ao = 1.0;  // ambient occlusion
const float PI = 3.14159265359;

// Fresnel-Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    // adjusting F0 based on roughness to account diffuse surface characteristics (blur reflections)
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// Distribution function: GGX
float DistributionGGX(vec3 N, vec3 H, float ROUGHNESS)
{
    float a = ROUGHNESS * ROUGHNESS;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Geometry function: Smith
float GeometrySchlickGGX(float NdotV, float ROUGHNESS)
{
    float r = (ROUGHNESS + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float ROUGHNESS)
{
    float ggx2 = GeometrySchlickGGX(max(dot(N, V), 0.0), ROUGHNESS);
    float ggx1 = GeometrySchlickGGX(max(dot(N, L), 0.0), ROUGHNESS);

    return ggx1 * ggx2;
}

void main()
{
	vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - currPos); // view direction vector
    vec3 R = reflect(-V, N); // reflection vector
    
	// Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04); // standard base reflectivity for dielectrics is 0.04
    F0 = mix(F0, Albedo, METALNESS);
    
    vec3 directLighting = vec3(0.0);

    // The numbers below are somewhat random and were chosen based on visual results.
    // This was determined through a trial-and-error approach.
    for (int i = 0; i < 9; i++) {
        float angle = time + i * 15;
        if(i == 0){
            lightPositions[i] = vec3(sin(angle) * 325.0f, 325.0f, cos(angle) * 325.0f);
            lightColors[i] = vec3(sin(angle), cos(angle), sin(angle)*cos(angle));
        }else if(i == 1){
            lightPositions[i] = vec3(sin(angle) * 325.0f, -325.0f, cos(angle) * 325.0f);
            lightColors[i] = vec3(1.0);//vec3(sin(angle), cos(angle), 0.5);
        }else if(i == 2){
            lightPositions[i] = vec3(-325.0f, sin(angle) * 325.0f, cos(angle) * 325.0f);
            lightColors[i] = vec3(cos(angle), sin(angle), sin(angle)*cos(angle));
        }else if(i == 3){
            lightPositions[i] = vec3(325.0f, sin(angle) * 325.0f, cos(angle) * 325.0f);
            lightColors[i] = vec3(1.0);//vec3(cos(angle), sin(angle), 0.5);
        }else if(i == 4){
            lightPositions[i] = vec3(sin(angle) * 325.0f, cos(angle) * 325.0f, -325.0f);
            lightColors[i] = vec3(sin(angle), sin(angle), cos(angle));
        }else if(i == 5){
            lightPositions[i] = vec3(sin(angle) * 325.0f, cos(angle) * 325.0f, 325.0f);
            lightColors[i] = vec3(1.0);//vec3(cos(angle), sin(angle), cos(angle));
        }else if(i == 6){
            lightPositions[i] = vec3(sin(angle) * 325.0f, cos(angle) * 325.0f, cos(angle) * 325.0f);
            lightColors[i] = vec3(sin(angle), cos(angle), sin(angle));
        }else if(i == 7){
            lightPositions[i] = vec3(sin(angle) * 325.0f, sin(angle) * 325.0f, cos(angle) * 325.0f);
            lightColors[i] = vec3(1.0);//vec3(cos(angle), cos(angle), sin(angle));
        }else{
            lightPositions[i] = vec3(1.0);
            lightColors[i] = vec3(1.0);
        }
        vec3 L = normalize(lightPositions[i] - currPos); // Light direction vector
        vec3 H = normalize(V + L); // Halfway Vector
        float distance = distance(lightPositions[i], currPos);

        float attenuation = 1.0 / (distance*distance) + (1.0 / distance); // its correct to use 1 / (distance * distance), 
                                                                          // but for more visual effect, adding (1 / distance)
        attenuation = clamp(attenuation, 0.0, 1.0);
        vec3 radiance = vec3(20.0) * attenuation * lightColors[i]; // scaling the radiance by 20, will add angular dependency in final lighting calculation
        radiance *= max(sin(time), 0); // to change light intensity over the time

	    // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, ROUGHNESS); // Normal Distribution Function (NDF) 
        float G = GeometrySmith(N, V, L, ROUGHNESS); // Geometry term
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0); // Fresnel reflectance term 
    
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // adding 0.001 so we will not divide numerator by 0 
        vec3 specular = numerator / denominator; // specular reflection component

        vec3 kS = F; // specular reflection coefficient 
        vec3 kD = vec3(1.0) - kS; // diffuse reflection coefficient
        kD *= 1.0 - METALNESS; // to reduce diffuse reflection for metallic surface

        float NdotL = max(dot(N, L), 0.0); // cosine of angle between normal and light direction
        directLighting += (kD * Albedo / PI + specular) * radiance * NdotL; // combining diffuse and specular components, 
                                                                            // scaled by radiance and NdotL 
    }
    
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, ROUGHNESS); // Fresnel reflection term with roughness
    
    vec3 kS = F; // specular reflection coefficient  
    vec3 kD = 1.0 - kS; // diffuse reflection coefficient
    kD *= 1.0 - METALNESS; // to reduce diffuse reflection for metallic surface

    // DIFFUSE IBL
    vec3 irradiance = texture(irradianceMap, N).rgb; // sampling irradianceMap based on N
    vec3 diffuseIBL = irradiance * Albedo; // diffuse contribution from the ambient environment

    // SPECULAR IBL
    float roughnessLevel = ROUGHNESS * 4.0; 
    vec3 prefilteredColor = textureLod(prefilteredEnvMap, R, roughnessLevel).rgb; // sampling prefilteredEnvMap 
                                                                                  // based on R and roughnesslevel (mipmap level)
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), ROUGHNESS)).rg; // sampling BRDF LookUp Texture using NdotV and roughness
    
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y); // specular contribution from the ambient environment
                                                                 // using prefiltered color and BRDF coefficients

    // combine diffuse and specular ibl
    vec3 ambientIBL = (kD * diffuseIBL + specularIBL) * ao;

    vec3 color = ambientIBL + directLighting; // final color with IBL and direct lighting values

    // Gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}