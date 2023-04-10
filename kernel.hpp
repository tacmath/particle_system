#pragma once
#include <CL/opencl.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>

class Kernel {
	cl::Program program;

public:
	void Build(const cl::Context& context, const cl::Device &device, const std::vector<std::string>& files) {
		cl::Program program(context, GetSources(files));
		if (program.build({ device }) != CL_SUCCESS) {
			std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
			exit(1);
		}
		this->program = program;
	}

	static cl::Program::Sources GetSources(const std::vector<std::string>& files) {
		cl::Program::Sources sources;
		std::fstream		 stream;
		std::string			 kernel_code;

		for (const std::string& fileName : files) {
			stream.open(fileName);
			kernel_code.assign((std::istreambuf_iterator<char>(stream)),
				std::istreambuf_iterator<char>());
			stream.close();
			sources.push_back({ kernel_code.c_str(),kernel_code.length() });
		}
		return sources;
	}
};