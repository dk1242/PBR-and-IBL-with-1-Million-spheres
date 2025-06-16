#ifndef SSBO_CLASS_H
#define SSBO_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>
#include<string>

class SSBO
{
public:
	// Reference ID of the Shader Storage Buffer Object
	GLuint ID;

	// Constructor that generates a Shader Storage Buffer Object and links it to data
	SSBO(const std::vector<glm::vec3>& instanceData, std::string bufferType);
	SSBO(const std::vector<glm::vec2>& instanceData, std::string bufferType);

	void Clean() const;

	// Binds the SSBO
	void Bind() const;
	// Unbinds the SSBO
	void Unbind();
	// Deletes the SSBO
	void Delete() const;
};

#endif