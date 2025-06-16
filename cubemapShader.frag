#version 330 core

in vec3 localPos;
out vec4 FragColor;

uniform sampler2D hdrTexture; // equirectangular map

const vec2 invAtan = vec2(0.1591, 0.3183); // constants used for inverse trigonometric scaling to map spherical coordinates to UV space

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y)); // atan calculates longitude and asin calulates latitude
    uv *= invAtan; // scaling to fit the UV space
    uv += 0.5; // offset for maintaining the UV range
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(localPos)); // calculate uv coordinates
    vec3 color = texture(hdrTexture, uv).rgb; // sampling the hdrtexture to retrieve color at the uv direction
    
    FragColor = vec4(color, 1.0);
}