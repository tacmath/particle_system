#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<fstream>
#include<iostream>
#include <streambuf>
//#include<glm/gtc/type_ptr.hpp>
#include<vector>
#include<string>

class Shader {
public:
    // Reference ID of the Shader Program
    GLuint ID;
    // default constuctor
    Shader() { ID = 0; }
    // Constructor that build the Shader Program from 2 different shaders
    Shader(const char* vertexFile, const char* fragmentFile) { Load(vertexFile, fragmentFile); };

    void Load(const char* vertexFile, const char* fragmentFile, const char* geometryShaderFile = 0);

    void Load(const std::vector<std::string> &options, const char* vertexFile, const char* fragmentFile, const char* geometryShaderFile = 0); //maybe give the possibiliti to add define with value and change the version

    void LoadSources(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource = 0);

    void Activate() {
        glUseProgram(ID);
    }

    void Delete() {
        glDeleteProgram(ID);
        ID = 0;
    }
    /*
    void setMat4(const char* name, const glm::mat4& matrix) {
        glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name), 1, GL_FALSE, &matrix[0][0]);
    }
    void setMat4(const char* name, int numbers, const glm::mat4* matrix) {
        glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name), numbers, GL_FALSE, &matrix[0][0][0]);
    }
    void setVec3(const char* name, const glm::vec3& vector) {
        glProgramUniform3fv(ID, glGetUniformLocation(ID, name), 1, &vector[0]);
    }*/
    void setVec3(const char* name, float x, float y, float z) {
        glProgramUniform3f(ID, glGetUniformLocation(ID, name), x, y, z);
    }
    inline void setVec2(const char* name, float x, float y) {
        glProgramUniform2f(ID, glGetUniformLocation(ID, name), x, y);
    }
    inline void setVec2(const char* name, int x, int y) {
        glProgramUniform2i(ID, glGetUniformLocation(ID, name), x, y); //maybe change it to calculate glGetUniformLocation only once
    }
    void setInt(const char* name, int value) {
        glProgramUniform1i(ID, glGetUniformLocation(ID, name), value);
    }
    void setFloat(const char* name, float value) {
        glProgramUniform1f(ID, glGetUniformLocation(ID, name), value);
    }

private:
    // Checks if the different Shaders have compiled properly
    void compileErrors(unsigned int shader, const char* type);

    std::string getShaderSource(const char* fileName);
};

#endif

