#include "Flash.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Flash::Flash(unsigned int _texID, bool _lightning, float _decreaseRate, float _initAlpha)
{
	alpha = _initAlpha;
	decreaseRate = _decreaseRate;
	done = false;
	textureID = _texID;

	mat = new Material(GL_FRONT);
	mat->setSpecular(1, 1, 1, alpha);
	mat->setDiffuse(1, 1, 1, alpha);
	mat->setAmbient(1, 1, 1, alpha);
	mat->setEmission(1, 1, 1, alpha);

	emission = new float[4];
	emission[0] = 1.0;
	emission[1] = 1.0;
	emission[2] = 1.0;
	emission[3] = alpha;

	if (_lightning)
	{
		// create a lightning bolt
	}
}

Flash::~Flash()
{
	delete mat;
	delete [] emission;
}

void Flash::update()
{
	alpha -= decreaseRate;
	if (alpha <= 0.0)
		done = true;
}

void Flash::drawFlash()
{
	if (done)	// if the flash is over, no need to draw anything
		return;

	glPushMatrix();
	glLoadIdentity();

	mat->setAlpha(alpha);
	mat->setMaterial();

	glBindTexture(GL_TEXTURE_2D, textureID);

	// this works well as long as FIELD_OF_VIEW is 90 or below
	const float MULTX = ASPECT_RATIO * FIELD_OF_VIEW/45.0;
	const float MULTY = FIELD_OF_VIEW/45.0;

	glBegin(GL_QUADS);	// draw the quad just in front of the viewer
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-0.1 * MULTX, -0.1 * MULTY, -0.2);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(0.1 * MULTX, -0.1 * MULTY, -0.2);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(0.1 * MULTX, 0.1 * MULTY, -0.2);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-0.1 * MULTX, 0.1 * MULTY, -0.2);
	glEnd();

	glPopMatrix();
}

void Flash::drawLightning()
{
	if (done)	// if the flash is over, no need to draw anything
		return;
}

bool Flash::isDone()
{
	return done;
}