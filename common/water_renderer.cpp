#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/stb_image.h>
#include <source/sailing_ship.hpp>

using namespace std;

#include <stdlib.h>
#include <string.h>

#include "water_renderer.hpp"


GLfloat playerY;

float sinOffset = 0.0f;
float sinOffset2 = 1.0f;
float sinOffset2start = 1.0f;
float speedOfWaves = 3.0f;
float speedOfWaves2 = 2.0f;

GLuint waterVAO;
GLuint waterProgramID; 

GLuint WaterMatrixID;
GLuint WaterViewMatrixID;
GLuint WaterModelMatrixID;
GLuint WaterTextureID;
GLuint LightColorID;


GLuint Texture2;

GLuint NumberOfVertices = 2400;
GLuint NumberOfNormals = 2400;

GLfloat waterVertexData[20 * 20 * 6 * 3];
GLfloat waterUVs[20 * 20 * 6 * 2];
GLfloat waterNormals[20 * 20 * 6 * 3];

GLuint waterVertexBuffer;
GLuint waterUVsBuffer;
GLuint waterNormalsBuffer;



//TO NIEPOZORNE GOWNO TRZEBA ZROBIC BARDZIEJ WIDOCZNE I GLOBALNE //include <constants>?
glm::vec3 lightColor = glm::vec3(1.0f, 0.8f, 0.8f);
//glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);


//offset always multiple of 4?


GLfloat getPlayerY() {
	//std::cout <<"playerY: "<< playerY << std::endl;
	return 50*playerY;
}

GLuint getNumberOfVertices() {
	return NumberOfVertices;
}

GLuint getNumberOfNormals() {
	return NumberOfNormals;
}

void GenerateWater(GLfloat vertices[], GLfloat waterUVs[], GLfloat waterNormals[], GLfloat offset) {
	
	static double lastTime = glfwGetTime();
	
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	sinOffset += speedOfWaves * deltaTime;
	if (sinOffset > 6.28f) sinOffset = 0.0f;

	sinOffset2 += speedOfWaves2 * deltaTime;
	if (sinOffset2 > 6.28f + sinOffset2start) sinOffset2 = sinOffset2start;

	glm::vec4 model = getModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	GLfloat playerX = fmod(model.x, 20.0f);
	GLfloat playerZ = fmod(model.z, 20.0f);
	if (playerX > 10.0f) playerX -= 20.0f;
	if (playerZ > 10.0f) playerZ -= 20.0f;

	//GLfloat playerX = model.x;
	//GLfloat playerZ = model.z;
	GLfloat player_sinX_left = sin(playerX * 0.314f + 3.14f + sinOffset);
	GLfloat player_sinZ_left = sin(playerZ * 0.314f + 3.14f + sinOffset2);
	playerY = (player_sinX_left + player_sinZ_left) / 2.0f;


	GLfloat x_left_corner;
	GLfloat z_left_corner;

	GLfloat x_right_corner;
	GLfloat z_right_corner;

	GLfloat y = 0.0f;
	
	GLfloat u_left_corner;
	GLfloat u_right_corner;
	GLfloat v_left_corner;
	GLfloat v_right_corner;

	GLfloat sinX_left;
	GLfloat sinX_right;
	GLfloat sinZ_left;
	GLfloat sinZ_right;


	int i = 0;
	int j = 0;
	int k = 0;

	x_left_corner = -1.0f;
	z_left_corner = -1.0f;

	//should be 2400 vertices
	while (z_left_corner<1.0f) {
		
		x_right_corner = x_left_corner + 0.1f;
		z_right_corner = z_left_corner + 0.1f;

		u_left_corner = (x_left_corner + 1.0f) / 2.0f;
		u_right_corner = (x_right_corner + 1.0f) / 2.0f;
		v_left_corner = (z_left_corner + 1.0f) / 2.0f;
		v_right_corner = (z_right_corner + 1.0f) / 2.0f;

		sinX_left = sin(x_left_corner * 3.14f + 3.14f + sinOffset);
		sinX_right = sin(x_right_corner * 3.14f + 3.14f + sinOffset);

		sinZ_left = sin(z_left_corner * 3.14f + 3.14f + sinOffset2);
		sinZ_right = sin(z_right_corner * 3.14f + 3.14f + sinOffset2);

		//first triangle
		vertices[i] = x_left_corner;
		i++;
		vertices[i] = (sinX_left + sinZ_left)/2.0f;
		i++;
		vertices[i] = z_left_corner;
		i++;

		waterUVs[j] = u_left_corner;
		j++;
		waterUVs[j] = v_left_corner;
		j++;

		glm::vec3 A = glm::vec3(vertices[i - 3], vertices[i - 2], vertices[i - 1]);


		vertices[i] = x_right_corner;
		i++;
		vertices[i] = (sinX_right + sinZ_right)/2.0f;
		i++;
		vertices[i] = z_right_corner;
		i++;
		
		waterUVs[j] = u_right_corner;
		j++;
		waterUVs[j] = v_right_corner;
		j++;

		glm::vec3 B = glm::vec3(vertices[i - 3], vertices[i - 2], vertices[i - 1]);



		vertices[i] = x_right_corner;
		i++;
		vertices[i] = (sinX_right + sinZ_left) / 2.0f;
		i++;
		vertices[i] = z_left_corner;
		i++;

		waterUVs[j] = u_right_corner;
		j++;
		waterUVs[j] = v_left_corner;
		j++;

		glm::vec3 C = glm::vec3(vertices[i - 3], vertices[i - 2], vertices[i - 1]);

		A = glm::normalize(glm::cross(A - B, A - C));

		for (int i = 0; i < 3; i++) {
			waterNormals[k] = A.x;
			k++;
			waterNormals[k] = A.y;
			k++;
			waterNormals[k] = A.z;
			k++;
		}

		//second triangle
		vertices[i] = x_left_corner;
		i++;
		vertices[i] = (sinX_left + sinZ_left) /2.0f;
		i++;
		vertices[i] = z_left_corner;
		i++;

		waterUVs[j] = u_left_corner;
		j++;
		waterUVs[j] = v_left_corner;
		j++;

		A = glm::vec3(vertices[i - 3], vertices[i - 2], vertices[i - 1]);


		vertices[i] = x_left_corner;
		i++;
		vertices[i] = (sinX_left + sinZ_right) / 2.0f;
		i++;
		vertices[i] = z_right_corner;
		i++;

		waterUVs[j] = u_left_corner;
		j++;
		waterUVs[j] = v_right_corner;
		j++;

		B = glm::vec3(vertices[i - 3], vertices[i - 2], vertices[i - 1]);


		vertices[i] = x_right_corner;
		i++;
		vertices[i] = (sinX_right + sinZ_right) /2.0f;
		i++;
		vertices[i] = z_right_corner;
		i++;
		
		waterUVs[j] = u_right_corner;
		j++;
		waterUVs[j] = v_right_corner;
		j++;

		C = glm::vec3(vertices[i - 3], vertices[i - 2], vertices[i - 1]);
		
		A = glm::normalize(glm::cross(A - B, A - C));

		for (int i = 0; i < 3; i++) {
			waterNormals[k] = A.x;
			k++;
			waterNormals[k] = A.y;
			k++;
			waterNormals[k] = A.z;
			k++;
		}


		//translate the space
		x_left_corner += 0.1f;
		

		if (x_left_corner >= 1.0f) {
			x_left_corner = -1.0f;
			z_left_corner += 0.1f;
		}
		
	}

	lastTime = currentTime;

}

void prepareWater() {
	waterProgramID = LoadShaders("WaterShading.vertexshader", "WaterShading.fragmentshader");

	WaterMatrixID = glGetUniformLocation(waterProgramID, "MVP");
	WaterViewMatrixID = glGetUniformLocation(waterProgramID, "V");
	WaterModelMatrixID = glGetUniformLocation(waterProgramID, "M");
	WaterTextureID = glGetUniformLocation(waterProgramID, "myTextureSampler");
	//LightColorID = glGetUniformLocation(waterProgramID, "LightColour");

	GenerateWater(waterVertexData, waterUVs, waterNormals, 0);

	glGenVertexArrays(1, &waterVAO);
	glBindVertexArray(waterVAO);

	
	glGenBuffers(1, &waterVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, waterVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertexData), waterVertexData, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, waterVertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	
	glGenBuffers(1, &waterUVsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, waterUVsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterUVs), waterUVs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, waterUVsBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	
	glGenBuffers(1, &waterNormalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, waterNormalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterNormals), waterNormals, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, waterNormalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);


	//Texture2 = loadDDS("water.DDS");
	//Texture2 = loadJPG("water3.jpg");
	Texture2 = loadBMP_custom("water2.bmp");

	glBindVertexArray(0);
}

void getAreaAroundShip(int& i, int& j){
	glm::mat4 ModelMatrix = getModelMatrix(); //get Model Matrix from sailing_ship.cpp
	float temp_i = ModelMatrix[3][0];
	float temp_j = ModelMatrix[3][2];

	std::cout << temp_i << "  " << temp_j << endl;

	if (int(temp_i) % 20 == 0) {
		i = (int)temp_i/10;
		//cout << i << " ";
	}
	if ((int)temp_j % 20 == 0) {
		j = (int)temp_j / 10;
		//cout << j << endl;
	}


}

void drawWaterFrame(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
	
	static int i = 0;
	static int j = 0;


	glUseProgram(waterProgramID);

	GenerateWater(waterVertexData, waterUVs, waterNormals, 0);

	glBindVertexArray(waterVAO);

	glBindBuffer(GL_ARRAY_BUFFER, waterVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertexData), waterVertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, waterNormalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterNormals), waterNormals, GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(WaterViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	glm::vec3 lightPos = glm::vec3(10, 4, 0);
	glUniform3f(glGetUniformLocation(waterProgramID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

	//glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
	glUniform3f(glGetUniformLocation(waterProgramID, "LightColor"), lightColor.x, lightColor.y, lightColor.z);


	// Bind our texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture2);
	glUniform1i(WaterTextureID, 1);

	//cout << i << " " << j << endl;

	getAreaAroundShip(i, j);
	int top_i = i + 10;
	int top_j = j + 10;

	for (int m =i-10 ; m <= top_i; m = m + 2) {
		for (int n =j-10; n <= top_j; n = n + 2) {
			drawWaterSquareTemp(ProjectionMatrix, ViewMatrix, m, n);
		}
	}
	glBindVertexArray(0);
}

void drawWaterSquareTemp(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, GLfloat xPosition, GLfloat zPosition) {
	glm::mat4 ModelMatrix3 = glm::mat4(1.0f);
	ModelMatrix3 = glm::scale(ModelMatrix3, glm::vec3(10.f, 0.2f, 10.0f));
	ModelMatrix3 = glm::translate(ModelMatrix3, glm::vec3(xPosition, 0.0f, zPosition));
	glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;
	glUniformMatrix4fv(glGetUniformLocation(waterProgramID, "MVP"), 1, GL_FALSE, &MVP3[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(waterProgramID, "M"), 1, GL_FALSE, &ModelMatrix3[0][0]);
	glDrawArrays(
		GL_TRIANGLES,
		0,
		getNumberOfVertices()
	);
}