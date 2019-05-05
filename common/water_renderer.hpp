#ifndef WATER_RENDERER_HPP
#define WATER_RENDERER_HPP

void GenerateWater(GLfloat vertices[], GLfloat waterUVs[], GLfloat waterNormals[], GLfloat offset);
GLfloat getPlayerY();
GLuint getNumberOfVertices();
GLuint getNumberOfNormals();
void prepareWater();
void drawWaterFrame(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);
void drawWaterSquareTemp(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, GLfloat xPosition, GLfloat zPosition);
#endif