#version 430 core

out vec4 FragColor;

in vec3 color;
in vec3 Normal;
in vec3 currPos;

uniform vec3 light1Pos = vec3(-180.0f, 180.0f, 180.0f);
uniform vec3 light2Pos = vec3(0.0f);

uniform vec3 camPos;
uniform samplerCube skybox;

vec4 pointLight(){
	
	vec3 lightVec = light1Pos - currPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.20f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light1Pos - currPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currPos);
	vec3 halfwayDir = normalize(lightDirection + viewDirection);

	vec3 R = reflect(viewDirection, normalize(Normal));
	
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(normal, halfwayDir), 0.0f), 8);
	float specular = specAmount * specularLight;

	return (vec4(texture(skybox, R).rgb, 1.0f) *  (diffuse + ambient) + specular);
}

vec4 directionalLight(){
	float ambient = 0.20f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light2Pos - currPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (vec4(color, 1.0f) *  (diffuse + ambient) + specular);
}

vec4 pointLight2(){
	
	vec3 lightVec = light2Pos - currPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.20f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light2Pos - currPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currPos);
	vec3 halfwayDir = normalize(lightDirection + viewDirection);

	vec3 R = reflect(viewDirection, normalize(Normal));
	
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(normal, halfwayDir), 0.0f), 8);
	float specular = specAmount * specularLight;

	return (vec4(texture(skybox, R).rgb, 1.0f) *  (diffuse + ambient) + specular);
}

void main()
{
	FragColor = pointLight() + pointLight2();
	//vec3 viewDirection = normalize(camPos - currPos);
	//vec3 R = reflect(viewDirection, normalize(Normal));
	//FragColor = vec4(texture(skybox, R).rgb, 1.0);
}