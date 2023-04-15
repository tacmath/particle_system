#include "particle_system.hpp"

static void showFPS(GLFWwindow* window) {
	static double oldTime = 0;
	static double newTime;
	static int frames = 0;
	static char title[60];
	double timeDiff;

	newTime = glfwGetTime();
	timeDiff = newTime - oldTime;
	frames++;
	if (timeDiff < 1.0f / 30.0f)
		return;
	#ifdef _WIN32
		sprintf_s(title, "Particle System :  FPS = %d  ms = %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames);
	#else
		sprintf(title, "Particle System :  FPS = %d  ms = %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames);
	#endif
	glfwSetWindowTitle(window, title);
	frames = 0;
	oldTime = newTime;
}

void ParticleSystem::Start()
{
	info.center = { 0,0,0,0 };
	info.hasGravity = false;

	camera.Init(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, glm::vec3(0, 0, -2));

	InitCl();
	InitGl();
	CreateKernel();
	SetGlfwCallbacks();
	SetEventCallbacks();
}

void ParticleSystem::Run()
{
	isRunning = true;
//	glfwSwapInterval(0);
	while (isRunning) {

		GetEvents();

		ComputeParticles();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, NB_PARTICLE);

		showFPS(window.context);

		glfwSwapBuffers(window.context);
	}
}

void ParticleSystem::Stop()
{
	clQueue.finish();
	glFinish();
	shader.Delete();
}

void ParticleSystem::InitCl()
{
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Platform::get(&platforms);

	_ASSERT(platforms.size() > 0);
	cl::Platform platform = platforms.front();

	std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

	_ASSERT(devices.size() > 0);

	for (auto& device : devices) {
		std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";
		std::cout << "Vendor : " << device.getInfo<CL_DEVICE_VENDOR>() << "\n";
		std::cout << "Version : " << device.getInfo<CL_DEVICE_VERSION>() << "\n";
	}

	device = devices.front();

	if (!utils::IsCLExtensionSupported(device, "cl_khr_gl_sharing")) {
		std::cerr << "cl_khr_gl_sharing is not suported" << std::endl;
		exit(1);
	}

	cl_context_properties context_properties[] =
	{
		#ifdef _WIN32
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		#else
		CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetGLXContext(window.context),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glfwGetGLXWindow(window.context),
		#endif
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platform()),
		0
	};
	
	clContext = cl::Context(device, context_properties);
	clQueue = cl::CommandQueue(clContext, device);
}

void ParticleSystem::CreateKernel()
{
	cl_int err;
	cl::Program program = utils::BuildProgram(clContext, device, { "kernel/particle.cl" });
	kernel = cl::Kernel(program, "Particle", &err);
	_ASSERT(err == CL_SUCCESS);

	velBuffer = cl::Buffer(clContext, CL_MEM_READ_WRITE, 3 * sizeof(float) * NB_PARTICLE);
	infoBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY, sizeof(ParticlesInfo));
	posBuffer = cl::BufferGL(clContext, CL_MEM_READ_WRITE, particlesPos.ID);
	kernel.setArg(0, posBuffer);
	kernel.setArg(1, velBuffer);
	kernel.setArg(2, infoBuffer);

	GLObjects.push_back(posBuffer);

	clQueue.enqueueFillBuffer(velBuffer, 0, 0, 3 * sizeof(float) * NB_PARTICLE);
	clQueue.enqueueWriteBuffer(infoBuffer, CL_TRUE, 0, sizeof(ParticlesInfo), &info);
	clQueue.finish();
}

void ParticleSystem::ComputeParticles()
{
	clQueue.enqueueWriteBuffer(infoBuffer, CL_FALSE, 0, sizeof(ParticlesInfo), &info);

	glFinish(); //for portability

	clQueue.enqueueAcquireGLObjects(&GLObjects);
	
	clQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(NB_PARTICLE), cl::NullRange);
	
	clQueue.enqueueReleaseGLObjects(&GLObjects);

//	clQueue.finish();
}

void ParticleSystem::InitGl()
{
	particlesPos.Gen(0, sizeof(float) * 3 * NB_PARTICLE);

	utils::InitParticles(particlesPos, NB_PARTICLE, isSphere);

	vao.Gen();
	vao.LinkAttrib(particlesPos, 0, 3, GL_FLOAT, sizeof(float), 0);
	shader.Load("shaders/particleVS.glsl", "shaders/particleFS.glsl");
	shader.setMat4("VP", camera.projection * camera.view);
	shader.Activate();
	vao.Bind();
	glEnable(GL_DEPTH_TEST);
//	glPointSize(1.5);
	glFinish();
}

void ParticleSystem::SetGlfwCallbacks() {
	glfwSetWindowUserPointer(window.context, &callbacks);

	glfwSetCursorPosCallback(window.context, [](GLFWwindow* window, double xpos, double ypos) {
		static EventCallbacks* callbacks = (EventCallbacks*)glfwGetWindowUserPointer(window);
		callbacks->onMouseMouvement(xpos, ypos);
	});

	glfwSetKeyCallback(window.context, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		static EventCallbacks* callbacks = (EventCallbacks*)glfwGetWindowUserPointer(window);
		callbacks->onKey(key, scancode, action, mods);
	});

	glfwSetFramebufferSizeCallback(window.context, [](GLFWwindow* window, int width, int height) {
		static EventCallbacks* callbacks = (EventCallbacks*)glfwGetWindowUserPointer(window);
		callbacks->onFramebufferSize(width, height);
	});
}

void ParticleSystem::SetEventCallbacks()
{
	callbacks.onMouseMouvement = [&](double mouseX, double mouseY){
		static double lastMouseX = DEFAULT_WINDOW_WIDTH / 2;
		static double lastMouseY = DEFAULT_WINDOW_HEIGHT / 2;
		
		if (!freeCursor) {
			camera.Rotate((float)(mouseX - lastMouseX) * 0.5f, (float)(mouseY - lastMouseY) * 0.5f);
			camera.Update();
			shader.setMat4("VP", camera.projection * camera.view);
		}
		lastMouseX = mouseX;
		lastMouseY = mouseY;
	};

	callbacks.onKey = [&](int key, int scancode, int action, int mods) {
		(void)scancode;
		(void)mods;
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
			info.hasGravity = (info.hasGravity) ? false : true;
		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			isSphere = !isSphere;
			utils::InitParticles(particlesPos, NB_PARTICLE, isSphere);
		}
		if (key == GLFW_KEY_V && action == GLFW_PRESS)
			clQueue.enqueueFillBuffer(velBuffer, 0, 0, 3 * sizeof(float) * NB_PARTICLE);

		if (key == GLFW_KEY_LEFT_ALT) {
			if (action == GLFW_PRESS) {
				int width, height;
				glfwSetInputMode(window.context, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwGetWindowSize(window.context, &width, &height);
				glfwSetCursorPos(window.context, width / 2.0, height / 2.0);
				freeCursor = true;
				callbacks.onMouseMouvement(width / 2.0, height / 2.0);
			}
			else if (action == GLFW_RELEASE) {
				glfwSetInputMode(window.context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				freeCursor = false;
			}
		}
	};

	callbacks.onFramebufferSize = [&](int width, int height) {
		glViewport(0, 0, width, height);
		camera.ChangePerspective(70, (float)width, (float)height, 0.0001f, 1000.0f);
		shader.setMat4("VP", camera.projection * camera.view);
	};
}

void ParticleSystem::GetEvents() {
	glfwPollEvents();
	if (glfwWindowShouldClose(window.context) == 1 || glfwGetKey(window.context, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		isRunning = false;

	glm::vec3 velocity(0);
	glm::vec3 look = camera.GetDirection();
	if (glfwGetKey(window.context, GLFW_KEY_W) == GLFW_PRESS)
		velocity += look;
	if (glfwGetKey(window.context, GLFW_KEY_A) == GLFW_PRESS)
		velocity += -glm::normalize(glm::cross(look, glm::vec3(0.0f, 1.0f, 0.0f)));
	if (glfwGetKey(window.context, GLFW_KEY_S) == GLFW_PRESS)
		velocity += -look;
	if (glfwGetKey(window.context, GLFW_KEY_D) == GLFW_PRESS)
		velocity += glm::normalize(glm::cross(look, glm::vec3(0.0f, 1.0f, 0.0f)));
	
	velocity *= (glfwGetKey(window.context, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 0.05f : 0.01f;
	
	if (velocity != glm::vec3(0)) {
		camera.Move(velocity);
		camera.Update();
		shader.setMat4("VP", camera.projection * camera.view);
	}

	if (freeCursor && glfwGetMouseButton(window.context, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		int width, height;
		double posx, posy;
		glfwGetCursorPos(window.context, &posx, &posy);
		glfwGetWindowSize(window.context, &width, &height);
		glm::mat4 invVP = glm::inverse(camera.projection * (glm::mat4(glm::mat3(camera.view))));
		glm::vec4 screenPoint((float)(posx / width) * 2.0f - 1.0f, -(float)(posy / height) * 2.0f + 1.0f, 0, 1);
		glm::vec3 point = (glm::vec3(invVP * screenPoint) * 2.0f) + camera.GetPosition();
		info.center = { point.x, point.y, point.z,0 };
		shader.setVec3("center", point);
	}
}
