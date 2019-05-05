// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::vec3 direction;
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::vec3 TranslationVector = glm::vec3(0.0f, 0.0f, 0.0f);
GLfloat Angle;
GLfloat PaddleAngle;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

glm::vec3 getModelTranslationVector() {
	glm::vec3 temp = TranslationVector;
	TranslationVector = glm::vec3(0.0f, 0.0f, 0.0f);
	return temp;
}

GLfloat getAngle() {
	GLfloat temp = Angle;
	Angle = 0.0f;
	return temp;
}

GLfloat getPaddleAngle() {
	//GLfloat temp = PaddleAngle;
	//PaddleAngle = 0.0f;
	return PaddleAngle;
}

glm::vec3 getCameraDirection() {
	return direction;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 5);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.07f;
float rotationSpeed = 3.0f;
float paddleRotationSpeed = 1.0f;


void computeMatricesFromInputs() {

	speed = 1000.0f;
	paddleRotationSpeed = 2.0f;

	//static glm::vec3 translationVector = glm::vec3(0.0f, 0.0f, 0.0f);

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * deltaTime * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * float(768 / 2 - ypos);
	// if (verticalAngle > 1.57f) verticalAngle = 1.57f;
	if (verticalAngle > -0.1f) verticalAngle = -0.1f;
	else if (verticalAngle < -1.57f) verticalAngle = -1.57f;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Up vector
	//glm::vec3 up = glm::cross( right, direction );


	// Sprint
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed *= 2;
		paddleRotationSpeed *= 2;
	}
	// FPS CAMERA
	/*
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}
	// Move up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += up * deltaTime * speed;
	}
	// Move down
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		position -= up * deltaTime * speed;
	}
	*/



	// SHIP CAMERA
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		TranslationVector -= glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime * speed;

		PaddleAngle += 1 * deltaTime * paddleRotationSpeed;
		if (PaddleAngle > 6.28f) PaddleAngle = 0.0f;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		TranslationVector += glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime * speed;

		PaddleAngle -= 1 * deltaTime * paddleRotationSpeed;
		if (PaddleAngle < 0.0f) PaddleAngle = 6.28f;
	}
	// Rotate left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Angle += 1 * deltaTime * rotationSpeed;
	}
	// Rotate right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Angle -= 1 * deltaTime * rotationSpeed;
	}



	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}