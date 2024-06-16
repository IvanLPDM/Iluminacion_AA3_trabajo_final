#pragma once
#include <vector>
#include <GL/glew.h>

class Plane {
public:
    Plane(float size);
    ~Plane();

    void Render() const;

private:
    GLuint VAO, VBO;
    size_t numVertices;

    void generateVertices(float size, std::vector<float>& vertices);
};