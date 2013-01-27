#ifndef definicje
#define definicje

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GL/glfw.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

#include "SOIL.h"

#include<sys/time.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cassert>

extern int x,y,z;
extern unsigned int iBOindex;
extern float ox, oy,oz;
extern const int maxLoD;
extern bool autolod;
extern const double optfps;
extern char ball;
extern int startx, starty;

void InitGraphics();
int min(int a, int b);
int max(int a, int b);
void draw(GLuint& vaoObject, GLuint& vertexBufferObject, GLuint& indexBufferObject, unsigned int numberOfVertices);

void CleanVBOs(GLuint* vaoObjects, GLuint* vBO, const unsigned int& vBOsize,  GLuint* iBO, GLuint texture);

void GLFWCALL Key_Callback(int key, int action);
void loadVertices(const std::string& filename, std::vector< int>& arg, const bool& bin, const int& side, std::pair<int,int>& edge, int& height);
void genIndices(GLuint* indices, const unsigned int& side, const unsigned int& density);
void parse_args(const int& argc, char** argv, std::vector<std::string>& arg);
#endif
