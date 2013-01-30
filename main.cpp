#include "definicje.h"

int main( int argc, char** argv )
{

  std::cout << "Starting..\n";
  std::vector<std::string> file_names;
  parse_args(argc, argv, file_names);
  file_names.push_back(std::string("b0e0.hgt"));
  InitGraphics();

  glfwSetWindowTitle( "p7" );

  // Ensure we can capture the escape key being pressed below
  glfwEnable( GLFW_STICKY_KEYS );

  // Dark blue background
  glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS); 
//  glFrontFace(GL_CCW);

  // Create and compile our GLSL program from the shaders
  GLuint programIDs[2] = { LoadShaders( "tvs.vertexshader", "cfs.fragmentshader" ), LoadShaders( "tvs2.vertexshader", "cfs.fragmentshader" )};
  std::cout << "Linked shaders..\n";
  // Get a handle for our "MVP" uniform
  GLuint MatrixIDs[2] = {glGetUniformLocation(programIDs[0], "MVP"), glGetUniformLocation(programIDs[1], "MVP")};
  GLuint EdgexIDs[2] = {glGetUniformLocation(programIDs[0], "Edgex"), glGetUniformLocation(programIDs[1], "Edgex")};
  GLuint EdgeyIDs[2] = {glGetUniformLocation(programIDs[0], "Edgey"), glGetUniformLocation(programIDs[1], "Edgey")};
  GLuint HeightIDs[2] = {glGetUniformLocation(programIDs[0], "height"), glGetUniformLocation(programIDs[1], "height")};
  GLuint TextureIDs[2] = {glGetUniformLocation(programIDs[0], "tex2d"), glGetUniformLocation(programIDs[1], "tex2d")};
  GLuint TimeIDs[2] = {glGetUniformLocation(programIDs[0], "time"), glGetUniformLocation(programIDs[1], "time")};
  GLuint AlphaIDs[2] = {glGetUniformLocation(programIDs[0], "alpha"), glGetUniformLocation(programIDs[1], "alpha")};
  std::cout << "Got uniforms..\n";
  std::cout << glGetString(GL_VERSION) << std::endl;
 
  std::cout << "Loadin textures...\n";

  char texName[] = "texture3.jpg";
  std::cout << texName << std::endl;
  int t1x,t1y,t1comp;
  FILE* t1f = fopen(texName, "rb");
  unsigned char* tex1data = stbi_load_from_file(t1f, &t1x, &t1y, &t1comp,0);
  unsigned int tex_2d;
  glGenTextures(1, &tex_2d);
  glBindTexture(GL_TEXTURE_2D, tex_2d);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t1x, t1y, 0, GL_RGB, GL_UNSIGNED_BYTE, tex1data);
/*
  char texNameCl[] = "textureClouds3.png";
  std::cout << texNameCl << std::endl;
  int t2x,t2y,t2comp;
  FILE* t2f = fopen(texNameCl, "rb");
  unsigned char* tex2data = stbi_load_from_file  (t2f, &t2x, &t2y, &t2comp,0);
  unsigned int tex_cl;
  glGenTextures(1, &tex_cl);
  glBindTexture(GL_TEXTURE_2D, tex_cl);
  //-->
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t2x, t2y, 0, GL_RGB, GL_UNSIGNED_BYTE, tex2data);
*/
  std::cout << "Done!\n";

  const int side(1201);
  const int vBOsize(file_names.size());


  //Vertices:
  short piece_map[360][181];
  for(int i=0; i<360; i++)
    for(int y=0; y<=180; y++)
      piece_map[i][y] = -1;
  unsigned int numberOfVertices=side*side;
  GLuint vaoObjects[vBOsize+1], vertexBufferObject, vBOs[vBOsize+1];
  std::vector<std::pair<int, int> > edges(vBOsize+1);
  // -->
  std::cout << "Generating arrays...\n";
  glGenVertexArrays(vBOsize, vaoObjects);
  std::cout << "Done\n";
  glGenBuffers(vBOsize, vBOs);
  int height;
  // <---
  for(short i=0; i< vBOsize;i++)
  {
    std::vector< int > vertexPositionsVec(3*numberOfVertices);
    int* vertexPositions = &vertexPositionsVec[0];
    loadVertices(file_names[i], vertexPositionsVec, true, side, edges[i], height);
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
    if(i<vBOsize-1)
    {
      piece_map[edges[i].second+180][edges[i].first+90]=i;
      std::cout << edges[i].second+180 << " " << edges[i].first+90 << std::endl;
    }
  }

  //Indices::
  GLuint indexBufferObject, iBOs[maxLoD], numberOfIndices;
  std::vector<GLuint> nOIs(maxLoD);
  glGenBuffers(maxLoD, iBOs);
  for(unsigned int density=1, i=0;i<maxLoD; i++, density*=2)
  {  
    std::cout << "Entering for with i: " << i << "\n";
    nOIs[i]=(side-1)/density;
    if((side-1)%density!=0)
        nOIs[i]+=1;
    nOIs[i]=6*(nOIs[i]*(nOIs[i]));
    std::cout << "Allocating memory...\n";
    GLuint* indices = new GLuint [nOIs[i]];
    std::cout << "Done.\n";
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBOs[i]);
    std::cout << "Density: " << density << " Number of indices: " << nOIs[i] << std::endl;
    genIndices(indices, side, density);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*nOIs[i], indices, GL_STATIC_DRAW);
    std::cout << "Leaving for with i: " << i << "\n";
  }
  
  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection = 
   // glm::mat4(1.0f);
glm::perspective(45.0f, 4.0f / 3.0f, 100.f, 30000.0f);
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

  x = edges[0].second*12010;
  startx=x;
  y = edges[0].first*12010;
  starty=y;
  std::cout << edges[0].first << " " << edges[0].second << std::endl;
  do{
    int ex = x/12010+180;
    int ey = y/12010+90;
    //time statistics:
    FPScounter++;
    double cur_time = glfwGetTime();
    if(cur_time-last_reset>=2)
    {
      double FPS = (float)FPScounter/(cur_time-last_reset);
      std::cout << "FPS: " << FPS << " lod: " << iBOindex << std::endl;
      std::cout << ex << " " << ey << " " << alpha <<  std::endl;
      if(autolod && abs(FPS-optfps)>4)
      {
        if(FPS<optfps && iBOindex<maxLoD)
          iBOindex++;
        if(FPS>4*optfps && iBOindex > 0)
          iBOindex--;
      }
      FPScounter=0;
      last_reset=cur_time;
    }
    last_time=cur_time;

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programIDs[ball]);
    glm::mat4 Vw;
    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    Vw=MVP * glm::translate( mat4(1.0),vec3((-x + 6000)*ball,(-y - 6000)*ball,(z - 12000)*ball)) 
                     * glm::rotate(mat4(1.0), oz, glm::vec3(0,1,0)) 
                     * glm::rotate(mat4(1.0), ox, glm::vec3(1,0,0)) 
                     * glm::rotate(mat4(1.0), oy, glm::vec3(0,0,1))
                     * glm::translate( mat4(1.0), vec3(-x*(1-ball), -y*(1-ball), z*(1-ball)));
    glUniformMatrix4fv(MatrixIDs[ball], 1, GL_FALSE, &Vw[0][0]);
    
    glUniform1i(HeightIDs[ball], 0);
    
    glUniform1f(TimeIDs[ball], glfwGetTime());
    glUniform1f(AlphaIDs[ball], (float)alpha*0.1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_2d);
    glUniform1i(TextureIDs[ball], /*GL_TEXTURE0+*/0);

    indexBufferObject=iBOs[iBOindex];
    numberOfIndices=nOIs[iBOindex];

 //   std::cout << ex << " " << ey << std::endl;
    if(ball==0)
    {
        glCullFace(GL_FRONT);
      for(int i = max(ex-3,0); i<= min(ex+3,360) ;i++)
        for(int j=max(ey-3,0); j<= min(ey+3,180); j++)
        {
          glUniform1i(EdgexIDs[ball], (i-180));
          glUniform1i(EdgeyIDs[ball], (j-90));
          int point;
          if(piece_map[i][j]==-1)
          {
            point=vBOsize-1;
            draw(vaoObjects[point], vBOs[point], iBOs[maxLoD-1], nOIs[maxLoD-1]);
          }
          else
          {
            point = piece_map[i][j];
            draw(vaoObjects[point], vBOs[point], indexBufferObject, numberOfIndices);
          }
   //         std::cout << "Drawing " << file_names[point] << "with mods " << i-180 << " " << j-90 << std::endl
   //           << i << " "  << ex << " " << j << " " << ey << std::endl;

        }
    }
    else
    {
        glCullFace(GL_FRONT);
      for(int i=/*edges[0].second+180+*/0; i</*edges[0].second+18*/360;i++)
        for(int j=/*edges[0].first+90+*/0; j<=/*edges[0].first+90*/180;j++)
        {

          glUniform1i(EdgexIDs[ball], (i-180));
          glUniform1i(EdgeyIDs[ball], (j-90));

          int point;
          if(piece_map[i][j]==-1)
          {
            point=vBOsize-1;
            draw(vaoObjects[point], vBOs[point], iBOs[maxLoD-1], nOIs[maxLoD-1]);
          }
          else
          {
            point = piece_map[i][j];
            draw(vaoObjects[point], vBOs[point], indexBufferObject, numberOfIndices);
          }
        }
    }


    //CLOUDS
/*
    Vw=MVP * glm::translate( mat4(1.0),vec3((-x + 6000)*ball,(-y - 6000)*ball,(z - 12000)*ball)) 
                     * glm::rotate(mat4(1.0), oz+(float)glfwGetTime(), glm::vec3(0,1,0)) 
                     * glm::rotate(mat4(1.0), ox, glm::vec3(1,0,0)) 
                     * glm::rotate(mat4(1.0), oy, glm::vec3(0,0,1))
                     * glm::translate( mat4(1.0), vec3(-x*(1-ball), -y*(1-ball), z*(1-ball)));
    glUniformMatrix4fv(MatrixIDs[ball], 1, GL_FALSE, &Vw[0][0]);
    
    glUniform1i(HeightIDs[ball], 100);
    
    glUniform1f(TimeIDs[ball], glfwGetTime());
    glUniform1f(AlphaIDs[ball], 0.25);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_cl);
    glUniform1i(TextureIDs[ball], 0);

    indexBufferObject=iBOs[iBOindex];
    numberOfIndices=nOIs[iBOindex];

 //   std::cout << ex << " " << ey << std::endl;
    if(ball==0)
    {
        glCullFace(GL_FRONT);
      for(int i = max(ex-3,0); i<= min(ex+3,360) ;i++)
        for(int j=max(ey-3,0); j<= min(ey+3,180); j++)
        {
          glUniform1i(EdgexIDs[ball], (i-180));
          glUniform1i(EdgeyIDs[ball], (j-90));
          int point;
          {
            point=vBOsize-1;
            draw(vaoObjects[point], vBOs[point], iBOs[maxLoD-1], nOIs[maxLoD-1]);
          }

        }
    }
    else
    {
        glCullFace(GL_FRONT);
      for(int i=0; i<360;i++)
        for(int j=0; j<=180;j++)
        {

          glUniform1i(EdgexIDs[ball], (i-180));
          glUniform1i(EdgeyIDs[ball], (j-90));

          int point;
          {
            point=vBOsize-1;
            draw(vaoObjects[point], vBOs[point], iBOs[maxLoD-1], nOIs[maxLoD-1]);
          }
        }
    }*/
    // Swap buffers
    glfwSwapBuffers();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
             glfwGetWindowParam( GLFW_OPENED ) );

  // Cleanup VBO and shader
  glDeleteProgram(programIDs[0]);
  glDeleteProgram(programIDs[1]);
//  CleanVBOs(vaoObjects, vBOs, vBOsize+1, iBOs, tex_2d);

  std::cout << "Cleaning done, terminating..\n";
  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

