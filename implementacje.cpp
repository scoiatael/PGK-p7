#include "definicje.h"

int mcc::get_time()
{
  timeval timer;
  gettimeofday(&timer, NULL);
  return timer.tv_sec*1000000 + timer.tv_usec;
}
float mcc::get_timef()
{
  return (float)mcc::get_time()/10000.0f;
}
int mcc::make_lock(const float& timeout)
{
  locks.push_back(std::pair<float,float>(0.,timeout));
  return locks.size()-1;
}
int mcc::get_lock(const int& ID)
{
  if(ID<locks.size())
    return locks[ID].first;
  return 0;
}
void mcc::reset_lock(const int& ID)
{
  if(ID < locks.size())
    locks[ID].first = locks[ID].second;
}
void mcc::update()
{
  int cur_time = get_time();
  int interval = cur_time-last_update;
  last_update = cur_time;
  for(unsigned int i=0; i<locks.size();i++)
    locks[i].first-=interval;
}

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
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		exit(-1);
	}

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(-1);
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

void genIndices(std::vector< GLuint >& indices, const unsigned int& side, const unsigned int& interval)
{
  std::cout << "Generating indices with interval:" << interval << "..\n";
  assert(interval!=0);
  bool last_chance_x=false,last_chance_y=true;
  int i=0;
  for(int y=side-interval,x=0,lasty=side-1, lastx; y>=0 || last_chance_y; y-=interval)
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
/*      for(int j=6;j>0;j--)
        std::cout << j << "->" << indices[i-j] << " ";
      std::cout << std::endl;*/
    }
    lasty=y;
  }
  std::cout << "Done Indices: " << i << ".\n";
}
