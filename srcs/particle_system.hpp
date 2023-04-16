#pragma once

#ifndef _ASSERT
	#define _ASSERT(assertion) if (!(assertion)) std::cout << "Assertion failed" << std::endl; exit(1)
#endif


#include "opengl_particles_controleur.hpp"
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
	void InitGl();

	void SetGlfwCallbacks();
	void SetEventCallbacks();
	void GetEvents();
};