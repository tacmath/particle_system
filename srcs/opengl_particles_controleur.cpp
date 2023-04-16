#include "opengl_particles_controleur.hpp"


void ParticlesControleur::Init(size_t nbParticles, const VBO& glPosBuffer) {
	this->nbParticles = nbParticles;
    program.Load("shaders/particleCS.glsl");
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, glPosBuffer.ID, 0, nbParticles * sizeof(GLfloat) * 3);
}

void ParticlesControleur::Stop() {

}

void ParticlesControleur::Compute() {
    program.Activate();
    glDispatchCompute(nbParticles, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ParticlesControleur::UpdateInfo(const ParticlesInfo& info) {
}

void ParticlesControleur::ResetVelocity() {
}



ComputeShader::ComputeShader() {
	ID = 0;
}

ComputeShader::~ComputeShader() {
//	std::cout << "program destructor " << std::endl;
	glDeleteProgram(ID);
}

void ComputeShader::Load(const std::string& fileName) {
    std::string shaderCode = GetSource(fileName);
    const char* shaderCodePtr = shaderCode.c_str();
    unsigned int compute;
    // compute shader
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &shaderCodePtr, NULL);
    glCompileShader(compute);
    compileErrors(compute, "COMPUTE");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");
}

void ComputeShader::Activate() {
	glUseProgram(ID);
}
/*
ComputeShader& ComputeShader::operator=(ComputeShader&& shader) noexcept {
	*this = std::move(shader);
	return *this;
}*/

std::string ComputeShader::GetSource(const std::string& fileName) {
    std::string source;
    std::ifstream   file;

    if (fileName.empty())
        return source;
    file.open(fileName);
    if (!file.is_open()) {
        std::cout << "Failed to open " << fileName << std::endl;
        return source;
    }
    source.assign((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();
    return (source);
}

void ComputeShader::compileErrors(unsigned int shader, const char* type) {
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
        return;
    }
    glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
    if (hasCompiled == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
    }
}
