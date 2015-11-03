#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#define GLM_FORCE_RADIANS

#include <map>
#include <stack>
#include <vector>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include "utils/Object.h"
#include "Node.h"
#include "utils/Texture.h"

class Scenegraph
{    
    friend class LeafNode;
	friend class GroupNode;
	friend class TransformNode;
public:
    Scenegraph();
    ~Scenegraph();
    void makeScenegraph(Node *root);
    void initShaderProgram(GLint shaderProgram);
    void draw(stack<glm::mat4>& modelView);
	vector<glm::mat4> gatherLightingMatrices();
	vector<graphics::Light> gatherLightingObjects();

	void addInstance(graphics::Object *in)
	{
		if (instances.count(in->getName())<1)
			instances[in->getName()] = in;
	}

	void addTexture(graphics::Texture *tex)
	{
		if (instances.count(tex->getName())<1)
			textures[tex->getName()] = tex;
	}

	void animate(float t);

	graphics::Object *getInstance(string name)
	{
		if (instances.count(name)<1)
			return NULL;

		return instances[name];
	}

	graphics::Texture *getTexture(string name)
	{
		if (textures.count(name)<1)
			return NULL;

		return textures[name];
	}

private:
    Node *root;
	map<string,graphics::Object *> instances;
	map<string,graphics::Texture *> textures;

	

	//GLint objectColorLocation,modelviewLocation;

	GLint modelviewLocation,normalMatrixLocation;
    GLint numLightsLocation;
    GLint mat_ambientLocation,mat_diffuseLocation,mat_specularLocation,mat_shininessLocation;
};

#endif // SCENEGRAPH_H
