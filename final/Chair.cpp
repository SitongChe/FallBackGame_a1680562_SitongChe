#include "Chair.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Chair::Chair(float _x, float _y, float _z, float _width, float _height, float _depth, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight) : Object(_x, _y, _z, _box, _mat)
{
	width = _width;
	height = _height;
	depth = _depth;

	textureID = _textureID;
	texWidth = _texWidth;
	texHeight = _texHeight;

	metalMat = new Material(GL_FRONT);
	metalMat->setSpecular(1.0, 1.0, 1.0, 1.0);
	metalMat->setDiffuse(0.6, 0.6, 0.6, 1.0);
	metalMat->setAmbient(0.6, 0.6, 0.6, 1.0);
	metalMat->setShininess(110);

	quadric = gluNewQuadric();
}

Chair::~Chair()
{
	delete metalMat;
	gluDeleteQuadric(quadric);
}

void Chair::update()
{
}

void Chair::draw()
{
	glPushMatrix();
		glTranslatef(pos.getX(), pos.getY() + height, pos.getZ());
		glRotatef(-90, 1, 0, 0);
		trans.transform();

		// first, draw the top side of the table. We do it pretty much the same way as we draw the walls

		glNormal3f(0, 0, 1);

		mat->setMaterial();
		glBindTexture(GL_TEXTURE_2D, textureID);

		draw_surface(width, depth, texWidth, texHeight);
	glPopMatrix();
	// ...done drawing the top of the table...
	

	// ...next, we draw the legs of the table!
	metalMat->setMaterial();			// metal-ish material
	glBindTexture(GL_TEXTURE_2D, 0);	// no texture

	glPushMatrix();
		glTranslatef(pos.getX(), pos.getY(), pos.getZ());
		glTranslatef(-width/2 + width/10, 0, depth/2 - depth/10);
		glRotatef(-90, 1, 0, 0);
		gluCylinder(quadric, 0.05, 0.05, height, 20, 20);

		glRotatef(90, 1, 0, 0);
		glTranslatef(width - width/5, 0, 0);
		glRotatef(-90, 1, 0, 0);
		gluCylinder(quadric, 0.05, 0.05, height, 20, 20);

		glRotatef(90, 1, 0, 0);
		glTranslatef(0, 0, -depth + depth/5);
		glRotatef(-90, 1, 0, 0);
		gluCylinder(quadric, 0.05, 0.05, height, 20, 20);
    
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, -depth + depth/5);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quadric, 0.5, 0.5, height, 20, 20);

		glRotatef(90, 1, 0, 0);
		glTranslatef(-width + width/5, 0, 0);
		glRotatef(-90, 1, 0, 0);
		gluCylinder(quadric, 0.05, 0.05, height, 20, 20);
	glPopMatrix();
}