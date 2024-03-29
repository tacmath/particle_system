#ifdef USE_COMPUTE_SHADER

#include "opengl_particles_controleur.hpp"


void ParticlesControleur::Init(uint32_t nbParticles, const VBO& glPosBuffer) {
	this->nbParticles = nbParticles;
    program.Load("shaders/particleCS.glsl");
    infoBuffer.Gen(0, sizeof(ParticlesInfo), GL_DYNAMIC_DRAW);
    velocityBuffer.Gen(0, nbParticles * sizeof(GLfloat) * 3);
    float* data = (float*)velocityBuffer.Map(GL_WRITE_ONLY); //need to find a better system
    for (size_t n = 0; n < nbParticles * 3; n++)
        data[n] = 0;
    velocityBuffer.Unmap();
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, glPosBuffer.ID, 0, nbParticles * sizeof(GLfloat) * 3);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer.ID, 0, nbParticles * sizeof(GLfloat) * 3);
    program.BindBlock("info", 0);
    program.SetBlockBuffer(0, infoBuffer);
}

void ParticlesControleur::Stop() {
    infoBuffer.Delete();
    velocityBuffer.Delete();
}

void ParticlesControleur::Compute() {
    program.Activate();
    glDispatchCompute((GLuint)(nbParticles / 64), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticlesControleur::UpdateInfo(const ParticlesInfo& info) {
    glNamedBufferSubData(infoBuffer.ID, 0, sizeof(ParticlesInfo), &info);
}

void ParticlesControleur::ResetVelocity() {
    velocityBuffer.Delete();
    velocityBuffer.Gen(0, nbParticles * sizeof(GLfloat) * 3);
    void* data = (float*)velocityBuffer.Map(GL_WRITE_ONLY); //need to find a better system
    memset(data, 0, nbParticles * sizeof(GLfloat) * 3);
    velocityBuffer.Unmap();
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer.ID, 0, nbParticles * sizeof(GLfloat) * 3);
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

void ComputeShader::BindBlock(const char* name, GLuint bindIndex) {
    glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name), bindIndex);
}

void ComputeShader::SetBlockBuffer(GLuint bindIndex, VBO &buffer) {
    glBindBufferBase(GL_UNIFORM_BUFFER, bindIndex, buffer.ID);
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

#endif
