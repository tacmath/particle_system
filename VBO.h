#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include <cstddef>

class VBO
{
public:
    GLuint ID;

    VBO() = default;
    VBO(const void* data, size_t size);
    void Gen(const void* data, size_t size);
    void* Map(GLenum access);
    void Unmap();
    void Delete();
};

#endif