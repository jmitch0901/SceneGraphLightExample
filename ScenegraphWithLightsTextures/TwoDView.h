#ifndef TWO_D_VIEW_H
#define TWO_D_VIEW_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include <glm/glm.hpp>

class VAttribs
{
public:
    float position[4];		//x,y,z,w
    float color[3];			//color as r,g,b
};

class PathCoords{
	public:
		int row, col;

		/*bool operator<(const PathCoords& other) const{
			if(row == other.row){
				return col < other.col;
			}

			return row < other.row;
		}*/



};

/*struct CellComparer{
	bool operator()(const PathCoords& start, const PathCoords& next) const{
		if(start.row == next.row){
			return start.col < next.col;
		}

		return start.row < next.row;
	}
};*/

class TwoDView
{
  
	#define BUFFER_OFFSET(offset) ((void *)(offset))

    enum Buffer_IDs {ArrayBuffer,IndexBuffer,NumBuffers};

    typedef struct {
        GLenum       type; 
		string		filename; 
        GLuint       shader; 
    } ShaderInfo;


	public:
		TwoDView();
		void resize(int w,int h);
		void initialize(string fileName, string pathFile);
		void draw();
		void getOpenGLVersion(int *major,int *minor);
		void getGLSLVersion(int *major,int *minor);

		void dragRect(int x1, int y1, int x2, int y2);
		void finalRect(int x1, int y1, int x2, int y2);

	protected:
		GLuint createShaders(ShaderInfo *shaders);
		void printShaderInfoLog(GLuint shader);

	private:
		int WINDOW_WIDTH,WINDOW_HEIGHT;
		double SMALL_EDGE;
		GLuint program;
		GLuint vao;
		GLuint vbo[NumBuffers];
		GLint projectionLocation,modelViewLocation,vPositionLocation,vColorLocation;
		glm::mat4 proj,modelView;

		int row, col;
		vector<int> start, end;
		vector< vector<int> > mazeArr;
		vector< vector<PathCoords> > path;


		void parseFile(string fileName);
		void parsePathFile(string pathFile);
		vector<GLuint> indices;
		int numOfVertices;
		void pushWalls(int i, int j, bool walls[4]);
		void pushPath(int i, int j);
		bool rectDrawn;
		vector<VAttribs> vdata;
		string mazeFileName;
};

#endif