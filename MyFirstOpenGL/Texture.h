#pragma once
#include <GL/glew.h>
#include <vector>
#include <stb_image.h>

class Texture {
private:
	int width, height, nrChannels;
	unsigned char* imageData;
	GLuint textureID;

public:
	Texture();

	Texture(const char* id);

	void CreateTexture(Texture texture);

	void LoadTexture();

	void GetCroma(float r, float g, float b, GLuint compiledPrograms);

	GLuint GetTextureID();

};