#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vector>
#include <iostream>
#include "window.h"
#include "VAO.h"
#include "VBO.h"
#include "shader.h"

#define MAX_PARTICLE 8


typedef cl::compatibility::make_kernel<cl::Buffer, cl::Buffer> ComputeParticle;

class ParticleSystem {
	//modules
	Window		window;

	//opencl
	cl::Device			device;
	cl::Context			clContext;
	cl::CommandQueue	clQueue;

	//opengl
	VAO		vao;
	VBO		vbo;
	Shader	shader;

	//other
	bool isRunning = false;

public:
	void Start();
	void Run();
	void Stop();
private:
	void InitCl();
	void RunCl();

	void InitGl();
	void RunGl();
};