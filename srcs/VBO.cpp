#include "VBO.h"

VBO::VBO(const void* data, size_t size) {
    Gen(data, size);
}

void VBO::Gen(const void* data, size_t size, GLuint usage) {
    glCreateBuffers(1, &ID);
    glNamedBufferData(ID, size, data, usage);
}

void* VBO::Map(GLenum access) {
    return glMapNamedBuffer(ID, access);
}

void VBO::Unmap() {
    glUnmapNamedBuffer(ID);
}

void VBO::Delete() {
    glDeleteBuffers(1, &ID);
}