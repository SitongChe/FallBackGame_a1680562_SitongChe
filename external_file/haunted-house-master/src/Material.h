/*
/ This class contains some methods - both instance- and class-methods - for setting material
/ properties. Work could be done in making it more efficient.
*/

#ifndef MATERIAL_H
#define MATERIAL_H

#include <GLUT.h>


// we need this silly structure since C++ doesn't support static constructors. Oh well.
struct BlackAndWhite
{
	float white[4];
	float black[4];

	BlackAndWhite()
	{
		white[0] = 1.0;
		white[1] = 1.0;
		white[2] = 1.0;
		white[3] = 1.0;
		
		black[0] = 0.0;
		black[1] = 0.0;
		black[2] = 0.0;
		black[3] = 1.0;
	}
};

class Material
{
private:
	float * specular;
	float * diffuse;
	float * ambient;
	float * emission;
	float shininess;

	// which face does this material class contain values for?
	GLenum face;

	static BlackAndWhite blackAndWhite;
public:
	Material(GLenum _face = GL_FRONT_AND_BACK);
	~Material();

	void setSpecular(float _red, float _green, float _blue, float _alpha);
	void setDiffuse(float _red, float _green, float _blue, float _alpha);
	void setAmbient(float _red, float _green, float _blue, float _alpha);
	void setEmission(float _red, float _green, float _blue, float _alpha);
	void setShininess(float _shininess);

	// sets the alpha-value of all components of the material
	void setAlpha(float _alpha);
	// sets the alpha-value of the given component of the material
	void setAlpha(float _alpha, GLenum _pname);

	// set the OpenGL state to have the object's material properties
	void setMaterial();

	// the following methods sets the material state to black/white. The methods that does not
	// specify which parameter to set, sets all parameters EXCEPT emission. Shininess is set to 0.0.
	static void setWhiteMaterial(GLenum _face = GL_FRONT_AND_BACK);
	static void setWhiteMaterial(GLenum _face, GLenum _pname);
	
	static void setBlackMaterial(GLenum _face = GL_FRONT_AND_BACK);
	static void setBlackMaterial(GLenum _face, GLenum _pname);

	static void resetEmission(GLenum _face = GL_FRONT_AND_BACK);
};

#endif