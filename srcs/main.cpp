#include "particle_system.hpp"

#define NB_PARTICLE 1000000

std::string glStringEnum(GLenum glenum)
{
    std::string enumString = "NONE";

    switch (glenum)
    {
    case GL_INVALID_ENUM:                   enumString = "INVALID_ENUM"; break;
    case GL_INVALID_VALUE:                  enumString = "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:              enumString = "INVALID_OPERATION"; break;
    case GL_OUT_OF_MEMORY:                  enumString = "OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:  enumString = "INVALID_FRAMEBUFFER_OPERATION"; break;

    case GL_DEBUG_SEVERITY_MEDIUM:          enumString = "GL_DEBUG_SEVERITY_MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH:            enumString = "GL_DEBUG_SEVERITY_HIGH"; break;
    case GL_DEBUG_SEVERITY_LOW:             enumString = "GL_DEBUG_SEVERITY_LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:    enumString = "GL_DEBUG_SEVERITY_NOTIFICATION"; break;

    case GL_DEBUG_SOURCE_API:               enumString = "GL_DEBUG_SOURCE_API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     enumString = "GL_DEBUG_SOURCE_WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   enumString = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:       enumString = "GL_DEBUG_SOURCE_THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION:       enumString = "GL_DEBUG_SOURCE_APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER:             enumString = "GL_DEBUG_SOURCE_OTHER"; break;

    case GL_DEBUG_TYPE_ERROR:               enumString = "GL_DEBUG_TYPE_ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: enumString = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  enumString = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         enumString = "GL_DEBUG_TYPE_PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         enumString = "GL_DEBUG_TYPE_PERFORMANCE"; break;
    case GL_DEBUG_TYPE_MARKER:              enumString = "GL_DEBUG_TYPE_MARKER"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          enumString = "GL_DEBUG_TYPE_PUSH_GROUP"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           enumString = "GL_DEBUG_TYPE_POP_GROUP"; break;
    case GL_DEBUG_TYPE_OTHER:               enumString = "GL_DEBUG_TYPE_OTHER"; break;
    }
    return enumString;
}



static void debug(void) {
    std::cout << glGetString(GL_VERSION) << std::endl;
    glfwSetErrorCallback([](int error, const char* description)
        {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        });
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        (void)id;
    (void)userParam;
    (void)length;
    if (type != GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR && (severity == GL_DEBUG_SEVERITY_HIGH || type == GL_DEBUG_TYPE_ERROR))
        std::cerr << "Opengl : " << " severity : " << glStringEnum(severity) << " type : " << glStringEnum(type) << " source : " << glStringEnum(source) << std::endl << "message : " << message << std::endl << std::endl;
        }, 0);
}


int main(int ac, char **av) {
	ParticleSystem particleSystem;
    uint32_t nbParticles = NB_PARTICLE;

    if (ac > 1) {
        if (atoi(av[1]) <=0)
            std::cout << "Invalid Argument : particles must be more than 0" << std::endl;
        else
            nbParticles = atoi(av[1]);
    }
    if (nbParticles % 32)
        nbParticles = nbParticles - (nbParticles % 32) + 32;
    std::cout << "Alligned particles number is " << nbParticles << std::endl;
    debug();
	particleSystem.Start(nbParticles);
	particleSystem.Run();
	particleSystem.Stop();

	return 0;
}