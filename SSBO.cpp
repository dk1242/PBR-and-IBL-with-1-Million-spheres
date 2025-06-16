#include "SSBO.h"

SSBO::SSBO(const std::vector<glm::vec3>& instanceData, std::string bufferType)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
	if(bufferType == "justForSpace")
		glBufferData(GL_SHADER_STORAGE_BUFFER, instanceData.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	else
		glBufferData(GL_SHADER_STORAGE_BUFFER, instanceData.size() * sizeof(glm::vec3), instanceData.data(), GL_STATIC_DRAW);
}

SSBO::SSBO(const std::vector<glm::vec2>& instanceData, std::string bufferType)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
	if (bufferType == "justForSpace")
		glBufferData(GL_SHADER_STORAGE_BUFFER, instanceData.size() * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	else
		glBufferData(GL_SHADER_STORAGE_BUFFER, instanceData.size() * sizeof(glm::vec2), instanceData.data(), GL_STATIC_DRAW);
}

void SSBO::Clean() const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1000000 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
}

void SSBO::Bind() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
}

void SSBO::Unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::Delete() const
{
	glDeleteBuffers(1, &ID);
}
