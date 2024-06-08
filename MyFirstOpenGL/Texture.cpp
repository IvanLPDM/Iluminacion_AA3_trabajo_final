#include "Texture.h"

Texture::Texture()
{

}
Texture::Texture(const char* id)
{
	imageData = stbi_load(id, &width, &height, &nrChannels, 0);
}

void Texture::CreateTexture(Texture texture)
{
	const char* id = (const char*)texture.imageData;
	int _width = texture.width;
	int _height = texture.height;
	int _nrChannels = texture.nrChannels;


	imageData = stbi_load(id, &_width, &_height, &_nrChannels, 0);
}

void Texture::LoadTexture()
{
	//Definimos canal de textura activo
	glActiveTexture(GL_TEXTURE0);


	glGenTextures(1, &textureID);

	//Vinculamos texture
	glBindTexture(GL_TEXTURE_2D, textureID);


	//Cargar datos de la imagen de la textura
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	//Configuar textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//Generar mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	//Liberar memoria de la imagen cargada
	stbi_image_free(imageData);
}

void Texture::GetCroma(float r, float g, float b, GLuint compiledPrograms)
{
	//Cromas
	int valuePosition = glGetUniformLocation(compiledPrograms, "color");

	if (valuePosition != -1)
	{
		glUniform3f(valuePosition, r, g, b);
	}
	//else
		//std::cout << "No se ha podido encontrar la direccion" << std::endl;
}

GLuint Texture::GetTextureID()
{
	return textureID;
}