#include "VBO.h"

VBO::VBO(const void* data, size_t size) {
    Gen(data, size);
}

void VBO::Gen(const void* data, size_t size) {
    glCreateBuffers(1, &ID);
    glNamedBufferData(ID, size, data, GL_STATIC_DRAW);
}

void VBO::Delete() {
    glDeleteBuffers(1, &ID);
}