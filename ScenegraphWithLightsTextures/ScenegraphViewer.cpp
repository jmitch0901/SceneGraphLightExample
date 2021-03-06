/*
 * This is the main file. It sets up the window using sfml and initializes OpenGL extensions using glew
 *
 * We use sfml, an open-source library. SFML has very basic support for guis. There is SFGUI but I have not used it yet. 
 * SFML is by no means a full-fledged powerful UI framework, but it is very simple to use, and is cross-platform. But one of the things going for it is speed.
 * Feel free to use and modify this file for every project.
 *
 * SFML was written for C++ and opengl was written for C
 * In order to initialize OpenGL extensions to use OpenGL 4, we use glew. This simplifies setting it up for every program.
 **/
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "View.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "TwoDView.h"
using namespace std;


/* function prototypes */
void init(string& filename);
void resize(int w,int h);
void display(sf::RenderWindow *window);
void processEvent(sf::Event event,sf::RenderWindow& window);
void drawText(sf::RenderWindow& window,string text,int x,int y); 

View v; //an object to our View class that encapsulates everything that we do.
TwoDView v2;
bool navi;

sf::Font font;
sf::Clock sfclock;
int frames;
double frame_rate;
bool mousePressed;
int mouseX,mouseY;

int width, height, startX, startY;
int optimalPath;

string filename = "full-scene.xml";

int main(int argc, char *argv[])
{
	frames = 0;
	frame_rate = 0;
    // Request a 32-bits depth buffer when creating the window
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;
	contextSettings.majorVersion = 4;
	contextSettings.minorVersion = 0;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(700, 700), "Scenegraph Viewer", sf::Style::Default, contextSettings);
	resize(700,700);
  //  window.setVerticalSyncEnabled(true);

    // Make it the active window for OpenGL calls
 	window.setActive();   

    //glew currently has a bug that causes problems with 4.0. Setting this glew flag is the temporary solution for it
    glewExperimental = GL_TRUE;

    //initialize glew which initializes all the OpenGL extensions.
    if (glewInit()!=GLEW_OK)
    {
        cerr << "Unable to initialize GLEW...exiting" << endl;
        return EXIT_FAILURE;
    }

    //initialize stuff. This will likely change with every program.
    init(filename);

	
// Start game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
			processEvent(event,window);
        }

		if (window.isOpen())
			display(&window);
    }

    

    return EXIT_SUCCESS;
}

/*
 * This will be the function that processes any events
*/

void processEvent(sf::Event event,sf::RenderWindow& window)
{
	// Close window : exit
	switch(event.type)
	{
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::KeyPressed:
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			window.close();
			break;
		case sf::Keyboard::R:
			v.openFile(filename);
			break;
		case sf::Keyboard::Num1:
			v.helloMrMarshmellow();
			break;
		case sf::Keyboard::Num2:
			v.goodbyeMrMarshmellow();
			break;
		case sf::Keyboard::Left:
			navi = v2.navigate(0);
			v.moveMrMarshmellow(0, navi);
			break;
		case sf::Keyboard::Right:
			navi = v2.navigate(1);
			v.moveMrMarshmellow(1, navi);
			break;
		case sf::Keyboard::Up:
			navi = v2.navigate(2);
			v.moveMrMarshmellow(2, navi);
			break;
		case sf::Keyboard::Down:
			navi = v2.navigate(3);
			v.moveMrMarshmellow(3, navi);
			break;
		}
		
		break;
	case sf::Event::Resized:
		resize(event.size.width, event.size.height);
		break;
		case sf::Event::MouseButtonPressed:
		mousePressed = true;
		mouseX = sf::Mouse::getPosition(window).x;
		mouseY = window.getSize().y - sf::Mouse::getPosition(window).y;
		v.mousepress(mouseX,mouseY);
		break;
	case sf::Event::MouseButtonReleased:
		mousePressed = false;
		break;
	case sf::Event::MouseMoved:
		if (mousePressed)
		{
			mouseX = sf::Mouse::getPosition(window).x;
			mouseY = window.getSize().y - sf::Mouse::getPosition(window).y;
			v.mousemove(mouseX,mouseY);//sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y);
			
		}
		break;
	
	}
}

/*
 * This function helps us to draw text over a part of the window
 */

void drawText(sf::RenderWindow *window,string text,int x,int y)
{
	// Create some text to draw on top of our OpenGL object
  
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    sf::Text textobj(text, font);
    textobj.setColor(sf::Color(255, 255, 255, 255));

	
    textobj.setPosition((float)x,(float)y);

	textobj.setCharacterSize(18);

	window->pushGLStates();
	window->resetGLStates();
	window->draw(textobj);
    window->popGLStates();
}


/*
 * This will be our display function. Whenever glut needs to redraw the window, it will call this function.
 * The name of this function (display) is of no consequence: you can name it whatever you want, so long as the constraints below are obeyed.
 * Thus all your rendering code should be in this function, or should be called from this function.
 *
*/

void display(sf::RenderWindow *window)
{
	if (frames==0)
		sfclock.restart();
	
	// Draw using SFML
	window->pushGLStates();
	window->resetGLStates();
	//insert SFML drawing code here (any part you are using that does not involve opengl code)
	window->popGLStates();

	//set up the background color of the window. This does NOT clear the window. Right now it is (0,0,0) which is black
	/*glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //this command actually clears the window.
	glEnable(GL_DEPTH_TEST);
	v.draw(); //simply delegate to our view class that has all the data and does all the rendering
	*/
	//set up the background color of the window. This does NOT clear the window. Right now it is (0,0,0) which is black
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //this command actually clears the window.
	glEnable(GL_DEPTH_TEST);
	glViewport(0,0,width,height);
	v.draw(); //simply delegate to our view class that has all the data and does all the rendering



	glScissor(startX,startY,width-startX,height-startY);
	glEnable(GL_SCISSOR_TEST);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glViewport(startX,startY,width-startX,height-startY);
	v2.draw();
	glDisable(GL_SCISSOR_TEST);

	if (frames>500)
	{
		sf::Time t = sfclock.getElapsedTime();
		frame_rate = frames/t.asSeconds();
		frames = 0;
	}
	else
	{
		frames++;
	}

	stringstream str;
	int steps = v2.getSteps();
	str << "Optimal Path: " << optimalPath << "\tSteps: " << steps;
	//str1 << "Frame rate: " << frame_rate;

	//cout << str.str() << endl;
	// Draw some text on top of our OpenGL object
	
	drawText(window,str.str(),width*.1,30);
	// Finally, display the rendered frame on screen
	
	window->display();
//	cout << "Rendering" << endl;
}

/*
 * This function will be called by glut whenever the window resizes. This happens initially when it creates the window, and later if the user manually resizes the window or maximizes it.
 * The name of this function (resize) is of no consequence: you can name it whatever you want, so long as the constraints below are obeyed.
 * This function must take two integers as parameters, the width and height of the resized window in that order
 * This function must return void.
 **/
void resize(int w,int h)
{
    //delegate to our view class.
    v.resize(w,h);

	width=w;
	height=h;

	startX = (2.0f/4.0f)*w;
	startY = (2.0f/4.0f)*h;

	int diff = abs((height - startY) - (width - startX));
	if((height - startY) < (width - startX)){
		startX += diff;
	} else{
		startY += diff;
	}

    v2.resize(w - startX,h - startY);

}

void init(string& filename)
{
    int major,minor;
    v.getOpenGLVersion(&major,&minor);

    cout <<"Opengl version supported : "<<major<<"."<<minor<<endl;
    v.getGLSLVersion(&major,&minor);
    cout << "GLSL version supported : "<<major<<"."<<minor << endl;
    //delegate to our view class to do all the initializing
	v.initialize();
	v.openFile(filename);

	v2.initialize("maze-50x50.txt", "maze-50x50-paths.txt");
	optimalPath = v2.getOptimalPath();

	if (!font.loadFromFile("resources/sansation.ttf")){
		cout<<"error loading font!"<<endl;
		return;
	}

}
