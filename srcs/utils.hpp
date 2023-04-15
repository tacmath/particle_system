#ifndef UTILS_HPP
#define UTILS_HPP

#include <CL/opencl.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <glm/vec3.hpp>
#include "VBO.h"
#include <GLFW/glfw3.h>

#define TIME_PER_COLOR 5.0f

namespace utils {

	cl::Program::Sources GetSources(const std::vector<std::string>& files);

	cl::Program BuildProgram(const cl::Context& context, const cl::Device& device, const std::vector<std::string>& files);

	bool IsCLExtensionSupported(const cl::Device& device, const char* extension);

	glm::vec3 GetRandomPointInSphere();
	glm::vec3 GetRandomPointInCube();

	void		InitParticles(VBO &buffer, size_t nbParticles, bool sphere);

	class ColorList {
		std::vector<glm::vec3>	colors;
		glm::vec3				currentColor;
		unsigned				currentIndex;
		float					lastColorChange;
	public:
		void Update();
		void Add(const glm::vec3& color);
		void Init(const std::vector<glm::vec3>& colorList);
		const glm::vec3& GetCurrent();
	};
};

#endif // !UTILS_HPP