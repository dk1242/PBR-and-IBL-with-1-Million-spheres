#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include <vector>
#include "shaderClass.h"

class Camera {
public:
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f); // z coming out of screen
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); // y is up
	glm::vec3 RightDir = glm::normalize(glm::cross(Orientation, Up)); // x is right
	
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	int width;
	int height;

	float left, right, top, bottom, near, far;

	float speed = 1.0f;
	
	Camera(int width, int height, glm::vec3 position);

	// Updates the camera matrix
	void updateMatrix(); // we are not going to update any variable like FOV angle, near or far plane value
	void UpdateFrustum();
	// Exports the camera matrix to shader
	void Matrix(Shader& shader, const char* uniform);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
};

#endif