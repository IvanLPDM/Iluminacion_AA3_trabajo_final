#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 uvsGeometryShader[];
in vec3 normalsGeometryShader[];
in vec4 fragPosition[]; 
out vec2 uvsFragmentShader;
out vec3 normalsFragmentShader;
out vec4 primitivePosition;

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
        gl_Position = gl_in[i].gl_Position; 
        uvsFragmentShader = uvsGeometryShader[i];
        normalsFragmentShader = normalsGeometryShader[i];
        primitivePosition = fragPosition[i];
        EmitVertex();
    }
    EndPrimitive();
}