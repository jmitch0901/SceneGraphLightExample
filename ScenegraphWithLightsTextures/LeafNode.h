#ifndef _LEAF_H_
#define _LEAF_H_
#include "node.h"
#include "utils/Object.h"
#include "utils/Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/Texture.h"


class LeafNode : public Node
{

protected:
	graphics::Object *instanceOf;
	graphics::Material material;

public:

	LeafNode(graphics::Object *instanceOf,Scenegraph *graph,string name="")
		:Node(graph,name)
	{
		this->instanceOf = instanceOf;
		//default material
		material.setAmbient(1.0f,0.6f,0.6f);
		material.setDiffuse(1.0f,0.6f,0.6f);
		material.setSpecular(0.2f,0.1f,0.1f);
		material.setShininess(1);
	}

	~LeafNode(void)
	{
	}

	Node *clone()
	{
		LeafNode *newclone = new LeafNode(instanceOf,scenegraph,name);
		newclone->setMaterial(material);

		return newclone;
	}

	virtual vector<graphics::Light> grabLightingObjects(){
		return lighting;
	}


	//WE NEED DIRECTION AS WELL!
	virtual vector<glm::mat4> grabLightMatrices(){

		vector<glm::mat4> returnMe;
		
		for (int i=0;i<lighting.size();i++)
		{

			glm::mat4 lightTransform = glm::translate(glm::mat4(1.0f),glm::vec3(lighting[i].getPosition()));
			//returnMe.push_back(glm::mat4(1.0f) * lighting[i].getPosition());

			returnMe.push_back(lightTransform);
		}		

		
		return returnMe;

	}

	virtual void draw(stack<glm::mat4> &modelView)
    {

		GLuint a;
        if (instanceOf!=NULL)
		{
			//START DRAW HERE!

			 //get the color
            glm::vec4 color = material.getAmbient();


			//The total transformation is whatever was passed to it, with its own transformation
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));
			glUniformMatrix4fv(scenegraph->normalMatrixLocation,1,GL_FALSE,glm::value_ptr(glm::transpose(glm::inverse(modelView.top()))));

			glUniform3fv(scenegraph->mat_ambientLocation,1,glm::value_ptr(material.getAmbient()));
			glUniform3fv(scenegraph->mat_diffuseLocation,1,glm::value_ptr(material.getDiffuse()));
			glUniform3fv(scenegraph->mat_specularLocation,1,glm::value_ptr(material.getSpecular()));
			glUniform1f(scenegraph->mat_shininessLocation,material.getShininess());
			
			//glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));



			instanceOf->draw(GL_TRIANGLES);		
		}
    }


	//CHANGE THIS DRAW AS WELL
	virtual void drawBB(stack<glm::mat4>& modelView)
	{
		/*if (bbDraw)
		{
			glm::mat4 bbTransform;

			bbTransform = glm::translate(glm::mat4(1.0),0.5f*(minBounds+maxBounds)) * glm::scale(glm::mat4(1.0),maxBounds-minBounds);
			glm::vec4 color = glm::vec4(1,1,1,1);
			//set the color for all vertices to be drawn for this object
			//glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(color));
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top() * bbTransform));
			scenegraph->getInstance("box")->draw(GL_TRIANGLES);        		
		}*/
	}
	
	virtual void updateBB()
	{
		minBounds = instanceOf->getMinimumWorldBounds().xyz();
		maxBounds = instanceOf->getMaximumWorldBounds().xyz();
	}

	glm::vec4 getColor()
	{
		return material.getAmbient();
	}

	/*
		*Set the material of each vertex in this object
		*/
	virtual void setMaterial(graphics::Material& mat)
	{
		material = mat;
	}

	/*
	* gets the material
	*/
	graphics::Material getMaterial()
	{
		return material;
	}


	/*
		UPDATE THIS WHEN WE NEED TEXTURES
	*/
	void setTexture(graphics::Texture *tex)
	{
		cout << "Texture set to " << tex->getName() << endl;
	}


};
#endif
