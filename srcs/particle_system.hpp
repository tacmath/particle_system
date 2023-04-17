#pragma once

#ifndef _ASSERT
	#define _ASSERT(assertion) if (!(assertion)) std::cout << "Assertion failed" << std::endl; exit(1)
#endif

#ifdef USE_COMPUTE_SHADER
#include "opengl_particles_controleur.hpp"
#else
#include "opencl_particles_controleur.hpp"
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
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

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
	MainWindow			window;
	Camera				camera;
	ParticlesControleur particles;
	utils::ColorList	colors;

	//opengl
	VAO		vao;
	VBO		particlesPos;
	Shader	shader;

	//other
	uint32_t		nbParticles = 0;
	ParticlesInfo	info;
	EventCallbacks  callbacks;
	bool			freeCursor = false;
	bool			inMenu = false;
	bool			isSphere = true;
	bool			isRunning = false;

public:
	void Start(uint32_t nbParticles);
	void Run();
	void Stop();
private:
	void InitGl();
	void InitImgui();

	void SetGlfwCallbacks();
	void SetEventCallbacks();
	void GetEvents();
};