#include "definicje.h"
int x,y,z;
unsigned int iBOindex;
float ox, oy;
const int maxLoD(10);
bool autolod=false;
const double optfps(20);
char ball=0;


vec3 test_coords(vec3 vertexPosition)
{
  vec4 temp =  vec4(vec3(vertexPosition.x*10, vertexPosition.y*10,vertexPosition.z), 1.0);
  float R = 6400;
  float N = R/sqrt(1-pow(cos(temp.x),2));
  return vec3((N+temp.z)*cos(temp.x)*cos(temp.y), (N+temp.z)*cos(temp.x)*sin(temp.y), (N+temp.z)*sin(temp.x));
}
void init(GLuint& vaoObject1)
{
}

void draw(GLuint& vaoObject, GLuint& vertexBufferObject, GLuint& indexBufferObject, unsigned int numberOfVertices)
{
    glBindVertexArray(vaoObject);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glDrawElements(GL_TRIANGLES, numberOfVertices, GL_UNSIGNED_INT,0);
}

void CleanVBOs(GLuint* vaoObjects, GLuint* vBO, const unsigned int& vBOsize,  GLuint* iBO)
{
  glDeleteBuffers(vBOsize, vBO);
  glDeleteBuffers(maxLoD, iBO);
  glDeleteVertexArrays(vBOsize, vaoObjects);
}

void GLFWCALL Key_Callback(int key, int action)
{
  int mod=500;
  if(action == GLFW_PRESS)
  {
//    std::cout << (char)key << " " << iBOindex <<  "\n";
    switch(key)
    {
      case 'Q':
        x-=mod;
        break;
      case 'A':
        x+=mod;
        break;
      case 'W':
        y-=mod;
        break;
      case 'S':
        y+=mod;
        break;
      case 'E':
        z+=mod;
        break;
      case 'D':
        z-=mod;
        break;
      case 'R':
        ox+=5;
        break;
      case 'F':
        ox-=5;
        break;
      case 'T':
        oy+=5;
        break;
      case 'G':
        oy-=5;
        break;
      case GLFW_KEY_UP:
        iBOindex+=1;
        iBOindex%=maxLoD;
        break;
      case GLFW_KEY_DOWN:
        iBOindex-=1;
        iBOindex%=maxLoD;
        break;
      case GLFW_KEY_SPACE:
        autolod=!autolod;
        break;
      case GLFW_KEY_ENTER:
        ball=1-ball;
        break;


    }
  }
}

int main( int argc, char** argv )
{

  std::cout << "Starting..\n";
  std::vector<std::string> file_names;
  parse_args(argc, argv, file_names);
  InitGraphics();

  glfwSetWindowTitle( "p6" );

  // Ensure we can capture the escape key being pressed below
  glfwEnable( GLFW_STICKY_KEYS );

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS); 

  // Create and compile our GLSL program from the shaders
  GLuint programIDs[2] = { LoadShaders( "tvs.vertexshader", "cfs.fragmentshader" ), LoadShaders( "tvs2.vertexshader", "cfs.fragmentshader" )};
  std::cout << "Linked shaders..\n";
  // Get a handle for our "MVP" uniform
  GLuint MatrixIDs[2] = {glGetUniformLocation(programIDs[0], "MVP"), glGetUniformLocation(programIDs[1], "MVP")};
  std::cout << "Got uniforms..\n";
  
  const int side(1201);
  const int vBOsize(file_names.size());

//DEBUG
  std::vector< vec3 > edges_vec;


  //Vertices:
  unsigned int numberOfVertices=side*side;
  GLuint vaoObjects[vBOsize], vertexBufferObject, vBOs[vBOsize];
  std::vector<std::pair<int, int> > edges(vBOsize);
  glGenVertexArrays(vBOsize, vaoObjects);
  glGenBuffers(vBOsize, vBOs);
  int height;
  for(unsigned int i=0; i< vBOsize;i++)
  {
    std::vector< int > vertexPositionsVec(3*numberOfVertices);
    int* vertexPositions = &vertexPositionsVec[0];
    loadVertices(file_names[i], vertexPositionsVec, true, side, edges[i], height);
//    edges[i].first-=edges[0].first;
//    edges[i].second-=edges[0].second;
    edges_vec.push_back(vec3(vertexPositionsVec[0], vertexPositionsVec[1], vertexPositionsVec[2]));
    glBindVertexArray(vaoObjects[i]);
    glBindBuffer(GL_ARRAY_BUFFER, vBOs[i]);
    glVertexAttribPointer(
          0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
          3,                  // size
          GL_INT,           // type
          GL_FALSE,           // normalized?
          0,                  // stride
          (void*)0            // array buffer offset
    );
    glBufferData(GL_ARRAY_BUFFER, sizeof(int)*3*numberOfVertices, vertexPositions, GL_STATIC_DRAW);
  }

  //Indices::
  GLuint indexBufferObject, iBOs[maxLoD], numberOfIndices;
  std::vector<GLuint> nOIs(5);
  glGenBuffers(maxLoD, iBOs);
  for(unsigned int density=1, i=0;i<maxLoD; i++, density*=2)
  {  
    nOIs[i]=6*((side-1)/(density)+1)*((side-1)/(density)+1);
    std::vector< GLuint> indicesVec(nOIs[i]);
    GLuint* indices = &indicesVec[0];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBOs[i]);
    std::cout << "Density: " << density << " Number of indices: " << nOIs[i] << std::endl;
    genIndices(indicesVec, side, density);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*nOIs[i], indices, GL_STATIC_DRAW);
  }
  
  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection = 
   // glm::mat4(1.0f);
glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 30000.0f);
  // Camera matrix
//  int xVw = edges[0].first*side, yVw = edges[0].second*side;
  int xVw = 6000, yVw = -6000;
  height = 3000;
  glm::mat4 View       = glm::lookAt(
                                                          glm::vec3(xVw,yVw,2*height), // Camera is at (4,3,-3), in World Space
                                                          glm::vec3(xVw,yVw,0), // and looks at the origin
                                                          glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                             );
  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 Model      = glm::mat4(1.0f);
  // Our ModelViewProjection : multiplication of our 3 matrices
	
  glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

  std::cout << "Init done.\n";
  
  glfwSetKeyCallback(Key_Callback);
  
  double last_time = glfwGetTime(), last_reset=last_time;
  int FPScounter=0;

  //DEBUG
  vec3 tempD = test_coords(edges_vec[0]);
  std::cout << "1st pos: " << tempD.x << " " << tempD.y << " " << tempD.z << std::endl;
  do{
    //time statistics:
    FPScounter++;
    double cur_time = glfwGetTime();
    double FPS = 1/(cur_time-last_time);
    if(autolod && abs(FPS-optfps)>4)
    {
      if(FPS<optfps && iBOindex<maxLoD)
        iBOindex++;
      if(FPS>optfps && iBOindex > 0)
        iBOindex--;
    }
    if(cur_time-last_reset>=2)
    {
      std::cout << "True FPS: " << (float)FPScounter/(cur_time-last_reset) << " cur FPS: " << FPS << " lod: " << iBOindex << std::endl;
      FPScounter=0;
      last_reset=cur_time;
    }
    last_time=cur_time;

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programIDs[ball]);

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glm::mat4 Vw=glm::rotate( glm::rotate(glm::translate(glm::mat4(1.0), vec3(x,y,z)), oy, glm::vec3(0, 1, 0)), ox, glm::vec3(1,0,0));
    indexBufferObject=iBOs[iBOindex];
    numberOfIndices=nOIs[iBOindex];
    for(unsigned int i=0; i<vBOsize;i++)
    {
      glm::mat4 temp =  MVP * Vw * 
        glm::translate(glm::mat4(1.0), vec3((edges[i].second-edges[0].second)*(side-5)*10, (edges[i].first-edges[0].first)*(side-5)*10,0)) ;
      glUniformMatrix4fv(MatrixIDs[ball], 1, GL_FALSE, &temp[0][0]);
      draw(vaoObjects[i], vBOs[i], indexBufferObject, numberOfIndices);
    }

    // Swap buffers
    glfwSwapBuffers();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
             glfwGetWindowParam( GLFW_OPENED ) );

  // Cleanup VBO and shader
  glDeleteProgram(programIDs[0]);
  glDeleteProgram(programIDs[1]);
  CleanVBOs(vaoObjects, vBOs, vBOsize, iBOs);

  std::cout << "Cleaning done, terminating..\n";
  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

