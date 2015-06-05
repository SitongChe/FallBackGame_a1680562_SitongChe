#include "Wall.h"
#include "Constants.h"
#include <GLUT.h>

Wall::Wall(float _x, float _y, float _z, BoundingBox * _box, Material * _mat, float _width, float _height, unsigned int _textureID, float _texWidth, float _texHeight) : Object(_x, _y, _z, _box, _mat)
{
	width = _width;
	height = _height;
	textureID = _textureID;
	texWidth = _texWidth;
	texHeight = _texHeight;

	isTwosided = false;
	backTexID = 0;
	backTexWidth = backTexHeight = 0.0;
}

Wall::~Wall()
{}

void Wall::setBackside(Material * _backMat, unsigned int _textureID, float _texWidth, float _texHeight)
{
	isTwosided = true;
	backMat = _backMat;
	backTexID = _textureID;
	backTexWidth = _texWidth;
	backTexHeight = _texHeight;
}

void Wall::update()
{
}

void Wall::draw()
{
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	trans.transform();

	// the normal is pointing towards the viewer, in object coordinates
	glNormal3f(0, 0, 1);

	// apply texture and material
	mat->setMaterial();
	glBindTexture(GL_TEXTURE_2D, textureID);

	draw_surface(width, height, texWidth, texHeight);

	if (isTwosided)	// draw the backside
	{
		glRotatef(180, 0, 1, 0);

		glNormal3f(0, 0, 1);

		backMat->setMaterial();
		glBindTexture(GL_TEXTURE_2D, backTexID);

		draw_surface(width, height, texWidth, texHeight);
	}

	glPopMatrix();
}