// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <ctime>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/OBJ_Loader.h>
#include <common/vboindexer.hpp>
#include <common/water_renderer.hpp>
#include <common/skybox.hpp>


GLuint Texture;
GLuint Texture3;
GLuint Texture4;
GLuint TextureID;

GLuint monkeyVAO;
GLuint actualMonkeyVAO;
GLuint paddleVAO;
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint monkeyProgramID;
//GLuint waterProgramID;

std::vector<unsigned short> indices;
std::vector<unsigned short> indices2;
glm::vec3 lightPos;
glm::vec3 lightPos2;

// WAZNE, MODEL ZDEFINIOWANY W TYM MIEJSCU POZWALA NA RUCH STATKU
glm::mat4 ModelMatrix(1.0f);
glm::mat4 ModelMatrix2(1.0f);
glm::mat4 ModelMatrix3(1.0f);

glm::mat4 getModelMatrix() {
	return ModelMatrix;
}


void preparePaddle() {
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("MoanaPaddle.obj", vertices, uvs, normals);
	Texture3 = loadBMP_custom("moanaPaddle.bmp");

	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices2, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VAO and VBOs

	glGenVertexArrays(1, &paddleVAO);
	glBindVertexArray(paddleVAO);


	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(unsigned short), &indices2[0], GL_STATIC_DRAW);


	// Generate a buffer for the indices as well


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	glBindVertexArray(0);
}

void drawPaddle(glm::mat4 & ModelMatrix, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {


	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


	// Use our shader
	glUseProgram(monkeyProgramID);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	glUniform3f(glGetUniformLocation(monkeyProgramID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(monkeyProgramID, "SecondLightPosition_worldspace"), lightPos2.x, lightPos2.y, lightPos2.z);


	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture3);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 1);

	glBindVertexArray(paddleVAO);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices2.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glBindVertexArray(0);
}


void prepareMonkey() {

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("Boat.obj", vertices, uvs, normals);
	Texture = loadBMP_custom("boattex.bmp");

	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VAO and VBOs

	glGenVertexArrays(1, &monkeyVAO);
	glBindVertexArray(monkeyVAO);

	
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
	

	// Generate a buffer for the indices as well


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	glBindVertexArray(0);
}

void drawMonkey(glm::mat4 & ModelMatrix, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix ) {

	
	ModelMatrix = glm::rotate(ModelMatrix, getAngle(), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, getModelTranslationVector());
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f,(getPlayerY()), 0.0f));
	//ModelMatrix[1][0] = 0.0f;
	//ModelMatrix[1][1] = 1.0f;
	//ModelMatrix[1][2] = 0.0f;
	//ModelMatrix[1][3] = float(getPlayerY());


	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	
	// Use our shader
	glUseProgram(monkeyProgramID);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	glUniform3f(glGetUniformLocation(monkeyProgramID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(monkeyProgramID, "SecondLightPosition_worldspace"), lightPos2.x, lightPos2.y, lightPos2.z);


	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	glBindVertexArray(monkeyVAO);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glBindVertexArray(0);

}


void prepareActualMonkey() {

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("suzanne.obj", vertices, uvs, normals);
	Texture4 = loadDDS("uvmap.dds");

	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VAO and VBOs

	glGenVertexArrays(1, &actualMonkeyVAO);
	glBindVertexArray(actualMonkeyVAO);


	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);


	// Generate a buffer for the indices as well


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	glBindVertexArray(0);
}

void drawActualMonkey(glm::mat4 & ModelMatrix, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {


	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


	// Use our shader
	glUseProgram(monkeyProgramID);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	glUniform3f(glGetUniformLocation(monkeyProgramID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(monkeyProgramID, "SecondLightPosition_worldspace"), lightPos2.x, lightPos2.y, lightPos2.z);


	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Texture4);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 4);

	glBindVertexArray(actualMonkeyVAO);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glBindVertexArray(0);
}


int main( void )
{

	srand(time(0));

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Sailing Ship", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);


	// Create and compile our GLSL program from the shaders
	monkeyProgramID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );


	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(monkeyProgramID, "MVP");
	ViewMatrixID = glGetUniformLocation(monkeyProgramID, "V");
	ModelMatrixID = glGetUniformLocation(monkeyProgramID, "M");

	


	// Load the textures
	//Texture = loadDDS("OldBoat.DDS");
	


	// Get a handle for our "myTextureSampler" uniform
	TextureID = glGetUniformLocation(monkeyProgramID, "myTextureSampler");
	
	
	//ustaw pozycje slonca
	//lightPos = glm::vec3(5, 4, -5);
	lightPos = glm::vec3(-1000, 1000, -1000);
	lightPos2 = glm::vec3(0, 5, 0);


	prepareSkybox();
	prepareMonkey();
	prepareActualMonkey();

	prepareWater();
	
	preparePaddle();

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;


	
	ModelMatrix = glm::translate(ModelMatrix, vec3(0.0f, -0.2f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.004f, 0.004f, 0.004f));
	
	glm::mat4 actualMonkeyModelMatrix = glm::mat4(1.0f);
	actualMonkeyModelMatrix = glm::translate(actualMonkeyModelMatrix, glm::vec3(200.0f, 0.0f, 200.0f));
	actualMonkeyModelMatrix = glm::scale(actualMonkeyModelMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	actualMonkeyModelMatrix = glm::rotate(actualMonkeyModelMatrix, 4.14f, glm::vec3(0.0f, 1.0f, 0.0f));

	do{

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::vec4 ObjectCenter = ModelMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
		ObjectCenter = vec4(ObjectCenter.x, 0.0f, ObjectCenter.z, 1.0f);
		glm::mat4 ViewMatrix = lookAt(vec3(ObjectCenter) - (getCameraDirection()*10.0f), vec3(ObjectCenter), vec3(0.0f, 1.0f, 0.0f));
		

		//start of skybox
		drawSkybox(ViewMatrix, ProjectionMatrix);
		//end of skybox

		// Start of water 
		//glDisable(GL_CULL_FACE);
		drawWaterFrame(ProjectionMatrix, ViewMatrix);
		//glEnable(GL_CULL_FACE);



		//start of first monkey
		//TODO
		//WAZNE, TA FUNKCJA DZIALA ZGODNIE ZE STEROWANIEM, TRZEBA ZMIENIC TO NA drawPlayer() ALBO COS TAKIEGO//
		
		drawMonkey(ModelMatrix, ProjectionMatrix, ViewMatrix);
		
		drawActualMonkey(actualMonkeyModelMatrix, ProjectionMatrix, ViewMatrix);
		
		
		ModelMatrix2 = glm::scale(ModelMatrix, vec3(100.0f, 100.0f, 100.0f));
		ModelMatrix2 = glm::translate(ModelMatrix2, vec3(0.0f, 3.0f, 3.0f));
		ModelMatrix2 = glm::rotate(ModelMatrix2, 3.14f/2.0f, vec3(0.0f, 1.0f, 0.0f));
		glPushMatrix();
		ModelMatrix2 = glm::rotate(ModelMatrix2, -getPaddleAngle(), vec3(1.0f, 0.0f, 0.0f));
		glPopMatrix();
		ModelMatrix2 = glm::translate(ModelMatrix2, vec3(0.0f, -3.0f, 0.0f));


		ModelMatrix3 = glm::scale(ModelMatrix, vec3(100.0f, 100.0f, 100.0f));
		ModelMatrix3 = glm::translate(ModelMatrix3, vec3(0.0f, 3.0f, -3.0f));
		ModelMatrix3 = glm::rotate(ModelMatrix3, 3.14f + 3.14f / 2.0f, vec3(0.0f, 1.0f, 0.0f));
		glPushMatrix();
		ModelMatrix3 = glm::rotate(ModelMatrix3, getPaddleAngle(), vec3(1.0f, 0.0f, 0.0f));
		glPopMatrix();
		ModelMatrix3 = glm::translate(ModelMatrix3, vec3(0.0f, -2.85f, 0.0f));


		drawPaddle(ModelMatrix2, ProjectionMatrix, ViewMatrix);
		drawPaddle(ModelMatrix3, ProjectionMatrix, ViewMatrix);

		//end of first monkey
		
		glBindVertexArray(0);


		//BARDZO WAZNE BO INACZEJ ZACZNIE TONAC. BARDZO.
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -getPlayerY(), 0.0f));



		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );





	//TODO spojrzec na to czyszczenie bo olalem to teraz


	// Cleanup VBO and shader
	//glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &uvbuffer);
	//glDeleteBuffers(1, &normalbuffer);
	//glDeleteBuffers(1, &elementbuffer);
	//glDeleteProgram(monkeyProgramID);
	glDeleteTextures(1, &Texture);
//	glDeleteVertexArrays(1, &VertexArrayID);

//	glDeleteBuffers(1, &waterVertexBuffer);
//	glDeleteBuffers(1, &waterUVsBuffer);
//	glDeleteBuffers(1, &waterNormalsBuffer);
//	glDeleteProgram(waterProgramID);
	//glDeleteTextures(1, &Texture2);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

