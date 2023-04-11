#include "particle_system.hpp"
#include "utils.hpp"


void ParticleSystem::Start()
{
	InitCl();
	InitGl();
}

void ParticleSystem::Run()
{
	RunCl();
	RunGl();
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

void ParticleSystem::RunCl()
{
	int A_h[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int B_h[] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 2 };
	int C_h[10];

	int D_h = 2;

	cl::Buffer A_d(clContext, CL_MEM_READ_ONLY, sizeof(A_h));
	cl::Buffer B_d(clContext, CL_MEM_READ_ONLY, sizeof(B_h));
	cl::Buffer D_d(clContext, CL_MEM_READ_ONLY, sizeof(D_h));
	cl::Buffer C_d(clContext, CL_MEM_WRITE_ONLY, sizeof(C_h));

	cl::Program program;

	

	program = utils::BuildProgram(clContext, device, { "kernel.cl" });
	cl::compatibility::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> vector_add(cl::Kernel(program, "vector_add"));

	cl::EnqueueArgs eargs(clQueue, cl::NullRange, cl::NDRange(10), cl::NullRange);
	clQueue.enqueueWriteBuffer(A_d, CL_TRUE, 0, sizeof(A_h), A_h);
	clQueue.enqueueWriteBuffer(B_d, CL_TRUE, 0, sizeof(B_h), B_h);
	clQueue.enqueueWriteBuffer(D_d, CL_TRUE, 0, sizeof(D_h), &D_h);

	vector_add(eargs, A_d, B_d, C_d, D_d).wait();

	clQueue.enqueueReadBuffer(C_d, CL_TRUE, 0, sizeof(C_h), C_h);


	for (auto nb : C_h)
		std::cout << nb << std::endl;
}

void ParticleSystem::InitGl()
{

	GLuint pbuff, vbuff;

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

	glGenBuffers(1, &pbuff);
	glBindBuffer(GL_ARRAY_BUFFER, pbuff);
	glBufferData(GL_ARRAY_BUFFER, 3 * 8 * sizeof(float), NULL, GL_STATIC_DRAW);
	
	glGenBuffers(1, &vbuff);
	glBindBuffer(GL_ARRAY_BUFFER, vbuff);
	glBufferData(GL_ARRAY_BUFFER, 3 * 8 * sizeof(float), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vao.Gen();
	vbo.Gen(data, sizeof(data));
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(float), 0);
	shader.Load("particleVS.glsl", "particleFS.glsl");
	shader.Activate();
	vao.Bind();
	glFinish();
}

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

void ParticleSystem::RunGl()
{
	isRunning = true;
	//glfwSwapInterval(0);
	while (isRunning) {
		glfwPollEvents();
		if (glfwWindowShouldClose(window.context) == 1 || glfwGetKey(window.context, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			isRunning = false;

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_POINTS, 0, 8);

		showFPS(window.context);

		glfwSwapBuffers(window.context);
		glFinish();
	}
}
