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

	
private:
	graphics::Texture *texture;

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

		this->instanceOf->setMaterial(material);
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


	//DIRECTION AS WELL
	virtual vector<graphics::Light> grabLightingObjects(){
		return lighting;
	}
	

	virtual void draw(stack<glm::mat4> &modelView)
    {

		GLuint a;
        if (instanceOf!=NULL)
		{
			//START DRAW HERE!

			 //get the color
            //glm::vec4 color = material.getAmbient();

			instanceOf->setMaterial(material);


			//The total transformation is whatever was passed to it, with its own transformation
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()*instanceOf->getTransform()));
			glUniformMatrix4fv(scenegraph->normalMatrixLocation,1,GL_FALSE,glm::value_ptr(glm::transpose(glm::inverse(modelView.top()*instanceOf->getTransform()))));

			glUniform3fv(scenegraph->mat_ambientLocation,1,glm::value_ptr(material.getAmbient()));
			glUniform3fv(scenegraph->mat_diffuseLocation,1,glm::value_ptr(material.getDiffuse()));
			glUniform3fv(scenegraph->mat_specularLocation,1,glm::value_ptr(material.getSpecular()));
			glUniform1f(scenegraph->mat_shininessLocation,material.getShininess());
			
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));






			//glEnable(GL_TEXTURE_2D);//Tell openGL don't ignore my texture mapping commands
			//glActiveTexture(GL_TEXTURE0);//Starts at 0 -> 8, can use to layer mroe textures


			//glBindTexture(GL_TEXTURE_2D,textureID);//this texture is associated with texzture 0
			//glUniform1i(textureLocation,0); //bind GL_TEXTURE0 to sampler2D (whatever is bound to GL_TEXTURE0)
			


				
			
			if(texture!=NULL){
				
				glEnable(GL_TEXTURE_2D);//Tell openGL don't ignore my texture mapping commands
				glActiveTexture(GL_TEXTURE0);//Starts at 0 -> 8, can use to layer mroe textures


				glBindTexture(GL_TEXTURE_2D,texture->getTextureID());//this texture is associated with texzture 0
				glUniform1i(scenegraph->textureLocation,0); //bind GL_TEXTURE0 to sampler2D (whatever is bound to GL_TEXTURE0)



				if(texture->getName() == "floor"){
					//cout<<"FLOOR HIT"<<endl;
					//instanceOf->setTextureTransform(glm::scale(glm::mat4(1.0),glm::vec3(80,80,80)));
					glUniformMatrix4fv(scenegraph->texturematrixLocation,1,GL_FALSE,glm::value_ptr(glm::scale(glm::mat4(1.0f),glm::vec3(80,80,80))*instanceOf->getTextureTransform()));
				} else {
					glUniformMatrix4fv(scenegraph->texturematrixLocation,1,GL_FALSE,glm::value_ptr(glm::scale(glm::mat4(1.0f),glm::vec3(2,2,2))*instanceOf->getTextureTransform()));
				}
				
			} else {
				cout<<"THERE WAS NO TEXTURE, TARD"<<endl;
			}
		}

		instanceOf->draw(GL_TRIANGLES);	
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
		instanceOf->setMaterial(mat);
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
		//cout << "Texture set to " << tex->getName() << endl;
		texture=tex;

		/*this->texture=tex;
		sf::Image im;
	
		im.loadFromFile("checkerboard.png");

		//origin of the image is top left, all our texture coordinates are with respect to bottom left, so flip image vertically
		im.flipVertically();

		
    
		glGenTextures(1,&textureID); //get a unique texture ID
		glBindTexture(GL_TEXTURE_2D,textureID);// bind this texture as "current". All texture commands henceforth will apply to this ID


		//come back
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_REPEAT MEANING if larger than value 1, then REPEAT the pattern! DO this for s coordinate!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL CLAMP means anything < 0 is 0, anything > 1 is 1.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //Used for whenresolution of image is too small for pixle vount on screen
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR / GL_NEAREST. LINEAR more appealing but more expensive.

		//texture equivalent as glbufferdata
		//ACTUALLY copies the texture over!
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.getSize().x,im.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.getPixelsPtr());//r g b a r g b a r g b a ...... in array
		*/
	}




};
#endif
