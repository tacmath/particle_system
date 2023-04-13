#define CL_HPP_TARGET_OPENCL_VERSION 300
#include "utils.hpp"


cl::Program::Sources utils::GetSources(const std::vector<std::string>& files) {
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

cl::Program utils::BuildProgram(const cl::Context& context, const cl::Device& device, const std::vector<std::string>& files) {
	cl::Program program(context, GetSources(files));
	if (program.build(device) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
		exit(1);
	}
	return program;
}

bool utils::IsCLExtensionSupported(const cl::Device& device, const char* extension)
{
	if (extension == 0 || extension[0] == '\0')
		return false;
	cl::string extentions = device.getInfo<CL_DEVICE_EXTENSIONS>();
	return extentions.find(extension) != cl::string::npos;
}


glm::vec3 utils::GetRandomPointInSphere() {
	float d, x, y, z;
	do {
		x = ((float)rand() / (RAND_MAX)) * 2.0f - 1.0f;
		y = ((float)rand() / (RAND_MAX)) * 2.0f - 1.0f;
		z = ((float)rand() / (RAND_MAX)) * 2.0f - 1.0f;
		d = x * x + y * y + z * z;
	} while (d > 1.0f);
	return glm::vec3(x, y, z);
}

glm::vec3 utils::GetRandomPointInCube() {
	glm::vec3 result;
	result.x = ((float)rand() / (RAND_MAX)) * 2.0f - 1.0f;
	result.y = ((float)rand() / (RAND_MAX)) * 2.0f - 1.0f;
	result.z = ((float)rand() / (RAND_MAX)) * 2.0f - 1.0f;
	return result;
}

void utils::InitParticles(VBO& buffer, size_t nbParticles, bool sphere) {
	float* gldata = (float*)buffer.Map(GL_WRITE_ONLY);
	for (size_t n = 0; n < nbParticles; n++) {
		glm::vec3 point = (sphere) ? GetRandomPointInSphere() : GetRandomPointInCube();
		gldata[(n * 3)] = point.x;
		gldata[(n * 3) + 1] = point.y;
		gldata[(n * 3) + 2] = point.z;
	}
	buffer.Unmap();
}
