#include "TexturedSurface.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

TexturedSurface::TexturedSurface() : Object(0, 0, 0, NULL, NULL)
{
	width = 0;
	height = 0;
	textureID = 0;
}

TexturedSurface::TexturedSurface(float _x, float _y, float _z, float _width, float _height, Material * _mat, unsigned int _texID) : Object(_x, _y, _z, NULL, _mat)
{
	width = _width;
	height = _height;
	textureID = _texID;
}

void TexturedSurface::draw()
{
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	trans.transform();

	if (mat != NULL)
		mat->setMaterial();
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-width/2, -height/2, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(width/2, -height/2, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(width/2, height/2, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-width/2, height/2, 0.0);
	glEnd();

	glPopMatrix();
}

void TexturedSurface::update()
{
}