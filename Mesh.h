#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"SSBO.h"
#include"ACBO.h"
#include "InstanceVBO.h"

#include <vector>
#include "Camera.h"
#include "Texture.h"

class Mesh {
public:
    //GLuint debugBuffer;

    SSBO* instancePosBuffer;
    SSBO* instanceColorBuffer;
    SSBO* instanceMaterialPropsBuffer;

    SSBO* highPosBuffer;
    SSBO* highColorBuffer;
    SSBO* highMaterialPropsBuffer;

    SSBO* medPosBuffer;
    SSBO* medColorBuffer;
    SSBO* medMaterialPropsBuffer;

    SSBO* lowPosBuffer;
    SSBO* lowColorBuffer;
    SSBO* lowMaterialPropsBuffer;

    ACBO* highLodCounterBuffer;
    ACBO* medLodCounterBuffer;
    ACBO* lowLodCounterBuffer;

    GLuint visibleInstanceCount;

    GLuint highLodCount, medLodCount, lowLodCount;

    std::vector <glm::vec3> vertices;
    std::vector <GLuint> indices;

    std::vector <glm::vec3> highDetailVertices;
    std::vector <GLuint> highDetailIndices;

    std::vector <glm::vec3> mediumDetailVertices;
    std::vector <GLuint> mediumDetailIndices;

    std::vector <glm::vec3> lowDetailVertices;
    std::vector <GLuint> lowDetailIndices;

    std::vector <glm::vec3> highDetailInstancePositions;
    std::vector <glm::vec3> mediumDetailInstancePositions;
    std::vector <glm::vec3> lowDetailInstancePositions;

    std::vector <glm::vec3> highDetailInstanceColors;
    std::vector <glm::vec3> mediumDetailInstanceColors;
    std::vector <glm::vec3> lowDetailInstanceColors;

    std::vector <glm::vec2> highDetailInstanceMaterialProperties;
    std::vector <glm::vec2> mediumDetailInstanceMaterialProperties;
    std::vector <glm::vec2> lowDetailInstanceMaterialProperties;

	std::vector <glm::vec3> instancePositions;
	std::vector <glm::vec3> instanceColors;
    std::vector <glm::vec2> instanceMaterialProperties;

    const size_t numChunks = 5; 
    size_t currentChunk = 0;
    size_t chunkSize = 1; // just for initialization

    // Temporary staging buffers for incremental updates
    std::vector<glm::vec3> highDetailStagingPositions;
    std::vector<glm::vec3> mediumDetailStagingPositions;
    std::vector<glm::vec3> lowDetailStagingPositions;

    std::vector<glm::vec3> highDetailStagingColors;
    std::vector<glm::vec3> mediumDetailStagingColors;
    std::vector<glm::vec3> lowDetailStagingColors;

    std::vector<glm::vec2> highDetailStagingMaterialProperties;
    std::vector<glm::vec2> mediumDetailStagingMaterialProperties;
    std::vector<glm::vec2> lowDetailStagingMaterialProperties;

    VAO mainVAO;
	VAO highDetailVAO, mediumDetailVAO, lowDetailVAO;

    InstanceVBO * highPosVBO, * highColorVBO, * highMaterialVBO,
                * medPosVBO, * medColorVBO, * medMaterialVBO,
                * lowPosVBO, * lowColorVBO, * lowMaterialVBO;

    Mesh(std::vector <glm::vec3>& vertices, std::vector <GLuint>& indices);
    Mesh(std::vector <glm::vec3>& highVertices, std::vector <GLuint>& highIndices,
        std::vector <glm::vec3>& medVertices, std::vector <GLuint>& medIndices,
        std::vector <glm::vec3>& lowVertices, std::vector <GLuint>& lowIndices,
        std::vector<glm::vec3>& instancePositions, std::vector<glm::vec3>& instanceColors, std::vector<glm::vec2>& instanceMaterialProperties);

    // Normal chunk based calculation related functions
    void initializeInstanceVBOs(Camera& camera);
    void AggregateBuffers();
    void UpdateChunkLODs(Camera& camera);

    // GPU culling related functions
    void initializeSSBOs(std::vector<glm::vec3>& instancePositions, std::vector<glm::vec3>& instanceColors, std::vector<glm::vec2>& instanceMaterialProperties);
	void gpuCullingLOD(Shader& computeShader, Camera& camera);

    void Draw(Shader& shader) const;
    void Draw(Shader& shader, Camera& camera, GLuint cubemapFaceTexture) const; // for skybox
    void Draw(Shader& shader, Camera& camera,
        GLuint irradianceMap, GLuint prefilteredEnvMap, GLuint brdfLUTTexture); // for spheres
};


#endif