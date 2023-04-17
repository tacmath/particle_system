#pragma once

#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <glm/vec3.hpp>
#include "VBO.h"


struct ParticlesInfo {
	GLfloat		center[3];
	GLboolean	hasGravity;

	void SetCenter(const glm::vec3& _center) {
		center[0] = _center.x;
		center[1] = _center.y;
		center[2] = _center.z;
	}

	glm::vec3 GetCenter() {
		return glm::vec3(center[0], center[1], center[2]);
	}
};

class ComputeShader {
	GLuint ID;
public:
	ComputeShader();
	~ComputeShader();

	void Load(const std::string& fileName);
	void Activate();

	void BindBlock(const char* name, GLuint bindIndex);
	void SetBlockBuffer(GLuint bindIndex, VBO& buffer);
//	ComputeShader& operator=(ComputeShader&& shader) noexcept;

private:
	std::string GetSource(const std::string& fileName);
	void compileErrors(unsigned int shader, const char* type);
};

class ParticlesControleur {
	uint32_t			nbParticles;
	VBO					velocityBuffer;
	VBO					infoBuffer;
	ComputeShader		program;


public:
	void Init(uint32_t nbParticles, const VBO& glPosBuffer);
	void Stop();
	void Compute();
	void UpdateInfo(const ParticlesInfo& info);
	void ResetVelocity();
private:
};