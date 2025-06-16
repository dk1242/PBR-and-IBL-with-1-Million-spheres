#include "Mesh.h"
#include "glm/gtx/string_cast.hpp"
#include <omp.h>

Mesh::Mesh(std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices)
{
    Mesh::vertices = vertices;
    Mesh::indices = indices;

    mainVAO.Bind();
    VBO mainVBO(vertices);
    EBO mainEBO(indices);
    mainVAO.LinkAttrib(mainVBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)(0));
    mainVAO.Unbind();

    mainVBO.Unbind();
    mainEBO.Unbind();
}

void Mesh::initializeSSBOs(std::vector<glm::vec3>& instancePositions, std::vector<glm::vec3>& instanceColors, std::vector<glm::vec2>& instanceMaterialProperties) {
    // for actually storing the data on GPU
    instancePosBuffer = new SSBO(instancePositions, "normal");
    instanceColorBuffer = new SSBO(instanceColors, "normal");
    instanceMaterialPropsBuffer = new SSBO(instanceMaterialProperties, "normal");

    // just for allocating the space on GPU
    highPosBuffer = new SSBO(instancePositions, "justForSpace");
    highColorBuffer = new SSBO(instanceColors, "justForSpace");
    highMaterialPropsBuffer = new SSBO(instanceMaterialProperties, "justForSpace");

    medPosBuffer = new SSBO(instancePositions, "justForSpace");
    medColorBuffer = new SSBO(instanceColors, "justForSpace");
    medMaterialPropsBuffer = new SSBO(instanceMaterialProperties, "justForSpace");

    lowPosBuffer = new SSBO(instancePositions, "justForSpace");
    lowColorBuffer = new SSBO(instanceColors, "justForSpace");
    lowMaterialPropsBuffer = new SSBO(instanceMaterialProperties, "justForSpace");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    // counter buffer
    highLodCounterBuffer = new ACBO();
    medLodCounterBuffer = new ACBO();
    lowLodCounterBuffer = new ACBO();

    //glGenBuffers(1, &debugBuffer);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, debugBuffer);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 3, nullptr, GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Mesh::Mesh(std::vector<glm::vec3>& highVertices, std::vector<GLuint>& highIndices,
    std::vector<glm::vec3>& medVertices, std::vector<GLuint>& medIndices, 
    std::vector<glm::vec3>& lowVertices, std::vector<GLuint>& lowIndices,
    std::vector<glm::vec3>& instancePositions, std::vector<glm::vec3>& instanceColors, std::vector<glm::vec2>& instanceMaterialProperties)
{
    initializeSSBOs(instancePositions, instanceColors, instanceMaterialProperties);

    vertices.resize(0);
    indices.resize(0);

	highDetailVertices = highVertices;
	highDetailIndices = highIndices;

	mediumDetailVertices = medVertices;
	mediumDetailIndices = medIndices;

	lowDetailVertices = lowVertices;
	lowDetailIndices = lowIndices;

	this->instancePositions = instancePositions;
	Mesh::instanceColors = instanceColors;
    Mesh::instanceMaterialProperties = instanceMaterialProperties;

    chunkSize = instancePositions.size() / numChunks;

    highDetailVAO.Bind();
    VBO highVBO(highVertices);
    EBO highEBO(highIndices);
    highDetailVAO.LinkAttrib(highVBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)(0));
    highDetailVAO.Unbind();
    highVBO.Unbind();
    highVBO.Unbind();

	mediumDetailVAO.Bind();
	VBO mediumVBO(highVertices);
	EBO mediumEBO(highIndices);
	mediumDetailVAO.LinkAttrib(mediumVBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)(0));
	mediumDetailVAO.Unbind();
    mediumVBO.Unbind();
    mediumVBO.Unbind();

	lowDetailVAO.Bind();
	VBO lowVBO(highVertices);
	EBO lowEBO(highIndices);
	lowDetailVAO.LinkAttrib(lowVBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)(0));
	lowDetailVAO.Unbind();
    lowVBO.Unbind();
    lowVBO.Unbind();
}

static bool isInside(const glm::vec3& position, Camera& camera) {
    // for frustum culling
    return position.x >= camera.left && position.x <= camera.right &&
        position.y >= camera.bottom && position.y <= camera.top &&
        position.z <= camera.near && position.z >= camera.far;
}

void Mesh::initializeInstanceVBOs(Camera &camera) {
    for (int i = 0; i < instancePositions.size(); ++i) {
        // Perform frustum culling
        if (isInside(instancePositions[i], camera)) {
            float distanceSquared = glm::distance2(camera.Position, instancePositions[i]);

            // LOD selection based on distance
            if (distanceSquared < 2500.0f) { // High detail
                highDetailInstancePositions.push_back(instancePositions[i]);
                highDetailInstanceColors.push_back(instanceColors[i]);
                highDetailInstanceMaterialProperties.push_back(instanceMaterialProperties[i]);
            }
            else if (distanceSquared < 10000.0f) { // Medium detail
                mediumDetailInstancePositions.push_back(instancePositions[i]);
                mediumDetailInstanceColors.push_back(instanceColors[i]);
                mediumDetailInstanceMaterialProperties.push_back(instanceMaterialProperties[i]);
            }
            else { // Low detail
                lowDetailInstancePositions.push_back(instancePositions[i]);
                lowDetailInstanceColors.push_back(instanceColors[i]);
                lowDetailInstanceMaterialProperties.push_back(instanceMaterialProperties[i]);
            }
        }
    }
    
    highPosVBO = new InstanceVBO(highDetailInstancePositions);
    highColorVBO = new InstanceVBO(highDetailInstanceColors);
    highMaterialVBO = new InstanceVBO(highDetailInstanceMaterialProperties);

    medPosVBO = new InstanceVBO(mediumDetailInstancePositions);
    medColorVBO = new InstanceVBO(mediumDetailInstanceColors);
    medMaterialVBO = new InstanceVBO(mediumDetailInstanceMaterialProperties);

    lowPosVBO = new InstanceVBO(lowDetailInstancePositions);
    lowColorVBO = new InstanceVBO(lowDetailInstanceColors);
    lowMaterialVBO = new InstanceVBO(lowDetailInstanceMaterialProperties);

    //highDetailInstancePositions.resize(0);
    //mediumDetailInstancePositions.resize(0);
    //lowDetailInstancePositions.resize(0);

    //highDetailInstanceColors.resize(0);
    //mediumDetailInstanceColors.resize(0);
    //lowDetailInstanceColors.resize(0);

    //highDetailInstanceMaterialProperties.resize(0);
    //mediumDetailInstanceMaterialProperties.resize(0);
    //lowDetailInstanceMaterialProperties.resize(0);
}

void Mesh::AggregateBuffers()
{
    highDetailInstancePositions = std::move(highDetailStagingPositions);
    mediumDetailInstancePositions = std::move(mediumDetailStagingPositions);
    lowDetailInstancePositions = std::move(lowDetailStagingPositions);

    highDetailInstanceColors = std::move(highDetailStagingColors);
    mediumDetailInstanceColors = std::move(mediumDetailStagingColors);
    lowDetailInstanceColors = std::move(lowDetailStagingColors);

    highDetailInstanceMaterialProperties = std::move(highDetailStagingMaterialProperties);
    mediumDetailInstanceMaterialProperties = std::move(mediumDetailStagingMaterialProperties);
    lowDetailInstanceMaterialProperties = std::move(lowDetailStagingMaterialProperties);

    // Clear staging buffers for the next cycle
    highDetailStagingPositions.resize(0);
    mediumDetailStagingPositions.resize(0);
    lowDetailStagingPositions.resize(0);

    highDetailStagingColors.resize(0);
    mediumDetailStagingColors.resize(0);
    lowDetailStagingColors.resize(0);

    highDetailStagingMaterialProperties.resize(0);
    mediumDetailStagingMaterialProperties.resize(0);
    lowDetailStagingMaterialProperties.resize(0);

    //std::cout << "Aggregate buffers:\n" << "highDetailInstancePositions size " << highDetailInstancePositions.size() << "\n" <<
    //    "mediumDetailInstancePositions size " << mediumDetailInstancePositions.size() << "\n" <<
    //    "lowDetailInstancePositions size " << lowDetailInstancePositions.size() << "\n";
}

void Mesh::UpdateChunkLODs(Camera& camera)
{
    omp_set_num_threads(4); // setting number of threads to 4
    // Determine the range of the current chunk
    size_t start = currentChunk * chunkSize;
    size_t end = std::min(start + chunkSize, instancePositions.size());
    glm::vec3 cameraPos = camera.Position;
    // Thread-local buffers
    std::vector<glm::vec3> highDetailLocalPositions, mediumDetailLocalPositions, lowDetailLocalPositions;
    std::vector<glm::vec3> highDetailLocalColors, mediumDetailLocalColors, lowDetailLocalColors;
    std::vector<glm::vec2> highDetailLocalMaterialProperties, mediumDetailLocalMaterialProperties, lowDetailLocalMaterialProperties;

    // Parallel loop for processing the chunk
#pragma omp parallel
    {
        // Private thread-local buffers
        std::vector<glm::vec3> threadHighDetailPositions, threadMediumDetailPositions, threadLowDetailPositions;
        std::vector<glm::vec3> threadHighDetailColors, threadMediumDetailColors, threadLowDetailColors;
        std::vector<glm::vec2> threadHighDetailMaterialProperties, threadMediumDetailMaterialProperties, threadLowDetailMaterialProperties;

#pragma omp for schedule(static)
        for (int i = start; i < end; ++i) {
            // Perform frustum culling
            if (isInside(instancePositions[i], camera)) {
                float distanceSquared = glm::distance2(cameraPos, instancePositions[i]);

                // LOD selection based on distance
                if (distanceSquared < 2500.0f) { // High detail
                    threadHighDetailPositions.push_back(instancePositions[i]);
                    threadHighDetailColors.push_back(instanceColors[i]);
                    threadHighDetailMaterialProperties.push_back(instanceMaterialProperties[i]);
                }
                else if (distanceSquared < 10000.0f) { // Medium detail
                    threadMediumDetailPositions.push_back(instancePositions[i]);
                    threadMediumDetailColors.push_back(instanceColors[i]);
                    threadMediumDetailMaterialProperties.push_back(instanceMaterialProperties[i]);
                }
                else { // Low detail
                    threadLowDetailPositions.push_back(instancePositions[i]);
                    threadLowDetailColors.push_back(instanceColors[i]);
                    threadLowDetailMaterialProperties.push_back(instanceMaterialProperties[i]);
                }
            }
        }

        // Merge thread-local buffers into shared buffers
#pragma omp critical
        {
            highDetailLocalPositions.insert(highDetailLocalPositions.end(), threadHighDetailPositions.begin(), threadHighDetailPositions.end());
            highDetailLocalColors.insert(highDetailLocalColors.end(), threadHighDetailColors.begin(), threadHighDetailColors.end());
            highDetailLocalMaterialProperties.insert(highDetailLocalMaterialProperties.end(), threadHighDetailMaterialProperties.begin(), threadHighDetailMaterialProperties.end());

            mediumDetailLocalPositions.insert(mediumDetailLocalPositions.end(), threadMediumDetailPositions.begin(), threadMediumDetailPositions.end());
            mediumDetailLocalColors.insert(mediumDetailLocalColors.end(), threadMediumDetailColors.begin(), threadMediumDetailColors.end());
            mediumDetailLocalMaterialProperties.insert(mediumDetailLocalMaterialProperties.end(), threadMediumDetailMaterialProperties.begin(), threadMediumDetailMaterialProperties.end());

            lowDetailLocalPositions.insert(lowDetailLocalPositions.end(), threadLowDetailPositions.begin(), threadLowDetailPositions.end());
            lowDetailLocalColors.insert(lowDetailLocalColors.end(), threadLowDetailColors.begin(), threadLowDetailColors.end());
            lowDetailLocalMaterialProperties.insert(lowDetailLocalMaterialProperties.end(), threadLowDetailMaterialProperties.begin(), threadLowDetailMaterialProperties.end());
        }
    }

    // Move thread-local buffers to staging buffers
    highDetailStagingPositions.insert(highDetailStagingPositions.end(), highDetailLocalPositions.begin(), highDetailLocalPositions.end());
    mediumDetailStagingPositions.insert(mediumDetailStagingPositions.end(), mediumDetailLocalPositions.begin(), mediumDetailLocalPositions.end());
    lowDetailStagingPositions.insert(lowDetailStagingPositions.end(), lowDetailLocalPositions.begin(), lowDetailLocalPositions.end());

    highDetailStagingColors.insert(highDetailStagingColors.end(), highDetailLocalColors.begin(), highDetailLocalColors.end());
    mediumDetailStagingColors.insert(mediumDetailStagingColors.end(), mediumDetailLocalColors.begin(), mediumDetailLocalColors.end());
    lowDetailStagingColors.insert(lowDetailStagingColors.end(), lowDetailLocalColors.begin(), lowDetailLocalColors.end());

    highDetailStagingMaterialProperties.insert(highDetailStagingMaterialProperties.end(), highDetailLocalMaterialProperties.begin(), highDetailLocalMaterialProperties.end());
    mediumDetailStagingMaterialProperties.insert(mediumDetailStagingMaterialProperties.end(), mediumDetailLocalMaterialProperties.begin(), mediumDetailLocalMaterialProperties.end());
    lowDetailStagingMaterialProperties.insert(lowDetailStagingMaterialProperties.end(), lowDetailLocalMaterialProperties.begin(), lowDetailLocalMaterialProperties.end());

    // Move to the next chunk
    currentChunk = (currentChunk + 1) % numChunks; // Wrap around when reaching the last chunk

    // If all chunks are processed, aggregate staging buffers into main buffers
    if (currentChunk == 0) {
        this->AggregateBuffers();
    }
}

void Mesh::gpuCullingLOD(Shader& computeShader, Camera& camera)
{
// for every frame
    // reset all counters to 0
    highLodCounterBuffer->Reset();
    medLodCounterBuffer->Reset();
    lowLodCounterBuffer->Reset();

    // clean all LOD buffers
    highPosBuffer->Clean();
    medPosBuffer->Clean();
    lowPosBuffer->Clean();

    computeShader.Activate();
    // Bind all SSBOs and ACBOs
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instancePosBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instanceColorBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, instanceMaterialPropsBuffer->ID);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, highPosBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, highColorBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, highMaterialPropsBuffer->ID);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, medPosBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, medColorBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, medMaterialPropsBuffer->ID);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, lowPosBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, lowColorBuffer->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, lowMaterialPropsBuffer->ID);

    // atomic counters
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, highLodCounterBuffer->ID);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, medLodCounterBuffer->ID);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, lowLodCounterBuffer->ID);

    // setting other uniforms
    glUniform3f(glGetUniformLocation(computeShader.ID, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
    glUniform1f(glGetUniformLocation(computeShader.ID, "left"), camera.left);
    glUniform1f(glGetUniformLocation(computeShader.ID, "right"), camera.right);
    glUniform1f(glGetUniformLocation(computeShader.ID, "bottom"), camera.bottom);
    glUniform1f(glGetUniformLocation(computeShader.ID, "top"), camera.top);
    glUniform1f(glGetUniformLocation(computeShader.ID, "near"), camera.near);
    glUniform1f(glGetUniformLocation(computeShader.ID, "far"), camera.far);
    glUniform1ui(glGetUniformLocation(computeShader.ID, "totalInstances"), instancePositions.size());

    // launching compute shader with work groups based on the instancePositions size
    glDispatchCompute((instancePositions.size() + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT); // for synchronization
    glFinish(); // blocks until all GPU operations are complete

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, debugBuffer);
    //float* mappedData = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    //printf("Data: (%f, %f, %f)\n", mappedData[0], mappedData[1], mappedData[2]);
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //printf("Camera Position: (%f, %f, %f)\n", camera.Position.x, camera.Position.y, camera.Position.z);
    //printf("Bounding Box: left=%f, right=%f, bottom=%f, top=%f, near=%f, far=%f\n",
    //    camera.left, camera.right, camera.bottom, camera.top, camera.near, camera.far);
    //glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, highLodCounterBuffer);
    //glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &highLodCount);
    //std::cout << "High LOD Count: " << highLodCount << std::endl;

    //glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, medLodCounterBuffer);
    //glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &medLodCount);
    //std::cout << "Medium LOD Count: " << medLodCount << std::endl;

    //glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, lowLodCounterBuffer);
    //glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &lowLodCount);
    //std::cout << "Low LOD Count: " << lowLodCount << std::endl;
}

void Mesh::Draw(Shader& shader) const
{
    mainVAO.Bind();

    glDrawElements(GL_TRIANGLES, GLsizei(indices.size()), GL_UNSIGNED_INT, 0);
}

void Mesh::Draw(Shader& shader, Camera& camera, GLuint cubemapFaceTexture) const {
    shader.Activate();
    mainVAO.Bind();

    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camMatrix");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapFaceTexture);
    glUniform1i(glGetUniformLocation(shader.ID, "skybox"), 0);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::Draw(Shader& shader, Camera& camera,
    GLuint irradianceMap, GLuint prefilteredEnvMap, GLuint brdfLUTTexture)
{
    shader.Activate();

    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camMatrix");
    
    if (shader.shaderName == "PBR") {
        // Bind IBL textures (irradiance map, prefiltered environment map, BRDF LUT)
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glUniform1i(glGetUniformLocation(shader.ID, "irradianceMap"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredEnvMap);
        glUniform1i(glGetUniformLocation(shader.ID, "prefilteredEnvMap"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glUniform1i(glGetUniformLocation(shader.ID, "brdfLUT"), 3);
    }

    // getting and storing LOD count in respective LOD counters
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, highLodCounterBuffer->ID);
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &highLodCount);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, medLodCounterBuffer->ID);
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &medLodCount);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, lowLodCounterBuffer->ID);
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &lowLodCount);

    //std::cout << "High LOD: " << highLodCount << ", Medium LOD: " << medLodCount << ", Low LOD: " << lowLodCount << std::endl;
    
    if (highLodCount > 0) {
        highDetailVAO.Bind();
        
        glUniform1f(glGetUniformLocation(shader.ID, "scaleFactor"), 0.9);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, highPosBuffer->ID); // Bind high LOD positions
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, highColorBuffer->ID);    // Bind high LOD colors
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, highMaterialPropsBuffer->ID); // Bind high LOD material properties

        glDrawElementsInstanced(GL_TRIANGLES, highDetailIndices.size(), GL_UNSIGNED_INT, 0, highLodCount);
    }

    // Render medium-detail instances
    if (medLodCount > 0) {
        mediumDetailVAO.Bind();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, medPosBuffer->ID); // Bind Medium LOD Positions ...
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, medColorBuffer->ID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, medMaterialPropsBuffer->ID);

        glUniform1f(glGetUniformLocation(shader.ID, "scaleFactor"), 0.95);

        glDrawElementsInstanced(GL_TRIANGLES, mediumDetailIndices.size(), GL_UNSIGNED_INT, 0, medLodCount);
    }

    // Render low-detail instances
    if (lowLodCount > 0) {
        lowDetailVAO.Bind();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lowPosBuffer->ID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, lowColorBuffer->ID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lowMaterialPropsBuffer->ID);

        glUniform1f(glGetUniformLocation(shader.ID, "scaleFactor"), 0.95);

        glDrawElementsInstanced(GL_TRIANGLES, lowDetailIndices.size(), GL_UNSIGNED_INT, 0, lowLodCount);
    }
}