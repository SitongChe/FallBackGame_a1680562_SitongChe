#include <cstdlib>
#include "Material.h"
#include <GLUT.h>

BlackAndWhite Material::blackAndWhite;

Material::Material(GLenum _face)
{
	specular = NULL;
	diffuse = NULL;
	ambient = NULL;
	emission = NULL;
	shininess = 0.0;

	face = _face;
}

Material::~Material()
{
	delete [] specular;
	delete [] diffuse;
	delete [] ambient;
	delete [] emission;
}

void Material::setSpecular(float _red, float _green, float _blue, float _alpha)
{
	if (specular == NULL)
		specular = new float[4];

	specular[0] = _red;
	specular[1] = _green;
	specular[2] = _blue;
	specular[3] = _alpha;
}

void Material::setDiffuse(float _red, float _green, float _blue, float _alpha)
{
	if (diffuse == NULL)
		diffuse = new float[4];

	diffuse[0] = _red;
	diffuse[1] = _green;
	diffuse[2] = _blue;
	diffuse[3] = _alpha;
}

void Material::setAmbient(float _red, float _green, float _blue, float _alpha)
{
	if (ambient == NULL)
		ambient = new float[4];

	ambient[0] = _red;
	ambient[1] = _green;
	ambient[2] = _blue;
	ambient[3] = _alpha;
}

void Material::setEmission(float _red, float _green, float _blue, float _alpha)
{
	if (emission == NULL)
		emission = new float[4];

	emission[0] = _red;
	emission[1] = _green;
	emission[2] = _blue;
	emission[3] = _alpha;
}

void Material::setShininess(float _shininess)
{
	shininess = _shininess;
}

void Material::setAlpha(float _alpha)
{
	if (specular != NULL)
		specular[3] = _alpha;
	if (diffuse != NULL)
		diffuse[3] = _alpha;
	if (ambient != NULL)
		ambient[3] = _alpha;
	if (emission != NULL)
		emission[3] = _alpha;	
}

void Material::setAlpha(float _alpha, GLenum _pname)
{
	if (specular != NULL && _pname == GL_SPECULAR)
		specular[3] = _alpha;
	else if (diffuse != NULL && _pname == GL_DIFFUSE)
		diffuse[3] = _alpha;
	else if (ambient != NULL && _pname == GL_AMBIENT)
		ambient[3] = _alpha;
	else if (emission != NULL && _pname == GL_EMISSION)
		emission[3] = _alpha;
}

void Material::setMaterial()
{
	if (specular != NULL)
		glMaterialfv(face, GL_SPECULAR, specular);
	if (diffuse != NULL)
		glMaterialfv(face, GL_DIFFUSE, diffuse);
	if (ambient != NULL)
		glMaterialfv(face, GL_AMBIENT, ambient);
	
	// this created great concern for me. emission is seldom set. should it 
	// be reset manually each time it has been set? that would call for all
	// classes to keep track of whether its material components contains
	// emission stuff, and if it does, reset it afterwards. this solution
	// instead makes sure that if no emission component is set, then emission
	// is simply zero
	if (emission != NULL)
		glMaterialfv(face, GL_EMISSION, emission);

	else
		setBlackMaterial(face, GL_EMISSION);

	glMaterialf(face, GL_SHININESS, shininess);
}

void Material::setWhiteMaterial(GLenum _face)
{
	setWhiteMaterial(_face, GL_SPECULAR);
	setWhiteMaterial(_face, GL_DIFFUSE);
	setWhiteMaterial(_face, GL_AMBIENT);
	glMaterialf(_face, GL_SHININESS, 1.0);
	setBlackMaterial(_face, GL_EMISSION);
}

void Material::setWhiteMaterial(GLenum _face, GLenum _pname)
{
	glMaterialfv(_face, _pname, blackAndWhite.white);
}
	
void Material::setBlackMaterial(GLenum _face)
{
	setBlackMaterial(_face, GL_SPECULAR);
	setBlackMaterial(_face, GL_DIFFUSE);
	setBlackMaterial(_face, GL_AMBIENT);
	glMaterialf(_face, GL_SHININESS, 1.0);
	setBlackMaterial(_face, GL_EMISSION);
}
	
void Material::setBlackMaterial(GLenum _face, GLenum _pname)
{
	glMaterialfv(_face, _pname, blackAndWhite.black);
}

void Material::resetEmission(GLenum _face)
{
	setBlackMaterial(_face, GL_EMISSION);
}