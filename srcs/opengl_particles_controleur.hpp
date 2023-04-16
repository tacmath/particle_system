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
	GLint		hasGravity;

	void SetCenter(const glm::vec3& _center) {
		center[0] = _center.x;
		center[1] = _center.y;
		center[2] = _center.z;
	}
};

class ComputeShader {
	GLuint ID;
public:
	ComputeShader();
	~ComputeShader();

	void Load(const std::string& fileName);
	void Activate();

//	ComputeShader& operator=(ComputeShader&& shader) noexcept;

private:
	std::string GetSource(const std::string& fileName);
	void compileErrors(unsigned int shader, const char* type);
};

class ParticlesControleur {
	size_t				nbParticles;
	ComputeShader		program;


public:
	void Init(size_t nbParticles, const VBO& glPosBuffer);
	void Stop();
	void Compute();
	void UpdateInfo(const ParticlesInfo& info);
	void ResetVelocity();
private:
};