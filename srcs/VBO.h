#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include <cstddef>

class VBO //maybe rename by NBO (named buffer Object) ou BO ou BufferObject
{
public:
    GLuint ID;

    VBO() = default;
    VBO(const void* data, size_t size);
    void Gen(const void* data, size_t size, GLuint usage = GL_STATIC_DRAW);
    void* Map(GLenum access);
    void Unmap();
    void Delete();
};

#endif