#include "shader.h"

std::string Shader::getShaderSource(const char* fileName) {
    std::string source;
    std::ifstream   file;

    if (!fileName)
        return source;
    file.open(fileName);
    if (!file.is_open()) {
        printf("Failed to open %s\n", fileName);
        return source;
    }
    source.assign((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();
    return (source);
}

void Shader::Load(const char *vertexShaderFile, const char *fragmentShaderFile, const char *geometryShaderFile) {
    std::vector<std::string> options;
    Load(options, vertexShaderFile, fragmentShaderFile, geometryShaderFile);
}

 
static std::string addOptionToShaderSource(const std::vector<std::string> &options, std::string source) {
    std::string fileSource;
    std::string shaderOptions = "";
    
    if (options.empty() || source.empty())
        return source;
    fileSource = source;
    for (std::string option : options)
        shaderOptions += "#define " + option + '\n';
    fileSource.insert(fileSource.find_first_of('\n') + 1, shaderOptions);
    return fileSource;
}

void Shader::Load(const std::vector<std::string> &options, const char *vertexShaderFile, const char *fragmentShaderFile, const char *geometryShaderFile) {
    std::string vertexShaderSource = addOptionToShaderSource(options, getShaderSource(vertexShaderFile));
    std::string fragmentShaderSource = addOptionToShaderSource(options, getShaderSource(fragmentShaderFile));
    std::string geometryShaderSource = addOptionToShaderSource(options, getShaderSource(geometryShaderFile));

    ID = 0;
    if (vertexShaderSource.empty() || fragmentShaderSource.empty())
        return ;

    LoadSources(vertexShaderSource.c_str(), fragmentShaderSource.c_str(), geometryShaderSource.empty() ? 0 : geometryShaderSource.c_str());
}

void Shader::LoadSources(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    GLuint geometryShader = 0;
    if (geometryShaderSource) {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
        glCompileShader(geometryShader);
        compileErrors(geometryShader, "GEOMETRY");
    }

    GLuint programShader = glCreateProgram();
    glAttachShader(programShader, vertexShader);
    glAttachShader(programShader, fragmentShader);
    if (geometryShader)
        glAttachShader(programShader, geometryShader);
    glLinkProgram(programShader);
    compileErrors(programShader, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
    ID = programShader;
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
    // Stores status of compilation
    GLint hasCompiled;
    // Character array to store error message in
    char infoLog[1024];
    if (!strcmp(type, "PROGRAM"))
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
        }
        return ;
    }
    glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
    if (hasCompiled == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
    }
}