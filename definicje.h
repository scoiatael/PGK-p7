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

#include<sys/time.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cassert>

struct mcc
{
  static int get_time();
  static float get_timef();

  std::vector< std::pair<int,int> > locks;
  int last_update;
  mcc()
  :  locks(), last_update(get_time())
    {}

  int make_lock(const float& timeout);
  int get_lock(const int& ID);
  void reset_lock(const int& ID);
  void update();
};

void InitGraphics();
void loadVertices(const std::string& filename, std::vector< int>& arg, const bool& bin, const int& side, std::pair<int,int>& edge, int& height);
void genIndices(std::vector<GLuint>& indices, const unsigned int& side, const unsigned int& density);
void parse_args(const int& argc, char** argv, std::vector<std::string>& arg);
#endif
