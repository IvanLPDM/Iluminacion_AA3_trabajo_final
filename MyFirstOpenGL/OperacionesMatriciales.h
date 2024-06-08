#pragma once
#include <gtc/matrix_transform.hpp>

glm::mat4 GenerateScaleMatrix(glm::vec3 scaleAxis) {

	return glm::scale(glm::mat4(1.0f), scaleAxis);
}

//Funcion que genera una matriz de rotacion dado un angulo y un vector
glm::mat4 GenerateRotationMatrix(glm::vec3 axis, float fDegrees) {

	return glm::rotate(glm::mat4(1.0f), glm::radians(fDegrees), glm::normalize(axis));
}

//Funcion que genera una matriz de traslacion representada por un vector
glm::mat4 GenerateTranslationMatrix(glm::vec3 translation) {

	return glm::translate(glm::mat4(1.0f), translation);
}