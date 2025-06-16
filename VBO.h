#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>


class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(const std::vector<glm::vec3>& vertices);

	// Binds the VBO
	void Bind() const;
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete() const;
};

#endif