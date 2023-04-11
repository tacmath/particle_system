#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
    // Reference ID of the VAO
    GLuint ID;

    // Default constructor
    VAO() { ID = 0; };
    // Default destructor
    ~VAO() { glDeleteVertexArrays(1, &ID); };
    // Generates a VAO ID
    void Gen();
    // Links Attribute such as a position or color to the VAO
    void LinkAttrib(const void *data, GLuint vertexNumbers, GLuint layout, GLuint numComponents, GLuint type, GLuint typeSize, GLuint offset);
    // Links VBO Attribute such as a position or color to the VAO
    void LinkAttrib(VBO &vbo, GLuint layout, GLuint numComponents, GLuint type, GLuint typeSize, GLuint offset);
    // Binds the VAO
    void Bind();
    // Unbinds the VAO
    void Unbind();
    // Deletes the VAO
    void Delete();
};

inline void VAO::Bind() {
    glBindVertexArray(ID);
}

#endif