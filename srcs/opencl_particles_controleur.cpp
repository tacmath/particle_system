#ifndef USE_COMPUTE_SHADER

#include "opencl_particles_controleur.hpp"

cl::Program::Sources openclUtils::GetSources(const std::vector<std::string>& files) {
	cl::Program::Sources sources;
	std::fstream		 stream;

	for (const std::string& fileName : files) {
		stream.open(fileName);
		std::string kernel_code((std::istreambuf_iterator<char>(stream)),
			std::istreambuf_iterator<char>());
		stream.close();
		sources.push_back({ kernel_code.c_str(),kernel_code.length() + 1 });
	}
	return sources;
}

cl::Program openclUtils::BuildProgram(const cl::Context& context, const cl::Device& device, const std::vector<std::string>& files) {
	cl::Program program(context, GetSources(files));
	if (program.build(device) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
		exit(1);
	}
	return program;
}

bool openclUtils::IsCLExtensionSupported(const cl::Device& device, const char* extension)
{
	if (extension == 0 || extension[0] == '\0')
		return false;
	cl::string extentions = device.getInfo<CL_DEVICE_EXTENSIONS>();
	return extentions.find(extension) != cl::string::npos;
}



void ParticlesControleur::Init(uint32_t nbParticles, const VBO& glPosBuffer) {
	this->nbParticles = nbParticles;
	glFinish();
	InitCL();
	CreateKernel(glPosBuffer);
}

void ParticlesControleur::Stop() {
	clQueue.finish();
}

void ParticlesControleur::Compute() {
	glFinish(); //for portability

	clQueue.enqueueAcquireGLObjects(&GLObjects);

	clQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(nbParticles), cl::NullRange);

	clQueue.enqueueReleaseGLObjects(&GLObjects);
}

void ParticlesControleur::UpdateInfo(const ParticlesInfo& info) {
	clQueue.enqueueWriteBuffer(infoBuffer, CL_FALSE, 0, sizeof(ParticlesInfo), &info);
}

void ParticlesControleur::ResetVelocity() {
	clQueue.enqueueFillBuffer(velBuffer, 0, 0, 3 * sizeof(float) * nbParticles);
}

void ParticlesControleur::InitCL() {
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

	if (!openclUtils::IsCLExtensionSupported(device, "cl_khr_gl_sharing")) {
		std::cerr << "cl_khr_gl_sharing is not suported" << std::endl;
		exit(1);
	}

	#ifndef _WIN32
		GLFWwindow* glfwContext = glfwGetCurrentContext();
	#endif

	cl_context_properties context_properties[] =
	{
		#ifdef _WIN32
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		#else
		CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetGLXContext(glfwContext),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glfwGetGLXWindow(glfwContext),
		#endif
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platform()),
		0
	};

	clContext = cl::Context(device, context_properties);
	clQueue = cl::CommandQueue(clContext, device);
}

void ParticlesControleur::CreateKernel(const VBO& glPosBuffer) {
	cl_int err;
	cl::Program program = openclUtils::BuildProgram(clContext, device, { "kernel/particle.cl" });
	kernel = cl::Kernel(program, "Particle", &err);
	_ASSERT(err == CL_SUCCESS);

	velBuffer = cl::Buffer(clContext, CL_MEM_READ_WRITE, 3 * sizeof(float) * nbParticles);
	infoBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY, sizeof(ParticlesInfo));
	posBuffer = cl::BufferGL(clContext, CL_MEM_READ_WRITE, glPosBuffer.ID);
	kernel.setArg(0, posBuffer);
	kernel.setArg(1, velBuffer);
	kernel.setArg(2, infoBuffer);

	GLObjects.push_back(posBuffer);

	clQueue.enqueueFillBuffer(velBuffer, 0, 0, 3 * sizeof(float) * nbParticles);
}

#endif // !USE_COMPUTE_SHADER