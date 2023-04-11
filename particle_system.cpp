#include "particle_system.hpp"
#include "utils.hpp"


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
	sprintf_s(title, "Particle System :  FPS = %d  ms = %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames);
	glfwSetWindowTitle(window, title);
	frames = 0;
	oldTime = newTime;
}

void ParticleSystem::Start()
{
	InitCl();
	InitGl();
	CreateKernel();
}

void ParticleSystem::Run()
{
	isRunning = true;
//	glfwSwapInterval(0);
	while (isRunning) {
		glfwPollEvents();
		if (glfwWindowShouldClose(window.context) == 1 || glfwGetKey(window.context, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			isRunning = false;

		RunCl();

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, 8);

		showFPS(window.context);

		glfwSwapBuffers(window.context);
	}
}

void ParticleSystem::Stop()
{
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

	const cl_context_properties context_properties[] =
	{
		#ifdef _WIN32
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		#else
		CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
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
	cl::Program program = utils::BuildProgram(clContext, device, { "particle.cl" });
	kernel = cl::Kernel(program, "Particle", &err);
	_ASSERT(err == CL_SUCCESS);

	velBuffer = cl::Buffer(clContext, CL_MEM_READ_WRITE, 3 * sizeof(float) * NB_PARTICLE);
	posBuffer = cl::BufferGL(clContext, CL_MEM_READ_WRITE, particlesPos.ID);
	kernel.setArg(0, posBuffer);
	kernel.setArg(1, velBuffer);

	glBuffers.push_back(posBuffer);

	//clQueue.enqueueWriteBuffer(velBuffer, CL_FALSE, 0, 3 * sizeof(float) * NB_PARTICLE, 0);
}

void ParticleSystem::RunCl()
{
	glFinish();

	clQueue.enqueueAcquireGLObjects(&glBuffers);
	
	clQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(NB_PARTICLE), cl::NullRange);
	
	clQueue.enqueueReleaseGLObjects(&glBuffers);

	clQueue.finish();
}

void ParticleSystem::InitGl()
{

	const float data[] = {
		0.0f, 0.1f, 0.0f,
		0.1f, 0.1f, 0.0f,
		0.2f, 0.1f, 0.0f,
		0.0f, -0.1f, 0.0f,
		-0.1f, 0.1f, 0.0f,
		-0.1f, -0.1f, 0.0f,
		0.3f, 0.1f, 0.0f,
		0.5f, -0.1f, 0.0f
	};
	
	particlesPos.Gen(0, sizeof(data));

	float* gldata = (float*)particlesPos.Map(GL_WRITE_ONLY);
	for (int n = 0; n < 24; n++)
		gldata[n] = data[n];
	particlesPos.Unmap();

	vao.Gen();
	vao.LinkAttrib(particlesPos, 0, 3, GL_FLOAT, sizeof(float), 0);
	shader.Load("particleVS.glsl", "particleFS.glsl");
	shader.Activate();
	vao.Bind();
	glFinish();
}