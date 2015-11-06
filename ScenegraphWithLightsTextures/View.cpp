#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include "View.h"
#include <GL/glew.h>
#include <cstdlib>
#include <fstream>
#include <vector>
using namespace std;

//glm headers to access various matrix producing functions, like ortho below in resize
#include <glm/gtc/matrix_transform.hpp>
//the glm header required to convert glm objects into normal float/int pointers expected by OpenGL
//see value_ptr function below for an example
#include <glm/gtc/type_ptr.hpp>

#include "SceneXMLReader.h"

View::View()
{
    trackballTransform = glm::mat4(1.0);
	time = 0.0;
	debugBool=false;
	directionMrMarshmellowIsFacing=1;//Direction: 0 ^	1 >		2 v		3 <
	cameraHeight = 40;
	marshDir = glm::vec3(100000,cameraHeight,0);
	marshPos = glm::vec3(-441,cameraHeight,297);
	tnMarshmellow=NULL;
	isInFPS=false;
}

View::~View()
{
    
}

void View::resize(int w, int h)
{
    //record the new dimensions
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;

    /*
     * This program uses orthographic projection. The corresponding matrix for this projection is provided by the glm function below.
     *The last two parameters are for the near and far planes.
     *
     *Very important: the last two parameters specify the position of the near and far planes with respect
     *to the eye, in the direction of gaze. Thus positive values are in front of the camera, and negative
     *values are in the back!
     **/
   
    while (!proj.empty())
        proj.pop();

	//proj.push(glm::ortho(-200.0f,200.0f,-200.0f*WINDOW_HEIGHT/WINDOW_WIDTH,200.0f*WINDOW_HEIGHT/WINDOW_WIDTH,0.1f,10000.0f));
    proj.push(glm::perspective(120.0f*3.14159f/180,(float)WINDOW_WIDTH/WINDOW_HEIGHT,0.1f,10000.0f));
}

void View::openFile(string filename)
{

	/*if(!debugBool)
		cout<<glGetError()<<endl;*/



	SceneXMLReader reader;
	cout << "Loading...";
	reader.importScenegraph(filename,sgraph);
	cout << "Done" << endl;

	/*if(!debugBool)
		cout<<glGetError()<<endl;*/




	//GATHER THE LIGHTING AND INIT LIGHTING FOR GPU

	//cout<<"ABOUT TO GATHER LIGHTING"<<endl;
	gatheredLights = sgraph.gatherLightingObjects();
	//cout<<"GOT "<<gatheredLights.size()<<" LIGHTS!"<<endl;

	//cout<<"Your Lights: "<<endl;


	//FOR DEBUGGING
	for(int i = 0 ; i < gatheredLights.size(); i++){


		glm::vec4 pos = gatheredLights[i].getPosition();
		
		//cout<<"Light["<<i<<"]: Position(x,y,z)"<<pos[0]<<", "<<pos[1]<<", "<<pos[2]<<endl;
	}
	//


	//cout<<"Error: " <<glGetError()<<endl;

	for (int i=0;i<gatheredLights.size();i++)
    {

		LightLocation lightLocation;
        stringstream name;

        name << "light[" << i << "].ambient";

        lightLocation.ambientLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());

        name << "light[" << i << "].diffuse";

        lightLocation.diffuseLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());

        name << "light[" << i << "].specular";

        lightLocation.specularLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());

        name << "light[" << i << "].position";

        lightLocation.positionLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());


		lightLocations.push_back(lightLocation);
    }

	/*if(!debugBool)
		cout<<glGetError()<<endl;*/

	
	//cout<<glGetError()<<endl;
	//END LIGHTING GATHERING

	/*mrMarshmellow = sgraph.getMrMarshmellow();

	if(mrMarshmellow!=NULL){
		cout<<"GOT EMM!"<<endl;
		cout<<mrMarshmellow->getName()<<endl;
	}*/

	tnMarshmellow = dynamic_cast<TransformNode *>(sgraph.getNode("mr_marshmellow"));
	



}

void View::initialize()
{
    //populate our shader information. The two files below are present in this project.
    ShaderInfo shaders[] =
    {
        {GL_VERTEX_SHADER,"phong-multiple.vert"},
        {GL_FRAGMENT_SHADER,"phong-multiple.frag"},
        {GL_NONE,""} //used to detect the end of this array
    };

     program = p_program = createShaders(shaders);

	if (p_program<=0)
		exit(1);

	shaders[0].filename = "gouraud-multiple.vert";
	shaders[1].filename = "gouraud-multiple.frag";

    g_program = createShaders(shaders);

	if (g_program<=0)
		exit(1);

    glUseProgram(program);

	//cout<<glGetError()<<endl;

	projectionLocation = glGetUniformLocation(program,"projection");

	sgraph.initShaderProgram(program);

	//cout<<glGetError()<<endl;

	/*if(!debugBool)
		cout<<glGetError()<<endl;*/

	
	glUseProgram(0);
}

void View::draw()
{
	time += 0.01f;
	sgraph.animate(time);
	gatheredLights = sgraph.gatherLightingObjects();
    /*
     *The modelview matrix for the View class is going to store the world-to-view transformation
     *This effectively is the transformation that changes when the camera parameters chang
     *This matrix is provided by glm::lookAt
     */
	glUseProgram(program);

    while (!modelview.empty())
        modelview.pop();

    modelview.push(glm::mat4(1.0));

	//Stoer the info for both FPS and non-FPS

	remembered3Pview = glm::lookAt(glm::vec3(0,350,.5),glm::vec3(0,0,0),glm::vec3(0,1,0)) /* trackballTransform*/;

	remembered1Pview = glm::lookAt(marshPos, marshDir,glm::vec3(0,1,0));

	if(isInFPS){
		modelview.top() = modelview.top() * remembered1Pview;
		
	} else {

		modelview.top() = modelview.top() * remembered3Pview;
	}


	/*if(!debugBool)
		cout<<glGetError()<<endl;*/

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    sgraph.draw(modelview);

	/*if(!debugBool)
		cout<<glGetError()<<endl;*/


	gatheredLights = sgraph.gatherLightingObjects();
	
	glUniformMatrix4fv(projectionLocation,1,GL_FALSE,glm::value_ptr(proj.top()));

	//START LIGHTING
	glUniform1i(sgraph.numLightsLocation,gatheredLights.size());

	/*if(!debugBool)
		cout<<glGetError()<<endl;*/

	for (int i=0;i<lightLocations.size();i++)
    {
		//The equivalent to glBufferData, accept for lighting
		glUniform3fv(lightLocations[i].ambientLocation,1,glm::value_ptr(gatheredLights[i].getAmbient()));
        glUniform3fv(lightLocations[i].diffuseLocation,1,glm::value_ptr(gatheredLights[i].getDiffuse()));
        glUniform3fv(lightLocations[i].specularLocation,1,glm::value_ptr(gatheredLights[i].getSpecular()));
		glUniform4fv(lightLocations[i].positionLocation,1,glm::value_ptr(modelview.top()*gatheredLights[i].getPosition()));
    }
	/*if(!debugBool)
		cout<<glGetError()<<endl;*/

	//END LIGHTING


    glFinish();
	glUseProgram(0);
    modelview.pop();
	debugBool=true;
}

void View::helloMrMarshmellow(){
	//Let the camera be in the coordinate system of mr. marshmellow
	if(!isInFPS){
		isInFPS = !isInFPS;
	}
}

void View::goodbyeMrMarshmellow(){
	//Say goodbye to mr. marshmellow, as you rudely rip the camera away from his delicious face.
	if(isInFPS){
		isInFPS = !isInFPS;
	}
}

void View::moveMrMarshmellow(int inputKey, bool valid){


	if(tnMarshmellow==NULL){
		cout<<"OH NO! Mr marshmellow was NULL!"<<endl;
		return;
	}

	if(valid){
		switch(inputKey){
		case 0://left arrow
			directionMrMarshmellowIsFacing--;

			if(directionMrMarshmellowIsFacing==-1){
				directionMrMarshmellowIsFacing = 3;
			}
			//Just do a rotate left 90 degrees
			setFPSCameraDirection();

			break;
		case 1://right arrow
			directionMrMarshmellowIsFacing++;

			if(directionMrMarshmellowIsFacing==4){
				directionMrMarshmellowIsFacing=0;
			}
			setFPSCameraDirection();
			//just do a rotate right 90 degrees

			break;
		case 2://up arrow
			//Just translate mr. marshmellow up and his deliciousness by a factor of a cell height
			//interpretMrMarshmellowsTranslation(2);
			moveForwards();
			break;
		case 3://down arrow
			//Just translate mr. marshmellow down and his deliciousness by a factor of a cell height
			//interpretMrMarshmellowsTranslation(3);
			moveBackwards();
			break;

		}
	}
}

void View::moveForwards(){
	int moveMagnitude = 18;
	int moveMagnitude2 = moveMagnitude/6;
	switch(directionMrMarshmellowIsFacing){
		case 0:
			marshPos.z -= moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(0,0,-moveMagnitude2)) * tnMarshmellow->getTransform());
			break;
		case 1:
			marshPos.x += moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(moveMagnitude2,0,0)) * tnMarshmellow->getTransform());
			break;
		case 2:
			marshPos.z += moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(0,0,moveMagnitude2)) * tnMarshmellow->getTransform());
			break;
		case 3:
			marshPos.x -= moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(-moveMagnitude2,0,0)) * tnMarshmellow->getTransform());
			break;
	}
}

void View::moveBackwards(){
	int moveMagnitude = 18;
	int moveMagnitude2 = moveMagnitude/6;
	switch(directionMrMarshmellowIsFacing){
		case 2:
			marshPos.z -= moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(0,0,-moveMagnitude2)) * tnMarshmellow->getTransform());
			break;
		case 3:
			marshPos.x += moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(moveMagnitude2,0,0)) * tnMarshmellow->getTransform());
			break;
		case 0:
			marshPos.z += moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(0,0,moveMagnitude2)) * tnMarshmellow->getTransform());
			break;
		case 1:
			marshPos.x -= moveMagnitude;
			tnMarshmellow->setTransform(glm::translate(glm::mat4(1.0), glm::vec3(-moveMagnitude2,0,0)) * tnMarshmellow->getTransform());
			break;
	}
}

void View::setFPSCameraDirection(){
	switch (directionMrMarshmellowIsFacing)
	{
	case 0:// ^
		marshDir = glm::vec3(0,cameraHeight,-100000);
		break;
	case 1:// >
		marshDir = glm::vec3(100000,cameraHeight,0);
		break;
	case 2:// v
		//mrMarshmellow->setTransform(mrMarshmellow->getTransform() * glm::translate(glm::mat4(1.0f),glm::vec3(6.0f,0,0)));
		marshDir = glm::vec3(0,cameraHeight,100000);
		break;
	case 3:// <
		marshDir = glm::vec3(-100000,cameraHeight,0);
		break;
	default:
		break;
	}
}

void View::mousepress(int x, int y)
{
    prev_mouse = glm::vec2(x,y);
}

void View::mousemove(int x, int y)
{
	if(isInFPS) //We don't want the trackball updating when we are in FPS mode.
		return;

    int dx,dy;

    dx = x - prev_mouse.x;
    dy = (y) - prev_mouse.y;

	if ((dx==0) && (dy==0))
		return;

    //(-dy,dx) gives the axis of rotation

    //the angle of rotation is calculated in radians by assuming that the radius of the trackball is 300
    float angle = sqrt((float)dx*dx+dy*dy)/300;
   
    prev_mouse = glm::vec2(x,y);
	
    trackballTransform = glm::rotate(glm::mat4(1.0),angle,glm::vec3(-dy,dx,0)) * trackballTransform;
}

/*
 *This is a helper function that will take shaders info as a parameter, compiles them and links them
 *into a shader program.
 *
 *This function is standard and should not change from one program to the next.
 */

GLuint View::createShaders(ShaderInfo *shaders)
{
    ifstream file;
    GLuint shaderProgram;
    GLint linked;

    ShaderInfo *entries = shaders;

    shaderProgram = glCreateProgram();


    while (entries->type !=GL_NONE)
    {
        file.open(entries->filename.c_str());
        GLint compiled;


        if (!file.is_open())
            return false;

        string source,line;


        getline(file,line);
        while (!file.eof())
        {
            source = source + "\n" + line;
            getline(file,line);
        }
        file.close();


        const char *codev = source.c_str();


        entries->shader = glCreateShader(entries->type);
        glShaderSource(entries->shader,1,&codev,NULL);
        glCompileShader(entries->shader);
        glGetShaderiv(entries->shader,GL_COMPILE_STATUS,&compiled);

        if (!compiled)
        {
            printShaderInfoLog(entries->shader);
            for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
            {
                glDeleteShader(processed->shader);
                processed->shader = 0;
            }
            return 0;
        }
        glAttachShader( shaderProgram, entries->shader );
        entries++;
    }

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&linked);

    if (!linked)
    {
        printShaderInfoLog(entries->shader);
        for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
        {
            glDeleteShader(processed->shader);
            processed->shader = 0;
        }
        return 0;
    }

    return shaderProgram;
}

void View::printShaderInfoLog(GLuint shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    GLubyte *infoLog;

    glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infologLen);
    //	printOpenGLError();
    if (infologLen>0)
    {
        infoLog = (GLubyte *)malloc(infologLen);
        if (infoLog != NULL)
        {
            glGetShaderInfoLog(shader,infologLen,&charsWritten,(char *)infoLog);
            printf("InfoLog: %s\n\n",infoLog);
            free(infoLog);
        }

    }
//	printOpenGLError();
}

void View::getOpenGLVersion(int *major,int *minor)
{
    const char *verstr = (const char *)glGetString(GL_VERSION);
    if ((verstr == NULL) || (sscanf_s(verstr,"%d.%d",major,minor)!=2))
    {
        *major = *minor = 0;
    }
}

void View::getGLSLVersion(int *major,int *minor)
{
    int gl_major,gl_minor;

    getOpenGLVersion(&gl_major,&gl_minor);
    *major = *minor = 0;

    if (gl_major==1)
    {
        /* GL v1.x can only provide GLSL v1.00 as an extension */
        const char *extstr = (const char *)glGetString(GL_EXTENSIONS);
        if ((extstr!=NULL) && (strstr(extstr,"GL_ARB_shading_language_100")!=NULL))
        {
            *major = 1;
            *minor = 0;
        }
    }
    else if (gl_major>=2)
    {
        /* GL v2.0 and greater must parse the version string */
        const char *verstr = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        if ((verstr==NULL) || (sscanf_s(verstr,"%d.%d",major,minor) !=2))
        {
            *major = 0;
            *minor = 0;
        }
    }
}












