# PBR and IBL Rendering for 1 Million Spheres at 60 FPS

## Overview
I implemented Physically Based Rendering (PBR) and Image-Based Lighting (IBL) to render 1 million spheres while maintaining a smooth frame rate of 60 FPS.

## Features
- **Physically Based Rendering (PBR):**
  - Realistic lighting and shading using standard physically accurate models.
  - Includes metallic and roughness properties too.

- **Image-Based Lighting (IBL):**
  - Used HDR environment maps for realistic light reflections and ambient environment lighting.
  - Precomputed diffuse irradiance and specular reflections and stored in separate maps.

- **Performance Optimization:**
  - Used Frustum culling to remove the invisible spheres from processing.
  - Implemented different Level of Detail selection based on distance from camera.
  - Used compute shader to run these culling and LOD selection calculation in GPU. 
  - Parallelized rendering pipeline using GPU acceleration.
  - Efficient instancing and batching techniques to handle large numbers of spheres.
  - Reduced CPU-GPU communication as less as possible.
  - Vertex shader reading the computed data buffer (from compute Shader) stored in GPU. 

## How It Works
1. **Sphere Instancing:**
   - All spheres are rendered using GPU instancing, significantly reducing draw calls.
2. **Material Properties:**
   - Each sphere can have unique material properties (metallic, roughness, albedo (color)).
3. **HDR Environment Maps:**
   - A single environment map provides global lighting and reflections.
4. **Dynamic Optimization:**
   - Frustum Culling and LOD (Level of Detail) techniques are employed to improve performance.

## Requirements
- **Hardware:**
  - Modern GPU supporting OpenGl 4.3.
  - Decent RAM.
- **Software:**
  - OpenGL 4.3 compatible rendering backend.
  - C++ environment with necessary dependencies like `glad, GLFW, glm, stb`.

## Installation
1. Clone the repository:
    ```bash
    git clone https://github.com/dk1242/PBR-and-IBL-with-1-Million-spheres.git
    ```
2. Open Visual Studio:
    - Launch Visual Studio and select **Open a Local Folder**.
    - Navigate to the cloned repository folder and open it.

3. Configure the project:
    - Ensure that you have the necessary dependencies linked (or first build it by following [this](https://learnopengl.com/Getting-started/Creating-a-window#:~:text=most%20other%20IDEs.-,Building%20GLFW,-GLFW%20can%20be)).
    
4. Build the project:
    - Click on **Build** > **Build Solution** (or press `Ctrl+Shift+B`).
    - This will compile the project and generate the executable.

5. Run the project:
    - After a successful build, press `F5` to run the application within Visual Studio.
    - Alternatively, locate the generated executable in the `Release` or `Debug` folder and run it directly.
    
## Usage
- Launch the application and observe 1 million spheres rendered with PBR and IBL techniques.
- You can move in the scene using `W`, `A`, `S` and `D`.

## Acknowledgments

I learned and cleared my basics from [learnopengl.com](https://learnopengl.com/) and [FreeCodeCamp video](https://youtu.be/45MIykWJ-C4?si=uBAfGH4kaYC-CwCn) on openGL. 
For PBR and IBL too, I learned it from [learnopengl.com](https://learnopengl.com/) and understood it completely with chatgpt.com .

---
