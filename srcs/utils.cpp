#include "utils.hpp"

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

void utils::showFPS(GLFWwindow* window) {
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

void utils::ColorList::Update() {
	float factor = ((float)glfwGetTime() - lastColorChange) / TIME_PER_COLOR;
	glm::vec3 current, next;

	if (factor > 1.0f) {
		lastColorChange = (float)glfwGetTime();
		currentIndex = (currentIndex + 1) * (currentIndex + 1 < colors.size());
		currentColor = colors[currentIndex];
		return;
	}
	current = colors[currentIndex];
	next = (currentIndex + 1 < colors.size()) ? colors[currentIndex + 1] : colors[0];
	currentColor = current * (1 - factor) + next * factor;
}

void utils::ColorList::Add(const glm::vec3& color) {
	colors.push_back(color);
}

void utils::ColorList::Init(const std::vector<glm::vec3>& colorList) {
	colors = colorList;
	currentColor = colorList[0];
	currentIndex = 0;
	lastColorChange = (float)glfwGetTime();
}

const glm::vec3& utils::ColorList::GetCurrent() {
	return currentColor;
}
