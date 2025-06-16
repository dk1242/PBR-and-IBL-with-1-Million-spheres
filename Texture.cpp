#include "Texture.h"

Texture::Texture(const char* imagePath, const char* texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;

	// opengl generate texture
	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, ID);

	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	// loading image
	unsigned char* bytes = stbi_load(imagePath, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes) {
		std::cerr << "Failed to load HDR texture at path: " << imagePath << std::endl;
		return;
	}
	else {
		std::cout << imagePath << " loaded correctly.\n";
	}

	// setting texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes); // clearing the memory
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(std::vector<std::string>& cubeFaces)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < cubeFaces.size(); i++)
	{
		unsigned char* bytes = stbi_load(cubeFaces[i].c_str(), &width, &height, &nrComponents, 0);
		if (bytes)
		{
			// for all 6 planes defined by GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
			stbi_image_free(bytes);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << cubeFaces[i] << std::endl;
			stbi_image_free(bytes);
		}
	}
	// setting texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Texture::Texture(const char* imagePath, GLenum slot, bool itsHDR)
{
	type = "HDR";

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, ID);

	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(true);
	float* bytes = stbi_loadf(imagePath, &width, &height, &numChannels, 0); // loading HDR

	if (!bytes) {
		std::cerr << "Failed to load HDR texture at path: " << imagePath << std::endl;
		return;
	}
	else {
		std::cout << imagePath << " loaded correctly.\n";
	}

	// Load HDR bytes into the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, bytes);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint uniTex0 = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(uniTex0, unit);
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete() const
{
	glDeleteTextures(1, &ID);
}
