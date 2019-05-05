#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getModelTranslationVector();
GLfloat getAngle();
GLfloat getPaddleAngle();
glm::vec3 getCameraDirection();

#endif