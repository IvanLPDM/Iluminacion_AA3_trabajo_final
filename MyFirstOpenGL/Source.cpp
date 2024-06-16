#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stb_image.h>
#include "Model.h"
#include <chrono>
#include <random>
#include "GameObject.h"
#include "Texture.h"
#include "Plane.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480



std::vector<GLuint> compiledPrograms;
std::vector<Model> models;



struct SpawnPoint {
	glm::vec3 position;
	glm::vec3 rotation; 
	glm::vec3 scale;
	ObjectType type;
};

//Hecho por ia---
float randomFloat(float min, float max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(min, max);
	return static_cast<float>(dis(gen));
}
//---------------

std::vector<SpawnPoint> generateRandomSpawnPoints(int numPoints) {
	std::vector<SpawnPoint> spawnPoints;

	int j;

	for (int i = 0; i < numPoints; ++i) {
		SpawnPoint point;

		j = ((int)randomFloat(0, 2));

		if (j == 1)
		{
			//troll
			point.position = glm::vec3(randomFloat(-2.0f, 2.0f), 0.0f, randomFloat(-2.0f, 2.0f));
			point.rotation = glm::vec3(randomFloat(0.0f, 10.0f), randomFloat(0.0f, 10.0f), 90);
			point.scale = glm::vec3(randomFloat(0.1f, 0.3f));
			point.type = ObjectType::TROLL;
		}
		else
		{
			//rock
			point.position = glm::vec3(randomFloat(-3.0f, 3.0f), 0.0f, randomFloat(-3.0f, 3.0f));
			point.rotation = glm::vec3((0.0f, 360.0f), randomFloat(0.0f, 360.0f), randomFloat(0.0f, 360.0f));
			point.scale = glm::vec3(randomFloat(0.1f, 1.0f));
			point.type = ObjectType::ROCK;
		}


		spawnPoints.push_back(point);
	}

	return spawnPoints;
}

struct Camera {
	float fFov = 45.f;
	float fNear = 0.1;
	float fFar = 10.f;

	float orbitAngle = 0.0f; 
	float orbitRadius = 2.0f; 
	float orbitVelocity = 1;
	float orbitVelocity_2 = 1.0f; 

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float deltaTime = 0.0f; 
	float lastFrame = 0.0f; 

	bool firstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = WINDOW_WIDTH / 2.0;
	float lastY = WINDOW_HEIGHT / 2.0;
	float fov = 45.0f;
	float mouseSensitivity = 0.1f;
	float cameraSpeed = 0.001f; 
	float cameraInitialSpeed = cameraSpeed;
	float maxDistance = 3.0f;

	//Player
	bool flashlightOn;
	float innerConeAngle;
	float outerConeAngle;
};
Camera camera;

class Light {
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	float outerConeAngle;
	float innerConeAngle;
	float maxDistance;

	Light()
	{
		color = { 1.0f, 1.0f, 1.0f };
	}
	Light(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float outerConeAngle, float innerConeAngle, float maxDistance)
		: position(position), direction(direction), color(color), outerConeAngle(outerConeAngle), innerConeAngle(innerConeAngle), maxDistance(maxDistance) {}

	void setUniforms(GLuint shaderProgram) {
		glUniform3fv(glGetUniformLocation(shaderProgram, "flashlightPosition"), 1, glm::value_ptr(position));
		glUniform3fv(glGetUniformLocation(shaderProgram, "flashlightDirection"), 1, glm::value_ptr(direction));
		glUniform3fv(glGetUniformLocation(shaderProgram, "flashlightColor"), 1, glm::value_ptr(color));
		glUniform1f(glGetUniformLocation(shaderProgram, "outerConeAngle"), glm::cos(glm::radians(outerConeAngle)));
		glUniform1f(glGetUniformLocation(shaderProgram, "innerConeAngle"), glm::cos(glm::radians(innerConeAngle)));
		glUniform1f(glGetUniformLocation(shaderProgram, "maxDistance"), maxDistance);
	}

	void UpdateFlashlight(Camera& camera) {
		position = camera.cameraPos;
		direction = camera.cameraFront;
	}
};

//Inputs
void processInput(GLFWwindow* window) {
	float currentFrame = glfwGetTime();
	camera.deltaTime = currentFrame - camera.lastFrame;
	camera.lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.cameraPos += camera.cameraSpeed * camera.cameraFront;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.cameraPos -= camera.cameraSpeed * camera.cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.cameraPos -= glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp)) * camera.cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.cameraPos += glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp)) * camera.cameraSpeed;


	static bool flashlightKeyPressed = false;
	static bool speedPressed = false;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !flashlightKeyPressed) {
		std::cout << "F PRESS" << std::endl;
		camera.flashlightOn = !camera.flashlightOn;
		flashlightKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		flashlightKeyPressed = false; // Reiniciar la variable booleana cuando se suelta la tecla F
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !speedPressed) {
		camera.cameraSpeed = 0.01f;
		speedPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		camera.cameraSpeed = camera.cameraInitialSpeed;
		speedPressed = false; 
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (camera.firstMouse) {
		camera.lastX = xpos;
		camera.lastY = ypos;
		camera.firstMouse = false;
	}

	float xoffset = xpos - camera.lastX;
	float yoffset = camera.lastY - ypos; 
	camera.lastX = xpos;
	camera.lastY = ypos;

	xoffset *= camera.mouseSensitivity;
	yoffset *= camera.mouseSensitivity;

	camera.yaw += xoffset;
	camera.pitch += yoffset;

	if (camera.pitch > 89.0f)
		camera.pitch = 89.0f;
	if (camera.pitch < -89.0f)
		camera.pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	front.y = sin(glm::radians(camera.pitch));
	front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	camera.cameraFront = glm::normalize(front);
}



struct ShaderProgram {
	GLuint vertexShader = 0;
	GLuint geometryShader = 0;
	GLuint fragmentShader = 0;
};



void Resize_Window(GLFWwindow* window, int iFrameBufferWidth, int iFrameBufferHeight) {

	//Definir nuevo tama�o del viewport
	glViewport(0, 0, iFrameBufferWidth, iFrameBufferHeight);
	glUniform2f(glGetUniformLocation(compiledPrograms[0], "windowSize"), iFrameBufferWidth, iFrameBufferHeight);
}


//Funcion que leera un .obj y devolvera un modelo para poder ser renderizado
Model LoadOBJModel(const std::string& filePath) {

	//Verifico archivo y si no puedo abrirlo cierro aplicativo
	std::ifstream file(filePath);

	if (!file.is_open()) {
		std::cerr << "No se ha podido abrir el archivo: " << filePath << std::endl;
		std::exit(EXIT_FAILURE);
	}

	//Variables lectura fichero
	std::string line;
	std::stringstream ss;
	std::string prefix;
	glm::vec3 tmpVec3;
	glm::vec2 tmpVec2;

	//Variables elemento modelo
	std::vector<float> vertexs;
	std::vector<float> vertexNormal;
	std::vector<float> textureCoordinates;

	//Variables temporales para algoritmos de sort
	std::vector<float> tmpVertexs;
	std::vector<float> tmpNormals;
	std::vector<float> tmpTextureCoordinates;

	//Recorremos archivo linea por linea
	while (std::getline(file, line)) {

		//Por cada linea reviso el prefijo del archivo que me indica que estoy analizando
		ss.clear();
		ss.str(line);
		ss >> prefix;

		//Estoy leyendo un vertice
		if (prefix == "v") {

			//Asumo que solo trabajo 3D as� que almaceno XYZ de forma consecutiva
			ss >> tmpVec3.x >> tmpVec3.y >> tmpVec3.z;

			//Almaceno en mi vector de vertices los valores
			tmpVertexs.push_back(tmpVec3.x);
			tmpVertexs.push_back(tmpVec3.y);
			tmpVertexs.push_back(tmpVec3.z);
		}

		//Estoy leyendo una UV (texture coordinate)
		else if (prefix == "vt") {

			//Las UVs son siempre imagenes 2D asi que uso el tmpvec2 para almacenarlas
			ss >> tmpVec2.x >> tmpVec2.y;

			//Almaceno en mi vector temporal las UVs
			tmpTextureCoordinates.push_back(tmpVec2.x);
			tmpTextureCoordinates.push_back(tmpVec2.y);

		}

		//Estoy leyendo una normal
		else if (prefix == "vn") {

			//Asumo que solo trabajo 3D as� que almaceno XYZ de forma consecutiva
			ss >> tmpVec3.x >> tmpVec3.y >> tmpVec3.z;

			//Almaceno en mi vector temporal de normales las normales
			tmpNormals.push_back(tmpVec3.x);
			tmpNormals.push_back(tmpVec3.y);
			tmpNormals.push_back(tmpVec3.z);

		}

		//Estoy leyendo una cara
		else if (prefix == "f") {

			int vertexData;
			short counter = 0;

			//Obtengo todos los valores hasta un espacio
			while (ss >> vertexData) {

				//En orden cada numero sigue el patron de vertice/uv/normal
				switch (counter) {
				case 0:
					//Si es un vertice lo almaceno - 1 por el offset y almaceno dos seguidos al ser un vec3, salto 1 / y aumento el contador en 1
					vertexs.push_back(tmpVertexs[(vertexData - 1) * 3]);
					vertexs.push_back(tmpVertexs[((vertexData - 1) * 3) + 1]);
					vertexs.push_back(tmpVertexs[((vertexData - 1) * 3) + 2]);
					ss.ignore(1, '/');
					counter++;
					break;
				case 1:
					//Si es un uv lo almaceno - 1 por el offset y almaceno dos seguidos al ser un vec2, salto 1 / y aumento el contador en 1
					textureCoordinates.push_back(tmpTextureCoordinates[(vertexData - 1) * 2]);
					textureCoordinates.push_back(tmpTextureCoordinates[((vertexData - 1) * 2) + 1]);
					ss.ignore(1, '/');
					counter++;
					break;
				case 2:
					//Si es una normal la almaceno - 1 por el offset y almaceno tres seguidos al ser un vec3, salto 1 / y reinicio
					vertexNormal.push_back(tmpNormals[(vertexData - 1) * 3]);
					vertexNormal.push_back(tmpNormals[((vertexData - 1) * 3) + 1]);
					vertexNormal.push_back(tmpNormals[((vertexData - 1) * 3) + 2]);
					counter = 0;
					break;
				}
			}
		}
	}
	return Model(vertexs, textureCoordinates, vertexNormal);
}


//Funcion que devolvera una string con todo el archivo leido
std::string Load_File(const std::string& filePath) {

	std::ifstream file(filePath);

	std::string fileContent;
	std::string line;

	//Lanzamos error si el archivo no se ha podido abrir
	if (!file.is_open()) {
		std::cerr << "No se ha podido abrir el archivo: " << filePath << std::endl;
		std::exit(EXIT_FAILURE);
	}

	//Leemos el contenido y lo volcamos a la variable auxiliar
	while (std::getline(file, line)) {
		fileContent += line + "\n";
	}

	//Cerramos stream de datos y devolvemos contenido
	file.close();

	return fileContent;
}

GLuint LoadFragmentShader(const std::string& filePath) {

	// Crear un fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Usamos la funcion creada para leer el fragment shader y almacenarlo 
	std::string sShaderCode = Load_File(filePath);
	const char* cShaderSource = sShaderCode.c_str();

	//Vinculamos el fragment shader con su c�digo fuente
	glShaderSource(fragmentShader, 1, &cShaderSource, nullptr);

	// Compilar el fragment shader
	glCompileShader(fragmentShader);

	// Verificar errores de compilaci�n
	GLint success;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	//Si la compilacion ha sido exitosa devolvemos el fragment shader
	if (success) {

		return fragmentShader;

	}
	else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

		//Obtenemos el log
		std::vector<GLchar> errorLog(logLength);
		glGetShaderInfoLog(fragmentShader, logLength, nullptr, errorLog.data());

		//Mostramos el log y finalizamos programa
		std::cerr << "Se ha producido un error al cargar el fragment shader:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}


GLuint LoadGeometryShader(const std::string& filePath) {

	// Crear un vertex shader
	GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

	//Usamos la funcion creada para leer el vertex shader y almacenarlo 
	std::string sShaderCode = Load_File(filePath);
	const char* cShaderSource = sShaderCode.c_str();

	//Vinculamos el vertex shader con su c�digo fuente
	glShaderSource(geometryShader, 1, &cShaderSource, nullptr);

	// Compilar el vertex shader
	glCompileShader(geometryShader);

	// Verificar errores de compilaci�n
	GLint success;
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

	//Si la compilacion ha sido exitosa devolvemos el vertex shader
	if (success) {

		return geometryShader;

	}
	else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &logLength);

		//Obtenemos el log
		std::vector<GLchar> errorLog(logLength);
		glGetShaderInfoLog(geometryShader, logLength, nullptr, errorLog.data());

		//Mostramos el log y finalizamos programa
		std::cerr << "Se ha producido un error al cargar el vertex shader:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

GLuint LoadVertexShader(const std::string& filePath) {

	// Crear un vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Usamos la funcion creada para leer el vertex shader y almacenarlo 
	std::string sShaderCode = Load_File(filePath);
	const char* cShaderSource = sShaderCode.c_str();

	//Vinculamos el vertex shader con su c�digo fuente
	glShaderSource(vertexShader, 1, &cShaderSource, nullptr);

	// Compilar el vertex shader
	glCompileShader(vertexShader);

	// Verificar errores de compilaci�n
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	//Si la compilacion ha sido exitosa devolvemos el vertex shader
	if (success) {

		return vertexShader;

	}
	else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

		//Obtenemos el log
		std::vector<GLchar> errorLog(logLength);
		glGetShaderInfoLog(vertexShader, logLength, nullptr, errorLog.data());

		//Mostramos el log y finalizamos programa
		std::cerr << "Se ha producido un error al cargar el vertex shader:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

//Funci�n que dado un struct que contiene los shaders de un programa generara el programa entero de la GPU
GLuint CreateProgram(const ShaderProgram& shaders) {

	//Crear programa de la GPU
	GLuint program = glCreateProgram();

	//Verificar que existe un vertex shader y adjuntarlo al programa
	if (shaders.vertexShader != 0) {
		glAttachShader(program, shaders.vertexShader);
	}

	if (shaders.geometryShader != 0) {
		glAttachShader(program, shaders.geometryShader);
	}

	if (shaders.fragmentShader != 0) {
		glAttachShader(program, shaders.fragmentShader);
	}

	// Linkear el programa
	glLinkProgram(program);

	//Obtener estado del programa
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	//Devolver programa si todo es correcto o mostrar log en caso de error
	if (success) {

		//Liberamos recursos
		if (shaders.vertexShader != 0) {
			glDetachShader(program, shaders.vertexShader);
		}

		//Liberamos recursos
		if (shaders.geometryShader != 0) {
			glDetachShader(program, shaders.geometryShader);
		}

		//Liberamos recursos
		if (shaders.fragmentShader != 0) {
			glDetachShader(program, shaders.fragmentShader);
		}

		return program;
	}
	else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		//Almacenamos log
		std::vector<GLchar> errorLog(logLength);
		glGetProgramInfoLog(program, logLength, nullptr, errorLog.data());

		std::cerr << "Error al linkar el programa:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}


void main() {

	//Definir semillas del rand seg�n el tiempo
	srand(static_cast<unsigned int>(time(NULL)));

	//Inicializamos GLFW para gestionar ventanas e inputs
	glfwInit();


	//Configuramos la ventana
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24); // Aseguramos un depth buffer de 24 bits

	//Inicializamos la ventana
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Engine", NULL, NULL);

	//Asignamos funci�n de callback para cuando el frame buffer es modificado
	glfwSetFramebufferSizeCallback(window, Resize_Window);

	//Definimos espacio de trabajo
	glfwMakeContextCurrent(window);

	// Desactivar el cursor y capturar el movimiento del rat�n
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Configurar los callbacks
	glfwSetCursorPosCallback(window, mouse_callback);

	//Permitimos a GLEW usar funcionalidades experimentales
	glewExperimental = GL_TRUE;

	//Activamos cull face
	glEnable(GL_CULL_FACE);

	//Indicamos lado del culling
	glCullFace(GL_BACK);

	//Leer textura
	Texture trollTexture("Assets/Textures/troll_v2.png");
	Texture rockTexture("Assets/Textures/rock_v2.png");
	Texture sunTexture("Assets/Textures/Cube_Texture.png");
	Texture moonTexture("Assets/Textures/moon.png");

	//Para los fps
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

	//Inicializamos GLEW y controlamos errores
	if (glewInit() == GLEW_OK) {


		// Habilitamos el depth test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Habilitamos cull face
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);


		glm::vec3 lookAt;

		//Compilar shaders
		ShaderProgram myFirstProgram;
		myFirstProgram.vertexShader = LoadVertexShader("MyFirstVertexShader.glsl");
		myFirstProgram.geometryShader = LoadGeometryShader("MyFirstGeometryShader.glsl");
		myFirstProgram.fragmentShader = LoadFragmentShader("MyFirstFragmentShader.glsl");

		//Cargo Modelo
		models.push_back(LoadOBJModel("Assets/Models/troll.obj"));
		models.push_back(LoadOBJModel("Assets/Models/rock.obj"));
		models.push_back(LoadOBJModel("Assets/Models/ball.obj"));

		//Compilar programa
		compiledPrograms.push_back(CreateProgram(myFirstProgram));

		GameObject sun(255, 0, 0, glm::vec3(0.f, 10.0f, 0.f), glm::vec3(180.f, 90.f, 0.f), glm::vec3(0.01f, 0.01f, 0.01f), sunTexture);
		GameObject moon(255, 255, 255, glm::vec3(0.f, 10.0f, 0.f), glm::vec3(180.f, 90.f, 0.f), glm::vec3(0.01f, 0.01f, 0.01f), moonTexture);
		GameObject planet(1, 1, 1, glm::vec3(0.f, -1.0f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1, 1, 1), moonTexture);
		GameObject nave(1, 1, 1, glm::vec3(0.f, 0.0f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.f, 100.f, 100.f), moonTexture);

		int numItems = 20;

		// Generar puntos de spawn aleatorios
		std::vector<SpawnPoint> spawnPoints = generateRandomSpawnPoints(numItems);

		std::vector<GameObject> items;
		int i = 0;

		for (const auto& spawnPoint : spawnPoints) 
		{
			items.emplace_back(1.0f, 1.0f, 1.0f, 
				spawnPoint.position, spawnPoint.rotation, spawnPoint.scale, trollTexture, spawnPoint.type);
		}

		Light lightSun;
		
		Light Linterna(
			camera.cameraPos, 
			camera.cameraFront, 
			glm::vec3(1.0f, 1.0f, 1.0f), 
			12.5f, 
			17.5f, 
			300.0f 
		);

		camera.flashlightOn = false;
		camera.innerConeAngle = 5.5f;
		camera.outerConeAngle = 7.5f;

		moon.angle = 160;

		//LOAD TEXTURE
		trollTexture.LoadTexture();
		rockTexture.LoadTexture();
		sunTexture.LoadTexture();
		moonTexture.LoadTexture();

		//Definimos color para limpiar el buffer de color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Definimos modo de dibujo para cada cara
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Indicar a la tarjeta GPU que programa debe usar
		glUseProgram(compiledPrograms[0]);

		//Asignar valores iniciales al programa
		glUniform2f(glGetUniformLocation(compiledPrograms[0], "windowSize"), WINDOW_WIDTH, WINDOW_HEIGHT);

		//Asignar valor variable de textura a usar
		glUniform1d(glGetUniformLocation(compiledPrograms[0], "textureSampler"), 0);
		//Generamos el game loop

		while (!glfwWindowShouldClose(window)) {

			currentTime = std::chrono::high_resolution_clock::now();
			deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

			processInput(window);

			//Movimiento sol
			sun.angle += sun.orbitSpeed * deltaTime;

			if (sun.angle >= 360.0f) {
				sun.angle -= 360.0f;
			}

			sun.position.y = sun.radius * sin(sun.angle);
			sun.position.z = sun.radius * cos(sun.angle);

			lightSun.position = sun.position;

			//Movimiento luna
			moon.angle += moon.orbitSpeed * deltaTime;

			moon.position.y = moon.radius * sin(moon.angle);
			moon.position.z = moon.radius * cos(moon.angle);

			//Pulleamos los eventos (botones, teclas, mouse...)
			glfwPollEvents();

			if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {

				camera.fFov += 1.0f;

				if (camera.fFov > 180.f) {
					camera.fFov = 180;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {

				camera.fFov -= 1.0f;

				if (camera.fFov < 1.f) {
					camera.fFov = 1;
				}
			}

			sun.preCarga();
			moon.preCarga();
			planet.preCarga();
			nave.preCarga();

			for (int i = 0; i < numItems; i++)
			{
				items[i].preCarga();
			}

			glm::mat4 viewMatrix = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
			glUniformMatrix4fv(glGetUniformLocation(compiledPrograms[0], "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

			glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, camera.fNear, camera.fFar);
			glUniformMatrix4fv(glGetUniformLocation(compiledPrograms[0], "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

			glUniformMatrix4fv(glGetUniformLocation(compiledPrograms[0], "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(glGetUniformLocation(compiledPrograms[0], "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

			GLint lightDirLocation = glGetUniformLocation(compiledPrograms[0], "lightDirection");
			glUniform3f(lightDirLocation, lightSun.position.x, lightSun.position.y, lightSun.position.z);
			glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

			// Calcular el frente de la cámara basado en los ángulos de la cámara
			camera.cameraFront.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
			camera.cameraFront.y = sin(glm::radians(camera.pitch));
			camera.cameraFront.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
			camera.cameraFront = glm::normalize(camera.cameraFront);


			glUseProgram(compiledPrograms[0]);
			GLuint shaderProgram = compiledPrograms[0];
			GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPosition");
			GLuint moonPosLoc = glGetUniformLocation(shaderProgram, "moonPosition");
			GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPosition");
			GLuint cameraFt = glGetUniformLocation(shaderProgram, "cameraFront");
			GLuint flashlightOnLoc = glGetUniformLocation(shaderProgram, "flashlightOn");

			GLuint innerConeAngleLoc = glGetUniformLocation(shaderProgram, "innerConeAngle");
			GLuint outerConeAngleLoc = glGetUniformLocation(shaderProgram, "outerConeAngle");

			GLuint maxDistanceLoc = glGetUniformLocation(shaderProgram, "maxDistance");

			Linterna.UpdateFlashlight(camera);
			Linterna.setUniforms(compiledPrograms[0]);

			glUniform3f(lightPosLoc, sun.position.x, sun.position.y, sun.position.z);
			glUniform3f(moonPosLoc, moon.position.x, moon.position.y, moon.position.z);
			glUniform3f(cameraPosLoc, camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
			glUniform3f(cameraFt, camera.cameraFront.x, camera.cameraFront.y, camera.cameraFront.z);
			glUniform1i(flashlightOnLoc, camera.flashlightOn ? 1 : 0);

			glUniform1f(outerConeAngleLoc, camera.outerConeAngle);
			glUniform1f(innerConeAngleLoc, camera.innerConeAngle);

			glUniform1f(maxDistanceLoc, camera.maxDistance);





			//Limpiamos los buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


			sun.Render(sunTexture, compiledPrograms[0]);
			models[2].Render();

			moon.Render(moonTexture, compiledPrograms[0]);
			models[2].Render();	

			planet.Render(moonTexture, compiledPrograms[0]);
			models[2].Render();


			for (int i = 0; i < numItems; i++)
			{
				

				if (items[i].typeOb == ObjectType::TROLL)
				{
					items[i].Render(trollTexture, compiledPrograms[0]);
					models[0].Render();
				}
				else if (items[i].typeOb == ObjectType::ROCK)
				{
					items[i].Render(rockTexture, compiledPrograms[0]);
					models[1].Render();
				}
			}
			

			// Guardar el tiempo actual para el pr�ximo fotograma
			lastTime = currentTime;

			//Cambiamos buffers
			glFlush();
			glfwSwapBuffers(window);
		}

		//Desactivar y eliminar programa
		glUseProgram(0);
		glDeleteProgram(compiledPrograms[0]);

	}
	else {
		std::cout << "Ha petao." << std::endl;
		glfwTerminate();
	}

	//Finalizamos GLFW
	glfwTerminate();

}