#version 430 core

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 instancePos;
//layout (location = 2) in vec3 instanceColor;
//layout (location = 3) in vec2 instanceMaterialProps;

// SSBO buffers
layout(std430, binding = 1) buffer Positions {
    vec3 positions[];
};
layout(std430, binding = 2) buffer Colors {
    vec3 colors[];
};
layout(std430, binding = 3) buffer MaterialProps {
    vec2 materialProps[];
};

// Uniforms (importing camera matrix, model, camera postion from main)
uniform mat4 camMatrix; // Camera matrix for view and projection (= view X projection)
uniform mat4 model; // Model matrix
uniform vec3 camPos; // camera Position in world space

// inputs for the fragment shader
out vec3 Albedo; // color
out vec3 Normal; // Normal vector
out vec3 currPos; // current vertex position in world space
out float METALNESS;
out float ROUGHNESS;

void main()
{
	// Fetch instance-specific data from SSBOs
	vec3 instancePos = positions[gl_InstanceID];
	vec3 instanceColor = colors[gl_InstanceID];
	vec2 instanceMaterialProps = materialProps[gl_InstanceID];

	// Scale factor based on camera distance
	float cameraDistance = length(camPos - instancePos);
	float scaleFactor = clamp(1.0 / (cameraDistance * 0.1), 0.75, 1.0); 

	currPos = vec3(model * vec4(aPos * scaleFactor + instancePos, 1.0f)); // world space position of vertex (adding instancePos)
	gl_Position = camMatrix * vec4(currPos, 1.0); // transforming to clip space

	// normal vector transformed with model matrix
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	Normal = normalize(normalMatrix * normalize(aPos));

	Albedo = instanceColor;
	METALNESS = instanceMaterialProps.x;
	ROUGHNESS = instanceMaterialProps.y;
}