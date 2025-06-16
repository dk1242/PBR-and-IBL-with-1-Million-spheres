#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;

	Camera::left = -100.0f;
	Camera::right = 100.0f;
	Camera::top = 100.0f; 
	Camera::bottom = -100.0f;
	Camera::near = 0.0f;
	Camera::far = -100.0f;

	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);
}

void Camera::updateMatrix()
{
	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 300.0f);

	// Sets new camera matrix
	cameraMatrix = projection * view;
	UpdateFrustum();
}

void Camera::UpdateFrustum() {
	// Define the orthographic bounds relative to the camera
	float frustumWidth = 240.0f;
	float frustumHeight = (9.0f / 16.0f) * frustumWidth;
	float frustumDepth = 240.0f;

	// Frustum dimensions centered around the camera
	left	= Position.x - frustumWidth / 2.0f;
	right	= Position.x + frustumWidth / 2.0f;

	bottom	= Position.y - frustumHeight / 2.0f;
	top		= Position.y + frustumHeight / 2.0f;

	near	= Position.z;
	far		= Position.z - frustumDepth;

}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 2.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}
}