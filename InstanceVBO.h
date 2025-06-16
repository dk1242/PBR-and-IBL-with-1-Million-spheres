#ifndef INSTANCE_VBO_CLASS_H
#define INSTANCE_VBO_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>

class InstanceVBO
{
public:
	GLuint ID;

	InstanceVBO();
	InstanceVBO(const std::vector<glm::vec3>& instanceProps);
	InstanceVBO(const std::vector<glm::vec2>& instanceProps);

	void Update(std::vector<glm::vec3>& instanceProps);
	void Update(std::vector<glm::vec2>& instanceProps);

	// Binds the InstanceVBO
	void Bind() const;
	// Unbinds the InstanceVBO
	void Unbind();
	// Deletes the InstanceVBO
	void Delete() const;
};

#endif