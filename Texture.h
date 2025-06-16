#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<vector>

#include"shaderClass.h"

class Texture {
public:
	GLuint ID = 0;
	const char* type = "diffuse"; // 

	Texture(const char* imagePath, const char* texType, GLenum slot, GLenum format, GLenum pixelType);
	Texture(std::vector <std::string>& cubeFaces); // for skybox (6 cube faces)
	
	// Constructor for HDR texture
	Texture(const char* imagePath, GLenum slot, bool itsHDR);

	// Assigns a texture unit to a Shader
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind() const;
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete() const;

};

#endif