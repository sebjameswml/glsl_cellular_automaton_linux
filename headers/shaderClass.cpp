#include "shaderClass.h"

// Compiling shaders in a function so we dont have to write compilation code twice
unsigned int shaderClass::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type); // Create shader
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr); // Set shader source
    glCompileShader(id); // Compile shader
    // Give us some information if compilation fails:
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        char info[1024];
        glGetShaderInfoLog(id, 1024, NULL, info);
        std::cout << "Failed to compile shader\n" << info << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}
// Constructor reads and builds the shader
shaderClass::shaderClass(const char* vertexPath, const char* fragmentPath) {
    // Stuff for debugging:
    int success;
    char infoLog[512];

    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vShaderCode);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fShaderCode);
    // Create Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);
    // Print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vs);
    glDeleteShader(fs);
}
// Use/activate the shader
void shaderClass::use() const {
    glUseProgram(ID);
}

void shaderClass::setUniform4f(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void shaderClass::setUniform3f(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void shaderClass::setUniform2f(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void shaderClass::setUniform1f(const std::string& name, float x) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), x);
}

void shaderClass::setUniform4i(const std::string& name, int x, int y, int z, int w) const {
    glUniform4i(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void shaderClass::setUniform3i(const std::string& name, int x, int y, int z) const {
    glUniform3i(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void shaderClass::setUniform2i(const std::string& name, int x, int y) const {
    glUniform2i(glGetUniformLocation(ID, name.c_str()), x, y);
}
void shaderClass::setUniform1i(const std::string& name, int x) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), x);
}
