#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vector>
#include <iostream>
#include "utils.hpp"
#include "window.h"
#include "VAO.h"
#include "VBO.h"
#include "shader.h"
#include "camera.h"
#include <functional>

#define NB_PARTICLE 1000000


typedef cl::compatibility::make_kernel<cl::Buffer, cl::Buffer> ComputeParticle;

struct ParticlesInfo {
	cl_float4 center;
	cl_bool	  hasGravity;
};

struct EventCallbacks {
	std::function<void(double mouseX, double mouseY)>					onMouseMouvement;
	std::function<void(int key, int scancode, int action, int mods)>	onKey;
	std::function<void(int width, int height)>							onFramebufferSize;
	

	EventCallbacks() {
		onMouseMouvement =	[](double, double){};
		onKey =				[](int, int, int, int) {};
		onFramebufferSize =	[](int, int) {};
	}
};


class ParticleSystem {
	//modules
	Window		window;
	Camera		camera;

	//opencl
	cl::Device			device;
	cl::Context			clContext;
	cl::CommandQueue	clQueue;
	cl::BufferGL		posBuffer;
	cl::Buffer			velBuffer, infoBuffer;
	cl::Kernel			kernel;
	std::vector<cl::Memory> GLObjects;

	//opengl
	VAO		vao;
	VBO		particlesPos;
	Shader	shader;

	//other
	ParticlesInfo	info;
	EventCallbacks  callbacks;
	bool			freeCursor = false;
	bool			isSphere = true;
	bool			isRunning = false;

public:
	void Start();
	void Run();
	void Stop();
private:
	void InitCl();
	void CreateKernel();
	void ComputeParticles();

	void InitGl();

	void SetGlfwCallbacks();
	void SetEventCallbacks();
	void GetEvents();
};