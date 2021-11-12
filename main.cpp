#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "headers/shaderClass.h"
#include "headers/computeShaderClass.h"
#include "headers/VAO.h"
#include "headers/VBO.h"
#include "headers/IBO.h"
#include "headers/VBO_layout.h"
#include "headers/renderer.h"
#include "headers/texture.h"
#include "headers/debugging.h"

#define AGENT_N 100000

void processInput(GLFWwindow* window, float& zoom, float& offsetX, float& offsetY) {
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { zoom *= 0.99f; }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { zoom /= 0.99f; }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { offsetX -= 0.01f * zoom; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { offsetX += 0.01f * zoom; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { offsetY -= 0.01f * zoom; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { offsetY += 0.01f * zoom; }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, 1); }
}

int newWindowWidth = 1024;
int newWindowHeight = 1024;
int currentWindowWidth = newWindowWidth;
int currentWindowHeight = newWindowHeight;

// This is called whenever the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); // Make sure the viewport matches the new window dimensions
    newWindowWidth = width;
    newWindowHeight = height;
}

int main(){
    GLFWwindow* window;

    // Init glfw
    if(!glfwInit()){ return -1; }

    // Use openGL 4.6, core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    window = glfwCreateWindow(1024,1024,"Agent simulation", nullptr, nullptr);
    if (!window) {glfwTerminate(); return -1;}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(0);; // vsync

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Vertex position + UV + indice data (pos is screenspace)
    float verts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {
        0,1,2, // tri 1
        2,3,0  // tri 2
    };

    // Set up VBO, VBO layout, IBO, VAO
    VBO vertexBuffer(verts, 4 * 5 * sizeof(float));
    VBO_layout layout;
    layout.pushFloat(3); // Position data
    layout.pushFloat(2); // TexCoord data
    std::cout << sizeof(indices) << std::endl;
    IBO indexBuffer(indices, sizeof(indices));
    VAO vertexArray;
    vertexArray.addBuffer(vertexBuffer, layout);

    // Create a shader program
    shaderClass shader1("GLSL_files/vs.glsl", "GLSL_files/fs.glsl");
    shader1.use();
    float zoom = 1.0f;
    float offset[2] = { 0.0f, 0.0f };
    float ratio = 1.0f;
    shader1.setUniform1f("zoom", zoom);
    shader1.setUniform2f("offset", offset[0], offset[1]);
    shader1.setUniform1f("ratio", ratio);

    // Create a texture
    unsigned int res = 2048;
    unsigned int* textures = new unsigned int[1];
    makeTextures(textures, 1, res);
    activebindtex(textures[0], 0, 0);

    // Create compute shader programs
    computeShaderClass agent("GLSL_files/agent.glsl");
    agent.use();
    glUniform1i(glGetUniformLocation(agent.ID, "size"), res);
    glUniform1f(glGetUniformLocation(agent.ID, "sensorDistance"), 20.0f);
    glUniform1f(glGetUniformLocation(agent.ID, "angleChange"), 0.5f);
    glUniform1f(glGetUniformLocation(agent.ID, "turnSpeed"), 10.0f);
    computeShaderClass diffuseAndFade("GLSL_files/diffuseAndFade.glsl");
    diffuseAndFade.use();
    glUniform1f(glGetUniformLocation(diffuseAndFade.ID, "pixelMult"), 0.1f);
    glUniform1f(glGetUniformLocation(diffuseAndFade.ID, "newPixelMult"), 0.895f);
    GLCall(glUniform1i(glGetUniformLocation(diffuseAndFade.ID, "size"), res));

    // Create some data to send to the compute shader
    struct shader_data_t
    {
        float xPos[AGENT_N];
        float yPos[AGENT_N];
        float angle[AGENT_N];
    };

    // Allocate on heap
    shader_data_t *shader_data = new shader_data_t;

    for (auto& i : shader_data->xPos) {
        i = (res / 2) + (res / 4) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
    for (auto& i : shader_data->yPos) {
        i = (res/2) + (res/4) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
    for (auto& i : shader_data->angle) {
        i = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 * 3.14159265359;
    }

    // Create a shader storage buffer object
    unsigned int ssbo;
    GLCall(glGenBuffers(1, &ssbo));
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo));
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data->xPos)*3, shader_data, GL_DYNAMIC_COPY));

    /*
      void* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
      memcpy(p, &shader_data, sizeof(shader_data->xPos)*3);
      GLCall(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));
    */

    // Make it possible to actually read/write to ssbo
    unsigned int block_index;
    block_index = glGetProgramResourceIndex(agent.ID, GL_SHADER_STORAGE_BLOCK, "shader_data");
    glShaderStorageBlockBinding(agent.ID, block_index, 0);
    unsigned int binding_point_index = 0;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, ssbo);

    // Misc setup stuff
    glDisable(GL_BLEND);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0);
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    // Render loop
    renderer renderer;
    while (!glfwWindowShouldClose(window)) {

        // Check performance
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Run compute shaders
        agent.use();
        glDispatchCompute(AGENT_N, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        diffuseAndFade.use();
        glDispatchCompute(res, res, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Render texture
        shader1.use();
        processInput(window, zoom, offset[0], offset[1]);
        shader1.setUniform1f("zoom", zoom);
        shader1.setUniform2f("offset", offset[1], offset[0]);
        activebindtex(textures[0], 0, 0);
        renderer.draw(vertexArray, indexBuffer, shader1);

        // Check if window dimensions have changed
        if (currentWindowWidth != newWindowWidth || currentWindowHeight != newWindowHeight) {
            currentWindowWidth = newWindowWidth;
            currentWindowHeight = newWindowHeight;
            ratio = (float)currentWindowWidth / (float)currentWindowHeight;
            shader1.use();
            shader1.setUniform1f("ratio", ratio);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
