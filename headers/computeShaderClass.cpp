#include "computeShaderClass.h"

computeShaderClass::computeShaderClass(const char* path)
{
    // Stuff for debugging:
    int success;
    char infoLog[512];

    // Load shader file
    std::string computeCode;
    std::ifstream cShaderFile;
    // Ensure ifstream objects can throw exceptions:
    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Open file
        cShaderFile.open(path);
        std::stringstream cShaderStream;
        // Read file's buffer contents into streams
        cShaderStream << cShaderFile.rdbuf();
        // Close file handler
        cShaderFile.close();
        // Convert stream into string
        computeCode = cShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* cShaderCode = computeCode.c_str();

    // Create compute shader program
    unsigned int compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, &cShaderCode, NULL);
    glCompileShader(compute_shader);

    // Print compiling errors if any
    int result;
    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        char info[1024];
        glGetShaderInfoLog(compute_shader, 1024, NULL, info);
        std::cout << "Failed to compile shader\n" << info << std::endl;
        glDeleteShader(compute_shader);
    }

    // Create program:
    ID = glCreateProgram();
    glAttachShader(ID, compute_shader);
    glLinkProgram(ID);
    // Print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // Shader isnt needed after its linked to a program
    glDeleteShader(compute_shader);
}

void computeShaderClass::use() const
{
    glUseProgram(ID);
}
