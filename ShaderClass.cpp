#include "ShaderClass.h"

std::string get_file_contents(const char* filename)
{
	// standard way of reading a file content and storing it in a string
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath, const char* shaderName)
{
	Shader::shaderName = shaderName;
	std::string vertexCode = get_file_contents(vertexFilePath);
	std::string fragmentCode = get_file_contents(fragmentFilePath);

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // vertex shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // attaching src to object
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);// fragment shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	ID = glCreateProgram(); // shader program object
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID); // wrap-up/link all shaders together
	compileErrors(ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(const char* computeShaderFilePath, const char* shaderName)
{
	Shader::shaderName = shaderName;
	std::string computeCode = get_file_contents(computeShaderFilePath);
	const char* computeShaderSource = computeCode.c_str();
	
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER); // compute shader object
	glShaderSource(computeShader, 1, &computeShaderSource, NULL); // attaching src to object
	glCompileShader(computeShader);

	compileErrors(computeShader, "CULLING_LOD_COMPUTE");

	ID = glCreateProgram(); // shader program object
	glAttachShader(ID, computeShader);
	glLinkProgram(ID); // wrap-up/link all shaders together
	compileErrors(ID, "PROGRAM");

	glDeleteShader(computeShader);
}

void Shader::Activate() const
{
	glUseProgram(ID);
}

void Shader::Delete() const
{
	glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		// when shader code
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << shaderName << "\n" << infoLog << std::endl;
		}
		else {
			std::cout << "SHADER_COMPILATION_SUCCESS for: " << shaderName << "\n";
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for: " << shaderName << "\n" << infoLog << std::endl;
		}
		else {
			std::cout << "SHADER_COMPILATION_SUCCESS for: " << shaderName << "\n";
		}
	}
}
