#include <cmath>
#include "Mirror.h"
#include "Constants.h"
#include <GLUT.h>

Mirror::Mirror(float _x, float _y, float _z, float _width, float _height, float _yRot) : pos(_x, _y, _z)
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
	float angle = atan2( (pos.getX() - charPos.getX()), (pos.getZ() - charPos.getZ()) );
	//cout << angle * 180.0 / 3.1416 << endl;

	// then, place the "mirror-viewer" accordingly
	float x = charPos.getX();
	float y = pos.getY();
	float z = charPos.getZ();
	Vector3f tmpVec;	// vector pointing from character position to mirror position

	tmpVec.setX(pos.getX() - charPos.getX());
	tmpVec.setY(0.0);
	tmpVec.setZ(pos.getZ() - charPos.getZ());

	x += 2 * tmpVec.getX();
	x += 2 * (x - pos.getX());
	x = charPos.getX();
	z += 2 * tmpVec.getZ();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, width/height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);

	gluLookAt(x, y, z, pos.getX(), pos.getY(), pos.getZ(), 0, 1, 0);

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

void Mirror::draw()
{
	glPushMatrix();
	
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	glRotatef(yRot, 0, 1, 0);

	float xMax = (float)WINDOW_WIDTH / texDimension;
	float yMax = (float)WINDOW_HEIGHT / texDimension;

	cout << xMax << endl << yMax << endl << endl;

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


	/*glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(-(width/2 + 0.01), -(height/2 + 0.01), 0.0);
		glVertex3f((width/2 + 0.01), -(height/2 + 0.01), 0.0);
		glVertex3f((width/2 + 0.01), (height/2 + 0.01), 0.0);
		glVertex3f(-(width/2 + 0.01), (height/2 + 0.01), 0.0);
	glEnd();*/

	glPopMatrix();

	glDeleteTextures(1, &textureID);	// the last thing we do is to remove the texture, since we won't be showing it again
}