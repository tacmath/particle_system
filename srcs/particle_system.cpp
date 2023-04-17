#include "particle_system.hpp"

void ParticleSystem::Start()
{
	info.SetCenter(glm::vec3(0));
	info.hasGravity = false;

	camera.Init(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, glm::vec3(0, 0, -2));
	colors.Init({ {1, 0, 0}, {1, 1, 0}, {0, 1, 1}, {0, 0, 1} });

	InitGl();
	particles.Init(NB_PARTICLE, particlesPos);
	SetGlfwCallbacks();
	SetEventCallbacks();
}

void ParticleSystem::Run()
{
	isRunning = true;
//	glfwSwapInterval(0);
	while (isRunning) {

		GetEvents();

		particles.UpdateInfo(info);
		particles.Compute();

		colors.Update();
		shader.setVec3("baseColor", colors.GetCurrent());

		shader.Activate();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, NB_PARTICLE);

		utils::showFPS(window.context);

		glfwSwapBuffers(window.context);
	}
}

void ParticleSystem::Stop()
{
	particles.Stop();
	particlesPos.Delete();
	shader.Delete();
}

void ParticleSystem::InitGl()
{
	particlesPos.Gen(0, sizeof(GLfloat) * 3 * NB_PARTICLE);

	utils::InitParticles(particlesPos, NB_PARTICLE, isSphere);

	vao.Gen();
	vao.LinkAttrib(particlesPos, 0, 3, GL_FLOAT, sizeof(GLfloat), 0);
	shader.Load("shaders/particleVS.glsl", "shaders/particleFS.glsl");
	shader.setMat4("VP", camera.projection * camera.view);
	shader.Activate();
	vao.Bind();
	glEnable(GL_DEPTH_TEST);
//	glPointSize(1.5);
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
			particles.ResetVelocity();

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
	
	if (velocity != glm::vec3(0)) {
		velocity *= (glfwGetKey(window.context, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 0.1f : 0.01f;

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
		info.SetCenter(point);
		shader.setVec3("center", point);
	}
}
