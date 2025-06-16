#include "Mesh.h"
#include "TextureUtilities.h"
#include <random>

using namespace std;

const int WIDTH = 1920, HEIGHT = 1080;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "1M spheres", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	
    glViewport(0, 0, WIDTH, HEIGHT);

	//************************************************-------------SkyMap----------*****************************************************

	Shader skymapShader("./skymap.vert", "./skymap.frag", "skybox");
	skymapShader.Activate();

	std::vector<glm::vec3> skyboxVertices = {
		// positions (setting x as +/- 500.0, want to make it a little cuboid)   
		glm::vec3(500.0f,	300.0f,	  300.0f),
		glm::vec3(500.0f,	-300.0f,  300.0f),
		glm::vec3(-500.0f,	-300.0f,  300.0f),
		glm::vec3(-500.0f,	300.0f,   300.0f),
		glm::vec3(500.0f,	300.0f,	  -300.0f),
		glm::vec3(500.0f,	-300.0f,  -300.0f),
		glm::vec3(-500.0f,	-300.0f,  -300.0f),
		glm::vec3(-500.0f,	300.0f,	  -300.0f),
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

	Mesh skymap(skyboxVertices, skyboxIndices);

	//************************************************-------------Camera----------**************************************************888
	
	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 10.0f));
	camera.updateMatrix();

	// ----------------------------------------------------------------Spheres-------------------------------------------------------------

	Shader sphereShader("./sphere.vert", "./sphere_pbr.frag", "PBR");
	sphereShader.Activate();
	
	// for random number generation
	std::random_device rd; 
	std::mt19937 gen(rd());

	std::uniform_real_distribution<> dis(0.0, 1.0); // between 0 an 1 (for color and material props)
	std::uniform_real_distribution<> dis2(-150.0, 150.0); // between -150 and 150 (for positions)
	
	std::vector<glm::vec3> highDetailVertices;
	std::vector<GLuint> highDetailIndices;

	std::vector<glm::vec3> mediumDetailVertices;
	std::vector<GLuint> mediumDetailIndices;

	std::vector<glm::vec3> lowDetailVertices;
	std::vector<GLuint> lowDetailIndices;

	const int highDetailLatSegments = 30;
	const int highDetailLonSegments = 30;

	const int mediumDetailLatSegments = 25;
	const int mediumDetailLonSegments = 25;

	const int lowDetailLatSegments = 5;
	const int lowDetailLonSegments = 5;
	const float radius = 0.5f;

	// high LOD vertices
	for (int i = 0; i <= highDetailLatSegments; ++i) {
		float theta = i * 3.14159265359f / highDetailLatSegments; // Polar angle (0 to π)
		for (int j = 0; j <= highDetailLonSegments; ++j) {
			float phi = j * 2.0f * 3.14159265359f / highDetailLonSegments; // Azimuthal angle (0 to 2π)
			
			float x = (radius)*sin(theta) * cos(phi);
			float y = (radius)*sin(theta) * sin(phi);
			float z = (radius)*cos(theta);

			float nx = (x) / radius;
			float ny = (y) / radius;
			float nz = (z) / radius;
			
			highDetailVertices.push_back(glm::vec3(x, y, z));
		}
	}
	// high LOD indices
	for (int i = 0; i < highDetailLatSegments; ++i) {
		for (int j = 0; j < highDetailLonSegments; ++j) {
			// Calculate indices for two triangles per grid cell
			unsigned int first = (i * (highDetailLonSegments + 1)) + j;
			unsigned int second = first + highDetailLonSegments + 1;

			highDetailIndices.push_back(first);
			highDetailIndices.push_back(second);
			highDetailIndices.push_back(first + 1);

			highDetailIndices.push_back(second);
			highDetailIndices.push_back(second + 1);
			highDetailIndices.push_back(first + 1);
		}
	}

	// medium LOD vertices
	for (int i = 0; i <= mediumDetailLatSegments; ++i) {
		float theta = i * 3.14159265359f / mediumDetailLatSegments; // Polar angle (0 to π)
		for (int j = 0; j <= mediumDetailLonSegments; ++j) {
			float phi = j * 2.0f * 3.14159265359f / mediumDetailLonSegments; // Azimuthal angle (0 to 2π)

			float x = (radius)*sin(theta) * cos(phi);
			float y = (radius)*sin(theta) * sin(phi);
			float z = (radius)*cos(theta);

			float nx = (x) / radius;
			float ny = (y) / radius;
			float nz = (z) / radius;

			mediumDetailVertices.push_back(glm::vec3(x, y, z));
		}
	}
	// medium LOD indices
	for (int i = 0; i < mediumDetailLatSegments; ++i) {
		for (int j = 0; j < mediumDetailLonSegments; ++j) {
			// Calculate indices for two triangles per grid cell
			unsigned int first = (i * (mediumDetailLonSegments + 1)) + j;
			unsigned int second = first + mediumDetailLonSegments + 1;

			mediumDetailIndices.push_back(first);
			mediumDetailIndices.push_back(second);
			mediumDetailIndices.push_back(first + 1);

			mediumDetailIndices.push_back(second);
			mediumDetailIndices.push_back(second + 1);
			mediumDetailIndices.push_back(first + 1);
		}
	}

	for (int i = 0; i <= lowDetailLatSegments; ++i) {
		float theta = i * 3.14159265359f / lowDetailLatSegments; // Polar angle (0 to π)
		for (int j = 0; j <= lowDetailLonSegments; ++j) {
			float phi = j * 2.0f * 3.14159265359f / lowDetailLonSegments; // Azimuthal angle (0 to 2π)

			float x = (radius)*sin(theta) * cos(phi);
			float y = (radius)*sin(theta) * sin(phi);
			float z = (radius)*cos(theta);

			float nx = (x) / radius;
			float ny = (y) / radius;
			float nz = (z) / radius;

			lowDetailVertices.push_back(glm::vec3(x, y, z));
		}
	}
	for (int i = 0; i < lowDetailLatSegments; ++i) {
		for (int j = 0; j < lowDetailLonSegments; ++j) {
			// Calculate indices for two triangles per grid cell
			unsigned int first = (i * (lowDetailLonSegments + 1)) + j;
			unsigned int second = first + lowDetailLonSegments + 1;

			lowDetailIndices.push_back(first);
			lowDetailIndices.push_back(second);
			lowDetailIndices.push_back(first + 1);

			lowDetailIndices.push_back(second);
			lowDetailIndices.push_back(second + 1);
			lowDetailIndices.push_back(first + 1);
		}
	}

	int numInstances = 1000000;

	std::vector<glm::vec3>instancePositions;
	std::vector<glm::vec3>instanceColors;
	std::vector<glm::vec2>instanceMaterialProperties;

	for (int i = 0; i < numInstances; ++i) {
		glm::vec3 position(dis2(gen), dis2(gen), dis2(gen)); // xyz
		glm::vec3 color(dis(gen), dis(gen), dis(gen)); // RGB
		glm::vec2 materialProperty(dis(gen), dis(gen)); // metalness , roughness

		instancePositions.push_back(position);
		instanceColors.push_back(color);
		instanceMaterialProperties.push_back(materialProperty);
	}

	Mesh baseSphere(highDetailVertices, highDetailIndices, mediumDetailVertices, mediumDetailIndices,
		lowDetailVertices, lowDetailIndices, instancePositions, instanceColors, instanceMaterialProperties);
	
	glm::vec3 spherePos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 sphereModel = glm::mat4(1.0f);
	sphereModel = glm::translate(sphereModel, spherePos);

	glUniformMatrix4fv(glGetUniformLocation(sphereShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(sphereModel));

//-----------------------------------GPU compute shader for LOD and Culling--------------------------------
	Shader cullingLODShader("./cullingLOD.comp", "Compute Shader");
	cullingLODShader.Activate();

//--------------------------------------******TEXTURES*********-----------------------------------------
	//vector<std::string> faces
	//{
	//	"./textures/skybox/right.jpg",
	//	"./textures/skybox/left.jpg",
	//	"./textures/skybox/top.jpg",
	//	"./textures/skybox/bottom.jpg",
	//	"./textures/skybox/front.jpg",
	//	"./textures/skybox/back.jpg",
	//};

	//Texture cubemapFaceTexture(faces); // skymap with 6 images of respected cube faces

	//--------------------------HDR Texture setup-----------------------------------
	Shader cubemapShader("./cubemapShader.vert", "./cubemapShader.frag", "cubemap");
	cubemapShader.Activate();
	Shader irradianceShader("./irradianceShader.vert", "./irradianceShader.frag", "irradiancemap");
	irradianceShader.Activate();
	Shader prefilterShader("./prefilterShader.vert", "./prefilterShader.frag", "prefiltermap");
	prefilterShader.Activate();
	Shader brdfShader("./brdfShader.vert", "./brdfShader.frag", "brdfMap");
	brdfShader.Activate();

	Texture hdrTexture("./textures/night.hdr", GL_TEXTURE0, true);
	
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	TextureUtilities textureUtilitiesObject;

	// Generate cubemap
	GLuint cubemapTexture = textureUtilitiesObject.GenerateCubemap(hdrTexture.ID, cubemapShader);
	glViewport(0, 0, WIDTH, HEIGHT); // resetting viewport

	// Generate irradiance map
	GLuint irradianceMap = textureUtilitiesObject.GenerateIrradianceMap(cubemapTexture, irradianceShader);
	glViewport(0, 0, WIDTH, HEIGHT);

	// Generate prefiltered environment map
	GLuint prefilteredEnvMap = textureUtilitiesObject.GeneratePrefilteredCubemap(cubemapTexture, prefilterShader, 5);
	glViewport(0, 0, WIDTH, HEIGHT);

	// Generate BRDF Look up Texture
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	GLuint brdfLUTTexture = textureUtilitiesObject.GenerateBRDFLUT(brdfShader);
	glViewport(0, 0, WIDTH, HEIGHT); // resetting viewport

	sphereShader.Activate();
	glUniform1i(glGetUniformLocation(sphereShader.ID, "skybox"), 0);

	skymapShader.Activate();
	glUniform1i(glGetUniformLocation(skymapShader.ID, "skybox"), 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_CULL_FACE);

	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff = 0.0;
	unsigned int counter = 0;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// baseSphere.initializeInstanceVBOs(camera); // for initializing instance VBOs (not needed with GPU culling)

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
			// exit with ESC
            glfwSetWindowShouldClose(window, true);
        }
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >= 1.0 / 30.0)
		{
			// Create new title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "1 Million spheres - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;
		}
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix();

		//baseSphere.UpdateChunkLODs(camera);
		
		baseSphere.gpuCullingLOD(cullingLODShader, camera); // calling GPU based culling and LOD

		sphereShader.Activate();
		glUniform1f(glGetUniformLocation(sphereShader.ID, "time"), crntTime);
		//std::cout << crntTime << "\n";
		baseSphere.Draw(sphereShader, camera,
			irradianceMap, prefilteredEnvMap, brdfLUTTexture);

		glDepthFunc(GL_LEQUAL);
		skymap.Draw(skymapShader, camera, cubemapTexture);
		glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL Error: " << err << std::endl;
		}
    }
	sphereShader.Delete();
	hdrTexture.Delete();
	glDeleteTextures(1, &cubemapTexture);
	glDeleteTextures(1, &irradianceMap);
	glDeleteTextures(1, &prefilteredEnvMap);
	glDeleteTextures(1, &brdfLUTTexture);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}