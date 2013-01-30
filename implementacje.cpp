#include "definicje.h"

int x,y,z;
unsigned int iBOindex=1;
float ox, oy,oz;
const int maxLoD(10);
bool autolod=false;
const double optfps(20);
char ball=0;
int startx, starty;
int alpha;

void parse_args(const int& argc, char** argv, std::vector<std::string>& arg)
{
  arg.clear();
  if(argc==1 || (argv[1][0]=='-' && argc==2))
  {
    std::cerr << "Usage: " << argv[0] << " (-[options]) [filenames]\n";
    exit(-1);
  }
  int i=1;
  if(argv[1][0]=='-')
  {
    std::cout << "Options unimplemented yet.\n";
    i++;
  }
  for(;i<argc;i++)
  {
    arg.push_back(std::string(argv[i]));
  }

}
void InitGraphics()
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit(-1);
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		exit(-1);
	}

    glewExperimental = GL_TRUE;
	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(-1);
	}
}

int min(int a, int b)
{
  if(a<b)
    return a;
  else
    return b;
}

int max(int a, int b)
{
  if(a>b)
    return a;
  else
    return b;
}
void draw(GLuint& vaoObject, GLuint& vertexBufferObject, GLuint& indexBufferObject, unsigned int numberOfVertices)
{
    glBindVertexArray(vaoObject);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glDrawElements(GL_TRIANGLES, numberOfVertices, GL_UNSIGNED_INT,0);
}

void CleanVBOs(GLuint* vaoObjects, GLuint* vBO, const unsigned int& vBOsize,  GLuint* iBO, GLuint texture)
{
  glDeleteBuffers(vBOsize, vBO);
  glDeleteBuffers(maxLoD, iBO);
  glDeleteVertexArrays(vBOsize, vaoObjects);
  glDeleteTextures(1, &texture);
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
      case 'Y':
        oz+=5;
        break;
      case 'H':
        oz-=5;
        break;
      case GLFW_KEY_UP:
        iBOindex+=1;
        iBOindex%=maxLoD;
        break;
      case GLFW_KEY_DOWN:
        iBOindex-=1;
        iBOindex%=maxLoD;
        break;
      case GLFW_KEY_LEFT:
        if(alpha<10)
            alpha+=1;
        break;
      case GLFW_KEY_RIGHT:
        if(alpha>0)
            alpha-=1;
        break;
      case GLFW_KEY_SPACE:
        autolod=!autolod;
        break;
      case GLFW_KEY_ENTER:
        ball=1-ball;
        oz=0;
        oy=0;
        ox=0;
        z=0;
        if(ball==0)
        {
          x=startx;
          y=starty;
        }
        else
        {
          y=0;
          x=0;
        }
        break;


    }
  }
}

void loadVertices(const std::string& filename, std::vector<int>& arg, const bool& bin, const int& side, std::pair<int,int>& edge, int& height)
{
  std::cout << "Loading vertices from " << filename << "...\n";
  std::fstream file;
  memset(&arg[0], 0, sizeof(int)*side*side*3);
  int max_h=0;
  if(bin)
  {
    file.open(filename.c_str(), std::fstream::in | std::fstream::binary);
    for(int i=0, j=0; i<side*side; i++)
    {
      int p1, p2;
      p1 = file.get();
      p2 = file.get();
      assert(p1 != EOF && p2 != EOF);
      arg[j]   = i%side;
      j++;
      arg[j]  = i/side;
      j++;
      arg[j]  = p1*256+p2;
      if(arg[j]>9000 ||arg[j]<-500)
        arg[j]=0;
//      std::cout <<  i << "->"  <<  arg[j-2] << " " << arg[j-1] << " " << arg[j] << std::endl;
      if(arg[j]>max_h)
        max_h=arg[j];
      j++;
    }
  }
  else
  {
    file.open(filename.c_str(), std::fstream::in);
    for(int i=0, j=0; i<side*side; i++)
    {
      int p;
      file >> p;
      arg[j]  = i%side;
      j++;
      arg[j]  = i/side;
      j++;
      arg[j]  = p;
      if(arg[j]>9000 ||arg[j]<-500)
        arg[j]=0;
//      std::cout <<  i << "->"  <<  arg[j-3] << " " << arg[j-2] << " " << arg[j-1] << std::endl;
      if(arg[j]>max_h)
        max_h=arg[j];
      j++;
    }
  }
  file.close();
  std::cout << "Done. Maximal height is " << max_h << "\n";
  height = max_h;
  char r,p;
  sscanf(filename.c_str(), "%c%d%c%d",&r, &edge.first, &p, &edge.second); 
  if(r=='S')
    edge.first*=-1;
  if(p=='W')
    edge.second*=-1;
  std::cout << "Edges are: " << edge.first << " " << edge.second << std::endl; 
}

void genIndices(GLuint* indices, const unsigned int& side, const unsigned int& interval)
{
  std::cout << "Generating indices with interval:" << interval << "..\n";
  assert(interval!=0);
  bool last_chance_x=false,last_chance_y=true;
  int i=0;
  for(int y=side-interval-1,x=0,lasty=side-1, lastx; y>=0 || last_chance_y; y-=interval)
  {
    last_chance_x=true;
    if(y<0)
    {
      if(lasty==0)
        break;
      last_chance_y=false;
      y=0;
    }
    for(x=interval, lastx=0; x<side || last_chance_x; x+=interval)
    {
     if(x>=side)
      {
        if(lastx==side-1)
          break;
        last_chance_x=false;
        x=side-1;
      }
      indices[i] = (lasty)*side+(lastx);
      i++;
      indices[i] = (y)*side+(lastx);
      i++;
      indices[i] = (lasty)*side+x;
      i++;
      indices[i] = (lasty)*side+x;
      i++;
      indices[i] = (y)*side+(lastx);
      i++;
      indices[i] = (y)*side+x;
      i++;
      lastx=x;
//      std::cout << y << " " << x << "\n";
    }
    lasty=y;
  }
  std::cout << "Done Indices: " << i << ".\n";
}
