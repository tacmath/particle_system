#ifndef UTILS_HPP
#define UTILS_HPP

#include <CL/opencl.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

namespace utils {

	cl::Program::Sources GetSources(const std::vector<std::string>& files) {
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

	cl::Program BuildProgram(const cl::Context& context, const cl::Device &device, const std::vector<std::string>& files) {
		cl::Program program(context, GetSources(files));
		if (program.build(device) != CL_SUCCESS) {
			std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
			exit(1);
		}
		return program;
	}

	bool IsCLExtensionSupported(const cl::Device& device, const char* extension)
	{
		if (extension == 0 || extension[0] == '\0')
			return false;
		cl::string extentions = device.getInfo<CL_DEVICE_EXTENSIONS>();
		return extentions.find(extension) != cl::string::npos;
	}
};

#endif // !UTILS_HPP