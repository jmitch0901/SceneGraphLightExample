#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "utils/Light.h"

Scenegraph::Scenegraph()
{
    root = NULL;
}

void Scenegraph::makeScenegraph(Node *r)
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }
    this->root = r;

	if (this->root!=NULL)
		this->root->setScenegraph(this);


	initAnimate();
}



Scenegraph::~Scenegraph()
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }

	for (map<string,graphics::Texture *>::iterator it=textures.begin();it!=textures.end();it++)
	{
		delete it->second;
	}
}

void Scenegraph::initShaderProgram(GLint program)
{
	modelviewLocation = glGetUniformLocation(program,"modelview");
	//projectionLocation = glGetUniformLocation(program,"projection");
    normalMatrixLocation = glGetUniformLocation(program,"normalmatrix");
    numLightsLocation = glGetUniformLocation(program,"numLights");


    mat_ambientLocation = glGetUniformLocation(program,"material.ambient");
    mat_diffuseLocation = glGetUniformLocation(program,"material.diffuse");
    mat_specularLocation = glGetUniformLocation(program,"material.specular");
    mat_shininessLocation = glGetUniformLocation(program,"material.shininess");

    //objectColorLocation = glGetUniformLocation(shaderProgram,"vColor");

	texturematrixLocation = glGetUniformLocation(program,"texturematrix");
	textureLocation = glGetUniformLocation(program,"image");

}


vector<graphics::Light> Scenegraph::gatherLightingObjects(){

	if (root!=NULL)
    {
		return root->grabLightingObjects();  

    } else {
		cout<<"For some reason, root in the scene graph was NULL!"<<endl;
	}

	return vector<graphics::Light>();
}

void Scenegraph::draw(stack<glm::mat4>& modelView)
{


    if (root!=NULL)
    {
        root->draw(modelView);
    } else {
		cout<<"ROOT WAS NULL FOR DRAWING"<<endl;
	}

	/*if (root!=NULL)
	{
		root->updateBB();
		root->drawBB(modelView);
	}*/
}

void Scenegraph::initAnimate(){

	central_spin = dynamic_cast<TransformNode *>(root->getNode("animate-me-m8"));

	ra = dynamic_cast<TransformNode *>(root->getNode("right-amusement-arm"));
	ba = dynamic_cast<TransformNode *>(root->getNode("back-amusement-arm"));
	la = dynamic_cast<TransformNode *>(root->getNode("left-amusement-arm"));
	fa = dynamic_cast<TransformNode *>(root->getNode("front-amusement-arm"));

	pistonTranslate = dynamic_cast<TransformNode *>(root->getNode("transform-movable-stem"));
	diskRotate = dynamic_cast<TransformNode*>(root->getNode("transform-top-disk"));

}


void Scenegraph::animate(float time)
{

	
	if(pistonTranslate!=NULL){		
		pistonTranslate->setAnimationTransform(glm::translate(glm::mat4(1.0f),glm::vec3(0,glm::sin(time)*30,0)));
	}
	
	
	if(diskRotate!=NULL){
		diskRotate->setAnimationTransform(glm::rotate(glm::mat4(1.0f),-1.0f * time,glm::vec3(0,1,0)));
	}


	float PI = 3.14159f;

	if(central_spin!=NULL){
		central_spin->setAnimationTransform(glm::rotate(glm::mat4(1.0),time/*/75.0f*/, glm::vec3(0,1,0)));
	}

	if(ra!=NULL){
		ra->setAnimationTransform(glm::rotate(glm::mat4(1.0),-cos(time/*/50.0f*/)*(PI / 12.0f) + (PI / 12.0f), glm::vec3(0,0,1)));
	}
	if(ba!=NULL){
		ba->setAnimationTransform(glm::rotate(glm::mat4(1.0),cos(time/*/50.0f*/)*(PI / 12.0f) - (PI / 12.0f), glm::vec3(1,0,0)));
	}
	if(la!=NULL){
		la->setAnimationTransform(glm::rotate(glm::mat4(1.0),-cos(time/*/50.0f*/)*(PI / 12.0f) + (PI / 12.0f), glm::vec3(0,0,-1)));
	}
	if(fa!=NULL){
		fa->setAnimationTransform(glm::rotate(glm::mat4(1.0),cos(time/*/50.0f*/)*(PI / 12.0f) - (PI / 12.0f), glm::vec3(-1,0,0)));
	}
}

