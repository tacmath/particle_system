#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/opencl.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifndef _ASSERT
	#define _ASSERT(assertion) if (!(assertion)) std::cout << "Assertion failed" << std::endl; exit(1)
#endif

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <glm/vec3.hpp>
#include "VBO.h"

#ifndef _WIN32
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

namespace openclUtils {
	cl::Program::Sources GetSources(const std::vector<std::string>& files);

	cl::Program BuildProgram(const cl::Context& context, const cl::Device& device, const std::vector<std::string>& files);

	bool IsCLExtensionSupported(const cl::Device& device, const char* extension);
};

struct ParticlesInfo {
	cl_float4 center;
	cl_bool	  hasGravity;

	void SetCenter(const glm::vec3& _center) {
		center = { _center.x, _center.y, _center.z, 0 };
	}

	glm::vec3 GetCenter() {
		return glm::vec3(center.s[0] , center.s[1], center.s[2]);
	}
};

class ParticlesControleur {
	uint32_t			nbParticles;
	cl::Device			device;
	cl::Context			clContext;
	cl::CommandQueue	clQueue;
	cl::BufferGL		posBuffer;
	cl::Buffer			velBuffer, infoBuffer;
	cl::Kernel			kernel;
	std::vector<cl::Memory> GLObjects;

public:
	void Init(uint32_t nbParticles, const VBO& glPosBuffer);
	void Stop();
	void Compute();
	void UpdateInfo(const ParticlesInfo& info);
	void ResetVelocity();
private:
	void InitCL();
	void CreateKernel(const VBO& glPosBuffer);
};