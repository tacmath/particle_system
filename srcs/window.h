#ifndef WINDOW_H
#define WINDOW_H

#ifdef _WIN32
#include <direct.h>
// MSDN recommends against using getcwd & chdir names
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

//#define DEBUG_MODE

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#include <glm/vec2.hpp>

# define DEFAULT_WINDOW_WIDTH  1700
# define DEFAULT_WINDOW_HEIGHT  1080

class MainWindow {
private:
    GLFWmonitor *monitor;
public:
	// the window context
	GLFWwindow*     context;

	MainWindow() {
        char path[1024];

        cwd(path, 1024);
        if (!glfwInit()) {
            std::cerr << "Glfw failed to init\n";
            exit(-1);
        }
        cd(path);
        //    glfwWindowHint(GLFW_SAMPLES, 4);      // no multisampling
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
        if (!(context = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Particle System", NULL, NULL))) {
            glfwTerminate();
            std::cerr << "Glfw failed to create a window\n";
            exit(-1);
        }
        glfwMakeContextCurrent(context);

        gladLoadGL();
        void SetWiewPort();
        glfwSetCursorPos(context, DEFAULT_WINDOW_WIDTH / 2.0, DEFAULT_WINDOW_HEIGHT / 2.0);
        glfwSetInputMode(context, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        monitor = glfwGetPrimaryMonitor();
	}

    void FullScreen(void) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(context, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    void Windowed(void) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(context, nullptr, 0, 0, (int)DEFAULT_WINDOW_WIDTH, (int)DEFAULT_WINDOW_HEIGHT, mode->refreshRate);
    }

    void SetWiewPort(void) {
        int width, height;
        glfwGetFramebufferSize(context, &width, &height);
        glViewport(0, 0, width, height);
    }

    ~MainWindow() {
        glfwDestroyWindow(context);
        glfwTerminate();
    }
};

#endif