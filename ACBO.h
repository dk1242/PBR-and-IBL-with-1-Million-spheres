#ifndef ACBO_CLASS_H
#define ACBO_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>
#include<string>

class ACBO
{
public:
	// Reference ID of the Atomic Counter Buffer Object
	GLuint ID;

	GLuint zero = 0;
	// Constructor that generates a Atomic Counter Buffer Object
	ACBO();

	void Reset() const;

	// Binds the ACBO
	void Bind() const;
	// Unbinds the ACBO
	void Unbind();
	// Deletes the ACBO
	void Delete() const;
};

#endif