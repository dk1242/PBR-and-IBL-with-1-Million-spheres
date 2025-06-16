#include "InstanceVBO.h"

InstanceVBO::InstanceVBO()
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

InstanceVBO::InstanceVBO(const std::vector<glm::vec3>& instanceProps)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, 1000000 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW); // add vertex/color data in VBO
}

InstanceVBO::InstanceVBO(const std::vector<glm::vec2>& instanceProps)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, 1000000 * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW); // add material property data in VBO
}

void InstanceVBO::Update(std::vector<glm::vec3>& instanceProps)
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceProps.size() * sizeof(glm::vec3), instanceProps.data());
}

void InstanceVBO::Update(std::vector<glm::vec2>& instanceProps)
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceProps.size() * sizeof(glm::vec2), instanceProps.data());
}

void InstanceVBO::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void InstanceVBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstanceVBO::Delete() const
{
	glDeleteBuffers(1, &ID);
}
