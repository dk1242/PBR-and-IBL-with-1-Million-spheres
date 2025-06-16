#ifndef TEXTURE_UTILITIES_H
#define TEXTURE_UTILITIES_H

#include <glad/glad.h>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include "Mesh.h"

class TextureUtilities {
public:
    TextureUtilities();

    glm::mat4 captureProjection; // projection Matrix
    std::vector<glm::mat4> captureViews; // vector array of View Matrix (for 6 cube faces)
    std::vector<glm::mat4> cameraMatrix; // Camera Matrix (View x Projection Matrix)

    std::vector<glm::vec3> skyboxVertices = {
        // positions          
        glm::vec3(1.0f,  1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
    };
    std::vector<GLuint> skyboxIndices = {
        0, 1, 2,
        2, 3, 0,

        0, 3, 4,
        3, 4, 7,

        2, 3, 6,
        3, 6, 7,

        0, 1, 5,
        0, 4, 5,

        1, 2, 5,
        2, 5, 6,

        4, 5, 6,
        4, 6, 7
    };
    Mesh* skymap;

	GLuint GenerateCubemap(GLuint &hdrTexture, Shader& shader);
	GLuint GenerateIrradianceMap(GLuint &cubemapTexture, Shader& irradianceShader);
	GLuint GeneratePrefilteredCubemap(GLuint &cubemapTexture, Shader& prefilterShader, GLuint maxMipLevels);
	GLuint GenerateBRDFLUT(Shader &brdfShader);
};
#endif