#include "Plane.h"
#include <iostream>

// Genera los vértices para el plano
void Plane::generateVertices(float size, std::vector<float>& vertices) {
    float halfSize = size / 2.0f;

    vertices = {
        // Posiciones
        -halfSize, 0.0f, -halfSize,
         halfSize, 0.0f, -halfSize,
         halfSize, 0.0f,  halfSize,

         halfSize, 0.0f,  halfSize,
        -halfSize, 0.0f,  halfSize,
        -halfSize, 0.0f, -halfSize
    };
}

// Constructor del plano
Plane::Plane(float size) {
    std::vector<float> vertices;
    generateVertices(size, vertices);

    // Almaceno la cantidad de vértices
    this->numVertices = vertices.size() / 3;

    // Generamos VAO/VBO
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    // Defino el VAO creado como activo
    glBindVertexArray(this->VAO);

    // Defino el VBO de las posiciones como activo, le paso los datos y lo configuro
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Activamos el atributo 0 (posiciones por defecto)
    glEnableVertexAttribArray(0);

    // Desvinculamos VAO y VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Destructor del plano
Plane::~Plane() {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

// Método para renderizar el plano
void Plane::Render() const {
    // Vinculo su VAO para ser usado
    glBindVertexArray(this->VAO);

    // Dibujamos
    glDrawArrays(GL_TRIANGLES, 0, this->numVertices);

    // Desvinculamos VAO
    glBindVertexArray(0);
}