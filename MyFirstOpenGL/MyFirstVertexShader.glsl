#version 440 core

layout(location = 0) in vec3 posicion;
layout(location = 1) in vec2 uvsVertexShader;
layout(location = 2) in vec3 normalsVertexShader;

out vec2 uvsGeometryShader;
out vec3 normalsGeometryShader;
out vec4 fragPosition; 

uniform mat4 translationMatrix;
uniform mat4 rotationMatrix;
uniform mat4 scaleMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    uvsGeometryShader = uvsVertexShader;
    normalsGeometryShader = normalsVertexShader;

    mat4 model = translationMatrix * rotationMatrix * scaleMatrix;
    fragPosition = model * vec4(posicion, 1.0);

    gl_Position = projectionMatrix * viewMatrix * fragPosition;
}