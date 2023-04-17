#include "particle_system.hpp"

void ParticleSystem::Start(uint32_t nbParticles)
{
	this->nbParticles = nbParticles;

	info.SetCenter(glm::vec3(0));
	info.hasGravity = false;

	camera.Init(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, glm::vec3(0, 0, -2));
	colors.Init({ {1, 0, 0}, {1, 1, 0}, {0, 1, 1}, {0, 0, 1} });

	InitGl();
	InitImgui();
	particles.Init(nbParticles, particlesPos);
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
		glDrawArrays(GL_POINTS, 0, nbParticles);

		DrawMenu();
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
	particlesPos.Gen(0, sizeof(GLfloat) * 3 * nbParticles);

	utils::InitParticles(particlesPos, nbParticles, isSphere);

	vao.Gen();
	vao.LinkAttrib(particlesPos, 0, 3, GL_FLOAT, sizeof(GLfloat), 0);
	shader.Load("shaders/particleVS.glsl", "shaders/particleFS.glsl");
	shader.setMat4("VP", camera.projection * camera.view);
	shader.Activate();
	vao.Bind();
	glEnable(GL_DEPTH_TEST);
}

void ParticleSystem::InitImgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 6;
	style.SeparatorTextAlign.x = 0.5f;
	style.WindowTitleAlign.x = 0.5f;
	style.WindowMenuButtonPosition = ImGuiDir_Right;

	ImGui_ImplGlfw_InitForOpenGL(window.context, false);
	ImGui_ImplOpenGL3_Init("#version 460");
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

	ImGui_ImplGlfw_InstallCallbacks(window.context);
}

void ParticleSystem::SetEventCallbacks()
{
	callbacks.onMouseMouvement = [&](double mouseX, double mouseY){
		static double lastMouseX = DEFAULT_WINDOW_WIDTH / 2;
		static double lastMouseY = DEFAULT_WINDOW_HEIGHT / 2;

		if (!freeCursor && !inMenu) {
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
			utils::InitParticles(particlesPos, nbParticles, isSphere);
		}
		if (key == GLFW_KEY_V && action == GLFW_PRESS)
			particles.ResetVelocity();

		if (key == GLFW_KEY_LEFT_ALT) {
			if (action == GLFW_PRESS) {
				freeCursor = true;
				UpdateCursorMode();
			}
			else if (action == GLFW_RELEASE) {
				freeCursor = false;
				UpdateCursorMode();
			}
		}

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			inMenu = !inMenu;
			UpdateCursorMode();
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
	if (glfwWindowShouldClose(window.context) == 1)
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

void ParticleSystem::DrawMenu() {
	static bool resuming = false;

	if (!inMenu)
		return;

	if (resuming) {
		inMenu = false;
		UpdateCursorMode();
		resuming = false;
		return;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Menu", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	ImVec2 buttonSize;
	const char* label;

	buttonSize.x = ImGui::GetWindowWidth() - 2 * ImGui::GetStyle().WindowPadding.x;
	buttonSize.y = 0;

	if (ImGui::ColorEdit3("Center color", (float*)&options.centerColor))
		shader.setVec3("centerColor", options.centerColor);

	glm::vec3 center = info.GetCenter();
	if (ImGui::InputFloat3("Center Pos", (float*)&center)) {
		shader.setVec3("center", center);
		info.SetCenter(center);
	}

	ImGui::PushItemWidth(buttonSize.x);
	if (ImGui::SliderFloat("##center size", &options.centerSize, 0.0f, 10.0f, "Center Size %.2f"))
		shader.setFloat("centerSize", options.centerSize);

	if (ImGui::SliderFloat("##particle size", &options.particleSize, 1.0f, 5.0f, "Particle Size %.0f"))
		glPointSize(options.particleSize);

	ImGui::PopItemWidth();

	if (ImGui::Button("Reset Velocity", buttonSize))
		particles.ResetVelocity();

	label = (info.hasGravity) ? "Gravity On" : "Gravity Off";
	if (ImGui::Button(label, buttonSize))
		info.hasGravity = (info.hasGravity) ? false : true;

	label = (isSphere) ? "Sphere" : "Cube";
	if (ImGui::Button(label, buttonSize)) {
		isSphere = !isSphere;
		utils::InitParticles(particlesPos, nbParticles, isSphere);
	}

	if (ImGui::Button("Resume", buttonSize))
		resuming = true;

	if (ImGui::Button("Exit", buttonSize))
		isRunning = false;

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ParticleSystem::UpdateCursorMode() {
	static int currentMode = GLFW_CURSOR_DISABLED;

	if ((freeCursor || inMenu) && currentMode == GLFW_CURSOR_DISABLED) {
		int width, height;
		glfwSetInputMode(window.context, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwGetWindowSize(window.context, &width, &height);
		glfwSetCursorPos(window.context, width / 2.0, height / 2.0);
		callbacks.onMouseMouvement(width / 2.0, height / 2.0);
		currentMode = GLFW_CURSOR_NORMAL;
	}
	else if (!freeCursor && !inMenu) {
		glfwSetInputMode(window.context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		currentMode = GLFW_CURSOR_DISABLED;
	}
}