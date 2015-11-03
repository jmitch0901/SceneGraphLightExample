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
}


vector<graphics::Light> Scenegraph::gatherLightingObjects(){

	if (root!=NULL)
    {
		return root->grabLightingObjects();      
    }

	return vector<graphics::Light>();
}

void Scenegraph::draw(stack<glm::mat4>& modelView)
{

	//Get the light vector for ALL nodes, with respect
	//to the view's coordinate system.

    if (root!=NULL)
    {
        root->draw(modelView);
    }

	/*if (root!=NULL)
	{
		root->updateBB();
		root->drawBB(modelView);
	}*/
}

void Scenegraph::animate(float time)
{
}

