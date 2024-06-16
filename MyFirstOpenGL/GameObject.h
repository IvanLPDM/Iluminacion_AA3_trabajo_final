#pragma once
#include "glm.hpp"
#include "Texture.h"
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "OperacionesMatriciales.h"

enum class ObjectType
{
	ROCK,
	TROLL
};

class GameObject {
public:

	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 rotation = glm::vec3(0.f);
	glm::vec3 scale = glm::vec3(1.f);
	float r, g, b;

	Texture texture;

	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 scaleMatrix;

	float angle = 0.0f;
	float radius = 7.0f;
	float orbitSpeed = 0.2f;

	ObjectType typeOb;

	GameObject(float r, float g, float b, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Texture _texture)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		

		texture.CreateTexture(_texture);
	}

	GameObject(float r, float g, float b, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Texture _texture, ObjectType _type)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		this->typeOb = _type;


		texture.CreateTexture(_texture);
	}

	void preCarga()
	{
		translationMatrix = GenerateTranslationMatrix(position);
		rotationMatrix = GenerateRotationMatrix(rotation, rotation.y);
		scaleMatrix = GenerateScaleMatrix(scale);
	}

	void Render(Texture _texture, GLuint compiledPrograms)
	{
		glUniformMatrix4fv(glGetUniformLocation(compiledPrograms, "translationMatrix"), 1, GL_FALSE, glm::value_ptr(translationMatrix));
		glUniformMatrix4fv(glGetUniformLocation(compiledPrograms, "rotationMatrix"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
		glUniformMatrix4fv(glGetUniformLocation(compiledPrograms, "scaleMatrix"), 1, GL_FALSE, glm::value_ptr(scaleMatrix));

		//Cambiar textura
		glBindTexture(GL_TEXTURE_2D, _texture.GetTextureID());
		//Croma
		_texture.GetCroma(r, g, b, compiledPrograms);
	}

	void ObjectLoadTexture()
	{
		texture.LoadTexture();
	}

	void setObjectType(ObjectType _type)
	{
		this->typeOb = _type;
	}

	

private:

};