#include <cmath>
#include "GrandfatherClock.h"
#include "Constants.h"
#include "BMPLoader.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
using namespace std;

GrandfatherClock::GrandfatherClock(float _x, float _y, float _z, BoundingBox * _box, Material * _mat) : Object(_x, _y, _z, _box, _mat), PENDULUM_MAX_ANGLE(10.0), HEIGHT(2.0), DEPTH(0.3), WIDTH(0.5)
{
	milliseconds = 0;
	seconds = 0;	
	minutes = 0;
	hours = 0;
	coeff = 1;
	secondAngle = 0.0;
	minuteAngle = 0.0;
	hourAngle = 0.0;
	
	calcAngles();

	Texture tmpTexture;
	load_bmp("resources/clock_face.bmp", 24, tmpTexture);
	glGenTextures(1, &faceTexID);
	glBindTexture(GL_TEXTURE_2D, faceTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	faceMat = new Material(GL_FRONT);
	faceMat->setAmbient(0.6, 0.6, 0.6, 1.0);
	faceMat->setDiffuse(0.6, 0.6, 0.6, 1.0);
	faceMat->setSpecular(0.3, 0.3, 0.3, 1.0);
	faceMat->setShininess(3.0);



	load_bmp("resources/clock_wood.bmp", 24, tmpTexture);
	glGenTextures(1, &baseTexID);
	glBindTexture(GL_TEXTURE_2D, baseTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	baseMat = new Material(GL_FRONT);
	baseMat->setAmbient(0.6, 0.5, 0.4, 1.0);
	baseMat->setDiffuse(0.6, 0.5, 0.4, 1.0);
	baseMat->setSpecular(0.4, 0.4, 0.4, 1.0);
	baseMat->setShininess(6.0);

	pendMat = new Material(GL_FRONT);
	pendMat->setAmbient(1.0, 1.0, 0.0, 1.0);
	pendMat->setDiffuse(1.0, 1.0, 0.0, 1.0);
	pendMat->setSpecular(1.0, 1.0, 0.0, 1.0);
	pendMat->setShininess(120.0);

	// PLEASE NOTE!! These bounding box-settings ONLY works if  the clock is rotated 180 degrees. Running out of time,
	// no time to make general stuff!
	box = new BoundingBox(pos.getX(), pos.getY(), pos.getZ() - DEPTH/2.0, WIDTH, HEIGHT, DEPTH);
	quadric = gluNewQuadric();
}

GrandfatherClock::~GrandfatherClock()
{
	glDeleteTextures(1, &faceTexID);
	delete faceMat;
	glDeleteTextures(1, &baseTexID);
	delete baseMat;
	delete pendMat;
	delete [] pendulumAngles;
	gluDeleteQuadric(quadric);
}

void GrandfatherClock::calcAngles()
{
	pendulumAngles = new float[FRAMES_PER_SECOND];
	
	for (int i = 0; i < FRAMES_PER_SECOND; i++)
		pendulumAngles[i] = sin(3.1416/2 + 3.1416 * ((float)i / FRAMES_PER_SECOND)) * PENDULUM_MAX_ANGLE;
}

void GrandfatherClock::update()
{
	++milliseconds;

	if (milliseconds == FRAMES_PER_SECOND)
	{
		milliseconds = 0;
		++seconds;
		secondAngle -= 360.0 / 60.0;	// change the angle of the second hand accordingly
		coeff *= -1;

		if (seconds >= 60)
		{
			seconds -= 60;
			secondAngle += 360.0;	

			++minutes;
			minuteAngle -= 360.0 / 60.0;	// change the angle of the minute handle
			hourAngle -= (360.0 / 12.0) / 60.0;	// to avoid big jumps of the hour handle each hour, we change the angle a little each minute, like clock's usually do

			if (minutes >= 60)
			{
				minutes -= 60;
				++hours;	// no need to change the angle of the hour handle - this is done each minute. ..making this field a little useless. But it might come in handy. Whatever!

				if (hours >= 12)
				{
					hours -= 12;
					hourAngle += 360.0;
				}
			}
		}
	}
}

void GrandfatherClock::draw()
{
	glPushMatrix();
		glTranslatef(pos.getX(), pos.getY(), pos.getZ());
		trans.transform();

		// Draw the base stuff of the clock (the sides and so on) We're cheating a little and don't draw the top or bottom, nyaah
		baseMat->setMaterial();
		glBindTexture(GL_TEXTURE_2D, baseTexID);
			
		glPushMatrix();
			glRotatef(180, 0, 1, 0);
			glNormal3f(0, 0, -1);
			draw_surface(WIDTH, HEIGHT, 0.5, 0.5);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 0, DEPTH-0.001);
			glNormal3f(0, 0, 1);
			draw_surface(WIDTH, HEIGHT, 0.5, 0.5);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-WIDTH/2, 0, DEPTH/2);
			glRotatef(-90, 0, 1, 0);
			glNormal3f(1, 0, 0);
			draw_surface(DEPTH, HEIGHT, 0.5, 0.5);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(WIDTH/2, 0, DEPTH/2);
			glRotatef(90, 0, 1, 0);
			glNormal3f(-1, 0, 0);
			draw_surface(DEPTH, HEIGHT, 0.5, 0.5);
		glPopMatrix();


		// Draw the face of the clock
		glPushMatrix();
			glTranslatef(0, HEIGHT/3, DEPTH);
			faceMat->setMaterial();
			glBindTexture(GL_TEXTURE_2D, faceTexID);
			glNormal3f(0, 0, 1);
			glBegin(GL_POLYGON);
				for (float i = 0.0; i < 2 * 3.1416; i += 0.2)
				{
					glTexCoord2f( 0.5 + cos(i)/2.0, 0.5 + sin(i)/2.0 );
					glVertex3f( cos(i) * WIDTH / 3.0, sin(i) * WIDTH / 3.0, 0.0);
				}
			glEnd();
		glPopMatrix();


		// Draw the hands of the clock
		glPushMatrix();
			glTranslatef(0, HEIGHT/3, DEPTH+0.001);
			Material::setWhiteMaterial();
			glBindTexture(GL_TEXTURE_2D, 0);
			
			glPushMatrix();
				glTranslatef(0, -WIDTH/9.5, 0);		// position the second hand inside the smaller "clock face" in the texture
				glRotatef(secondAngle, 0, 0, 1);
				glBegin(GL_LINES);
					glVertex3f(0.0, 0.0, 0.0);
					glVertex3f(0.0, WIDTH/18.0, 0.0);
				glEnd();
			glPopMatrix();

			glPushMatrix();
				glRotatef(minuteAngle, 0, 0, 1);
				glBegin(GL_LINES);
					glVertex3f(0.0, 0.0, 0.0);
					glVertex3f(0.0, WIDTH/3, 0.0);
				glEnd();
			glPopMatrix();

			glRotatef(hourAngle, 0, 0, 1);
			glBegin(GL_LINES);
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(0.0, WIDTH/5, 0.0);
			glEnd();
		glPopMatrix();


		// Draw the pendulum
		glPushMatrix();
			glTranslatef(0, HEIGHT/5, DEPTH);
			glRotatef(coeff * pendulumAngles[milliseconds], 0, 0, 1);
			pendMat->setMaterial();
			glBindTexture(GL_TEXTURE_2D, 0);
			glPushMatrix();
				glRotatef(90, 1, 0, 0);
				gluCylinder(quadric, 0.01, 0.01, HEIGHT/2, 10, 10);	// draw a cylinder as the rod
			glPopMatrix();
			glTranslatef(0, -HEIGHT/2, 0);
			glBegin(GL_POLYGON);
				for (float i = 0.0; i < 2 * 3.1416; i += 0.2)	// here, we are lazy and just draw a circle at the bottom. Looks pretty cool either way!
					glVertex3f( cos(i) * WIDTH / 5.0, sin(i) * WIDTH / 5.0, 0.0);
			glEnd();
		glPopMatrix();

	glPopMatrix();
}