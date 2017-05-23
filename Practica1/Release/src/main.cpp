﻿#define GLEW_STATIC
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include "Shader.h"
#include "Camera.h"
#include "Object.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Instantiate with Release x86!

//Key detection functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void RenderQuad();

//Variables
const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;
GLfloat FOV = 45.0f;
float cameraSpeedV = 5.f;
bool keys[1024];
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool firstMouse = true;
GLint controls = 1;
int texture = 2;

//Camera variables to pass to view matrix
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

//Objects vectors to define in constructor
vec3 scaleCube = vec3(0.3f, 0.3f, 0.3f);
vec3 rotationCube = vec3(0.f, 45.f, 0.f);
vec3 positionCube = vec3(1.5f, -1.0f, -1.5f);

/*
vec3 scaleLamp = vec3(0.1f, 0.1f, 0.1f);
vec3 rotationLamp = vec3(0.f, 0.f, 0.f);
vec3 positionLamp = vec3(-1.0f, 1.5f, -1.0f);*/

vec3 scalePoint = vec3(0.1f, 0.1f, 0.1f);
vec3 rotationPoint= vec3(0.f, 0.f, 0.f);
vec3 positionPoint= vec3(1.0f, -0.5f, 0.5f);

vec3 scaleSpot= vec3(0.1f, 0.1f, 0.1f);
vec3 rotationSpot = vec3(0.f, 0.f, 0.f);
vec3 positionSpot = vec3(3.0f, 1.5f, -1.0f);

//Camera speed control variables (deltaTime)
GLfloat deltaTime = 0.0f;	//Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	//Time of last frame

int luzSeleccionada = 2;

//Function to detect whenever a key is pressed and which one is it
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){

	//If that checks if a key is pressed or released
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	//Closing the window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//Lamp and cube controls
	/*if (key == GLFW_KEY_1 && action == GLFW_PRESS && controls == 2)
		controls = 1;

	if (key == GLFW_KEY_2 && action == GLFW_PRESS && controls == 1)
		controls = 2;*/

	//Light selection
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) luzSeleccionada = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) luzSeleccionada = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) luzSeleccionada = 3;

	//Texture change
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) texture = 1;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS) texture = 2;
	if (key == GLFW_KEY_6 && action == GLFW_PRESS) texture = 3;
	
}

//Function that detects what is pressed and moves the camera alongside user movements
void do_movement() {

	GLfloat cameraSpeed = cameraSpeedV * deltaTime; //Camera speed calculus

	//Movement control
	if (keys[GLFW_KEY_W])camera.ProcessKeyboard(FORWARD, deltaTime); //Go forward

	if (keys[GLFW_KEY_S])camera.ProcessKeyboard(BACKWARD, deltaTime); //Go backward

	if (keys[GLFW_KEY_A])camera.ProcessKeyboard(LEFT, deltaTime); //Go left

	if (keys[GLFW_KEY_D])camera.ProcessKeyboard(RIGHT, deltaTime); //Go right

}

GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}


//Function that detects whenever a mouse button is pressed and the coordenates of the mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos){

	//Correct initialization of the mouse position for the first time
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//Offset calculations
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset); //Pass the offset calculations to the camera movement function
}

//Function that detects the mouse scroll to zoom in and out
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { camera.ProcessMouseScroll(yoffset/8); }

int main(){

	//GLFW Initialization
	glfwInit();

	//Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "A_ParallaxMapping_MarcMolina_AniolFolch", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	//Set callback functions and input mode
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Glew Initialization and activation of experimental glew
	glewExperimental = GL_TRUE;
	glewInit();

	//Viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);  //Z-buffer enabler

	//Shader declaration
	//Shader lightingShader = Shader("./src/multipleLightVertexShader.vertexshader", "./src/multipleLightFragmentShader.fragmentshader");
	Shader PointShader = Shader("./src/lampVertexShader.vertexshader", "./src/lampFragmentShader.fragmentshader");
	//Shader PointShader2 = Shader("./src/lampVertexShader.vertexshader", "./src/lampFragmentShader.fragmentshader");
	//Shader SpotShader = Shader("./src/lampVertexShader.vertexshader", "./src/lampFragmentShader.fragmentshader");
	Object Cube = Object(scaleCube, rotationCube, positionCube, Object::cube);
	//Object Lamp = Object(scaleLamp, rotationLamp, positionLamp, Object::cube);
	//Object DirectionalLight = Object(scaleDirectional, rotationDirectional, positionDirectional, Object::cube); no representem amb cub a les directional lights
	Object PointLight = Object(scalePoint, rotationPoint, positionPoint, Object::cube);
	//Object PointLight2 = Object(scalePoint2, rotationPoint2, positionPoint2, Object::cube);
	//Object SpotLight = Object(scaleSpot, rotationSpot, positionSpot, Object::cube);

	Shader shader("./src/parallax_mapping.vertexshader", "./src/parallax_mapping.fragmentshader");

	glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

	// Load textures
	GLuint diffuseMap = loadTexture("./src/bricks.jpg");
	GLuint normalMap = loadTexture("./src/bricks_normal.jpg");
	GLuint heightMap = loadTexture("./src/bricks_disp.jpg");

	GLuint diffuseMap2 = loadTexture("./src/wood.png");
	GLuint normalMap2 = loadTexture("./src/wood_normal.png");
	GLuint heightMap2 = loadTexture("./src/wood_relief.png");

	GLuint diffuseMap3 = loadTexture("./src/rockwall.png");
	GLuint normalMap3 = loadTexture("./src/rockwall_normal.png");
	GLuint heightMap3 = loadTexture("./src/rockwall_relief.png");

	// Set texture units 
	shader.USE();
	glUniform1i(glGetUniformLocation(shader.Program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(shader.Program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(shader.Program, "depthMap"), 2);

	Shader modelShader = Shader("./src/textureVertex3d.vertexshader", "./src/textureFragment3d.fragmentshader");
	Model ourModel("./src/ath/colchoneta.obj");

	while (!glfwWindowShouldClose(window)){

		//Deltatime calculation for each frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Events checker
		glfwPollEvents();
		do_movement();
		//if (controls == 1) { Cube.do_movement(window); }
		//else { 
			//Lamp.do_movement(window); 
		if (luzSeleccionada == 2) {
			PointLight.do_movement(window);
		}
		
		//}

		//Background color and z-buffer corrector
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		

		// Configure view/projection matrices
		shader.USE();
		glm::mat4 viewP = camera.GetViewMatrix();
		glm::mat4 projectionP = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewP));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionP));
		//Render normal-mapped quad
		glm::mat4 model;
		//	model = glm::rotate(model, (GLfloat)glfwGetTime() * 0, glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // Rotates the quad to show parallax mapping works in all directions
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &camera.Position[0]);
		glUniform1f(glGetUniformLocation(shader.Program, "height_scale"),0.05f);
		glUniform1i(glGetUniformLocation(shader.Program, "parallax"), true);
		glUniform1i(glGetUniformLocation(shader.Program, "luzSeleccionada"), luzSeleccionada);
		glActiveTexture(GL_TEXTURE0);

		switch (texture) {

		case 1:
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, heightMap);
			break;
		case 2:
			glBindTexture(GL_TEXTURE_2D, diffuseMap2);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, heightMap2);
			break;
		case 3:
			glBindTexture(GL_TEXTURE_2D, diffuseMap3);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap3);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, heightMap3);
			break;
		}

		RenderQuad();

		// Set material properties
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32.0f);

		// Directional light
		if (luzSeleccionada == 1) {
			glUniform3f(glGetUniformLocation(shader.Program, "dirLight.direction"), 0.0f, 0.0f, -1.f);
			glUniform3f(glGetUniformLocation(shader.Program, "dirLight.ambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(shader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
			glUniform3f(glGetUniformLocation(shader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
		}
		

		// Point light
		if (luzSeleccionada == 2) {
			glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].position"), PointLight.GetPosition().x, PointLight.GetPosition().y, PointLight.GetPosition().z);
			glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].quadratic"), 0.032);
		}

		// SpotLight
		if (luzSeleccionada == 3) {
			glUniform3f(glGetUniformLocation(shader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
			glUniform3f(glGetUniformLocation(shader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
			glUniform3f(glGetUniformLocation(shader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(shader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
			glUniform3f(glGetUniformLocation(shader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shader.Program, "spotLight.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader.Program, "spotLight.linear"), 0.09);
			glUniform1f(glGetUniformLocation(shader.Program, "spotLight.quadratic"), 0.032);
			glUniform1f(glGetUniformLocation(shader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
			glUniform1f(glGetUniformLocation(shader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
		}

		if (luzSeleccionada == 2) {
			PointShader.USE();

			//Variables and uniform from shaders
			GLint modelLoc = glGetUniformLocation(PointShader.Program, "model");
			GLint viewLoc = glGetUniformLocation(PointShader.Program, "view");
			GLint projLoc = glGetUniformLocation(PointShader.Program, "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(viewP));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projectionP));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(PointLight.GetModelMatrix()));

			PointLight.Draw();
		}
		
		modelShader.USE();

		GLint modelLocation = glGetUniformLocation(modelShader.Program, "model");
		GLint viewLocation = glGetUniformLocation(modelShader.Program, "view");
		GLint projLocation = glGetUniformLocation(modelShader.Program, "projection");

		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(viewP));
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, value_ptr(projectionP));

		mat4 modelM;
		modelM = translate(modelM, vec3(2.0f, -1.0f, -0.3f));
		modelM = scale(modelM, vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(modelM));

		ourModel.Draw(modelShader, 0);



		//Drawing the point light 2
		/*PointShader2.USE();

		//Variables and uniform from shaders
		modelLoc = glGetUniformLocation(PointShader2.Program, "model");
		viewLoc = glGetUniformLocation(PointShader2.Program, "view");
		projLoc = glGetUniformLocation(PointShader2.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(Lamp.GetModelMatrix()));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(PointLight2.GetModelMatrix()));

		//Lamp.Draw();
		//DirectionalLight.Draw();
		PointLight2.Draw();*/

		//Drawing the spot light
		/*SpotShader.USE();
		modelLoc = glGetUniformLocation(SpotShader.Program, "model");
		viewLoc = glGetUniformLocation(SpotShader.Program, "view");
		projLoc = glGetUniformLocation(SpotShader.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));
	
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(SpotLight.GetModelMatrix()));

		SpotLight.Draw();*/

		/*
		//Movement checkers
		if (controls == 1) { Cube.check_movement(deltaTime); }
		else { 
			//Lamp.check_movement(deltaTime); 
			PointLight.check_movement(deltaTime);
		}*/

		if (luzSeleccionada == 2) {
			PointLight.check_movement(deltaTime);
		}
		//Swap the screen buffers
		glfwSwapBuffers(window);
	}

	//Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0, 1.0, 0.0);
		glm::vec3 pos2(-1.0, -1.0, 0.0);
		glm::vec3 pos3(1.0, -1.0, 0.0);
		glm::vec3 pos4(1.0, 1.0, 0.0);
		// texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		// normal vector
		glm::vec3 nm(0.0, 0.0, 1.0);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// - triangle 1
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// - triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		GLfloat quadVertices[] = {
			// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

	
