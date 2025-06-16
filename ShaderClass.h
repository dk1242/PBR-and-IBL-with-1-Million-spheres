#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader {
public:
	GLuint ID;
	std::string shaderName;
	Shader(const char* vertexFilePath, const char* fragmentFilePath, const char* shaderName);
	Shader(const char* computeShaderFilePath, const char* shaderName);

	void Activate() const; // for using a shader
	void Delete() const;
private:
	void compileErrors(unsigned int shader, const char* type);
};

#endif
