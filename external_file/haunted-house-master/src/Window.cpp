#include "Window.h"
#include <GLUT.h>

Window::Window(float _x, float _y, float _z, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID) : Object(_x, _y, _z, _box, _mat)
{
	width = _width;
	height = _height;
	textureID = _textureID;
}

void Window::update()
{
}

void Window::draw()
{
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	trans.transform();

	// in drawing the window, we make sure that backface culling is disabled, since it will be
	// using alpha blending stuff
	glDisable(GL_CULL_FACE);

	mat->setMaterial();
	glBindTexture(GL_TEXTURE_2D, textureID);
	glNormal3f(0, 0, 1);

	draw_surface(width, height, width, height);

	/*glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-width/2, -height/2, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(width/2, -height/2, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(width/2, height/2, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-width/2, height/2, 0.0);
	glEnd();*/

	glEnable(GL_CULL_FACE);
	glPopMatrix();
}