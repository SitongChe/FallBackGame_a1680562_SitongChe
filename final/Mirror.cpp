#include <cmath>
#include <iostream>
#include "Mirror.h"
#include "Constants.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
using namespace std;
Mirror::Mirror(float _x, float _y, float _z, float _width, float _height, float _yRot, BoundingBox * _box) : Object(_x, _y, _z, _box, NULL)
{
	width = _width;
	height = _height;
	yRot = _yRot;

	leftXOffset = 0.0;
	rightXOffset = 0.0;
	leftYOffset = 0.0;
	rightYOffset = 0.0;

	glGenTextures(1, &textureID);

	texDimension = 1;
	while ( (texDimension <= WINDOW_WIDTH) || (texDimension <= WINDOW_HEIGHT) )
		texDimension *= 2;
}

Mirror::~Mirror()
{
	glDeleteTextures(1, &textureID);
}

void Mirror::prepareMirror(Vector3f &charPos)
{
	float angle = atan2( (8.75 - charPos.getX()), (3.5 - charPos.getZ()) );
	//cout << angle * 180.0 / 3.1416 << endl;

	// then, place the "mirror-viewer" accordingly
	float x = charPos.getX();
	float y = 1.35;
	float z = charPos.getZ();
	Vector3f tmpVec;	// vector pointing from character position to mirror position

	tmpVec.setX(8.75 - charPos.getX());
	tmpVec.setY(0.0);
	tmpVec.setZ(3.5 - charPos.getZ());

	x += 2 * tmpVec.getX();
	x += 2 * (x - 8.75);
	x = charPos.getX();
	z += 2 * tmpVec.getZ();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, width/height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);

	gluLookAt(x, y, z, 8.75, 1.35, 3.5, 0, 1, 0);

	// Here we need both the distance and the angle
	leftXOffset = 0.0;
	rightXOffset = 0.0;
	leftYOffset = 0.0;
	rightYOffset = 0.0;
}

void Mirror::createTexture()
{
	glBindTexture(GL_TEXTURE_2D, textureID);

	// create a texture from the contents of the back buffer
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, texDimension, texDimension, 0);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, ASPECT_RATIO, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}
void Mirror::update()
{
}
void Mirror::draw()
{
	glPushMatrix();
	
	glTranslatef(8.75, 1.35, 3.5);
	glRotatef(yRot, 0, 1, 0);

	float xMax = (float)WINDOW_WIDTH / texDimension;
	float yMax = (float)WINDOW_HEIGHT / texDimension;

	//cout << xMax << endl << yMax << endl << endl;

	glBindTexture(GL_TEXTURE_2D, textureID);

	glBegin(GL_QUADS);
		glTexCoord2f(xMax - leftXOffset, 0.0 + leftYOffset);
		glVertex3f(-width/2, -height/2, 0.0);

		glTexCoord2f(0.0 + rightXOffset, 0.0 + rightYOffset);
		glVertex3f(width/2, -height/2, 0.0);

		glTexCoord2f(0.0 + rightXOffset, yMax - rightYOffset);
		glVertex3f(width/2, height/2, 0.0);

		glTexCoord2f(xMax - leftXOffset, yMax - leftYOffset);
		glVertex3f(-width/2, height/2, 0.0);
	glEnd();


	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(-(width/2 + 0.01), -(height/2 + 0.01), 0.0);
		glVertex3f((width/2 + 0.01), -(height/2 + 0.01), 0.0);
		glVertex3f((width/2 + 0.01), (height/2 + 0.01), 0.0);
		glVertex3f(-(width/2 + 0.01), (height/2 + 0.01), 0.0);
	glEnd();

	glPopMatrix();

	glDeleteTextures(1, &textureID);	// the last thing we do is to remove the texture, since we won't be showing it again
}