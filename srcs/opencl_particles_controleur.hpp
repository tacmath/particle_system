#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif


#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "VBO.h"

#ifndef _WIN32
#define GLFW_EXPOSE_NATIVE_GLX
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
};

class ParticlesControleur {
	size_t				nbParticles;
	cl::Device			device;
	cl::Context			clContext;
	cl::CommandQueue	clQueue;
	cl::BufferGL		posBuffer;
	cl::Buffer			velBuffer, infoBuffer;
	cl::Kernel			kernel;
	std::vector<cl::Memory> GLObjects;

public:
	void Init(size_t nbParticles, const VBO& glPosBuffer);
	void Stop();
	void Compute();
	void UpdateInfo(const ParticlesInfo& info);
	void ResetVelocity();
private:
	void InitCL();
	void CreateKernel(const VBO& glPosBuffer);
};