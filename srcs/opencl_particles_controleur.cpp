#ifndef USE_COMPUTE_SHADER

#include "opencl_particles_controleur.hpp"

static const char *getClErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

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
	cl_int error = cl::Platform::get(&platforms);
	if (error != CL_SUCCESS) {
		std::cerr << "Clerror : cl::Platform::get : "  << getClErrorString(error) << std::endl;
		exit(1);
	}
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