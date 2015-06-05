#include <cmath>
#include "SurvCamera.h"
#include "Constants.h"
#include "Vector3f.h"
#include "BMPLoader.h"
#include "Material.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

SurvCamera::SurvCamera(float _camX, float _camY, float _camZ, float _dirX, float _dirY, float _dirZ, float _screenX, float _screenY, float _screenZ, float _screenWidth, float _screenYRot) : camPos(_camX, _camY, _camZ), camDir(_dirX, _dirY, _dirZ), screenPos(_screenX, _screenY, _screenZ)
{
	screenWidth = _screenWidth;
	screenHeight = screenWidth / ASPECT_RATIO;
	screenDepth = 0.5 * screenWidth;	// the depth of the monitor is half of the width
	screenYRot = _screenYRot;
	
	glGenTextures(1, &textureID);

	monitorMat = new Material(GL_FRONT);
	monitorMat->setSpecular(0.1, 0.1, 0.1, 1.0);
	monitorMat->setDiffuse(0.9, 0.0, 0.0, 1.0);
	monitorMat->setAmbient(0.9, 0.0, 0.0, 1.0);
	monitorMat->setShininess(5.0);

	screenMat = new Material(GL_FRONT);
	screenMat->setSpecular(0.8, 0.8, 0.8, 1.0);
	screenMat->setDiffuse(0.5, 0.5, 0.5, 1.0);
	screenMat->setAmbient(0.5, 0.5, 0.5, 1.0);
	screenMat->setShininess(50);
	screenMat->setEmission(1, 1, 1, 1);

	Texture tmpTexture;
	load_bmp("resources/eye.bmp", 24, tmpTexture);
	glGenTextures(1, &cameraTex);
	glBindTexture(GL_TEXTURE_2D, cameraTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	// this is the angle between the yz-plane and camDir, basically
	theta = atan2( camDir.getX(), camDir.getZ() ) * 180.0 / 3.1416;
	
	// this vector is the same vector as camDir, projected onto the xz-plane
	Vector3f tmpVector(camDir.getX(), 0.0, camDir.getZ());

	phi = acos( camDir.dotProduct(tmpVector) / ( camDir.length() * tmpVector.length() ) ) * 180.0 / 3.1416;


	// calculate the texDimension
	texDimension = 1;
	while ( (texDimension <= WINDOW_WIDTH) || (texDimension <= WINDOW_HEIGHT) )
		texDimension *= 2;
}

SurvCamera::~SurvCamera()
{
	delete monitorMat;
	delete screenMat;
	glDeleteTextures(1, &textureID);
}

void SurvCamera::prepareCam()
{
	// basically, transform the scene so that the origin is at the camera lens, pointing in the direction of this camera
	// (hey, there's no shame using gluLookAt when it makes sense!!)
	gluLookAt(camPos.getX(), camPos.getY(), camPos.getZ(), camPos.getX() + camDir.getX(), camPos.getY() + camDir.getY(), camPos.getZ() + camDir.getZ(), 0, 1, 0);
}

void SurvCamera::createTexture()
{
	// delete the old texture

	glDeleteTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// create a texture from the contents of the back buffer
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, texDimension, texDimension, 0);
}

void SurvCamera::drawCam()
{
	glPushMatrix();
	glTranslatef(camPos.getX(), camPos.getY(), camPos.getZ());
	glRotatef(theta, 0, 1, 0);
	glRotatef(phi, 1, 0, 0);
	static int i = 0;
	i++;
	glRotatef(i, 0, 0, 1);	// make the camera rotate creepishly...

	Material::setWhiteMaterial();
	glBindTexture(GL_TEXTURE_2D, cameraTex);

	glBegin(GL_POLYGON);
		for (float i = 0; i <= 3.1416 * 2; i += 0.3)	// draw the camera as a circle, applying the texture accordingly
		{
			glTexCoord2f(0.5 + cos(i)/2, 0.5 + sin(i)/2);
			glVertex3f(cos(i) * 0.1, sin(i) * 0.1, 0.0);
		}
	glEnd();

	glPopMatrix();

	glColor3f(1, 1, 1);
}

void SurvCamera::drawScreen()
{
	glPushMatrix();
	glTranslatef(screenPos.getX(), screenPos.getY() + screenHeight/2, screenPos.getZ());
	glRotatef(screenYRot, 0, 1, 0);

	monitorMat->setMaterial();

	// draw the sides, top, bottom and back of the monitor. We do some magic to create some nice geometry
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
		// left side
		glColor3f(0.7, 0.7, 0.7);
		glNormal3f(-1, 0, 0);
		glVertex3f(-screenWidth/2, -screenHeight/2, screenDepth/2);
		glVertex3f(-screenWidth/2, screenHeight/2, screenDepth/2);
		glVertex3f(-screenWidth/2 + screenWidth/5, screenHeight/2 - screenHeight/5, -screenDepth/2);
		glVertex3f(-screenWidth/2 + screenWidth/5, -screenHeight/2, -screenDepth/2);

		// right side
		glNormal3f(1, 0, 0);
		glVertex3f(screenWidth/2, -screenHeight/2, screenDepth/2);
		glVertex3f(screenWidth/2 - screenWidth/5, -screenHeight/2, -screenDepth/2);
		glVertex3f(screenWidth/2 - screenWidth/5, screenHeight/2 - screenHeight/5, -screenDepth/2);
		glVertex3f(screenWidth/2, screenHeight/2, screenDepth/2);
		
		// back side
		glNormal3f(0, 0, -1);
		glVertex3f(-screenWidth/2 + screenWidth/5, -screenHeight/2, -screenDepth/2);
		glVertex3f(-screenWidth/2 + screenWidth/5, screenHeight/2 - screenHeight/5, -screenDepth/2);
		glVertex3f(screenWidth/2 - screenWidth/5, screenHeight/2 - screenHeight/5, -screenDepth/2);
		glVertex3f(screenWidth/2 - screenWidth/5, -screenHeight/2, -screenDepth/2);

		// top side
		glNormal3f(0, 1, 0);
		glVertex3f(-screenWidth/2, screenHeight/2, screenDepth/2);
		glVertex3f(screenWidth/2, screenHeight/2, screenDepth/2);
		glVertex3f(screenWidth/2 - screenWidth/5, screenHeight/2 - screenHeight/5, -screenDepth/2);
		glVertex3f(-screenWidth/2 + screenWidth/5, screenHeight/2 - screenHeight/5, -screenDepth/2);

		// bottom side
		glNormal3f(0, -1, 0);
		glVertex3f(-screenWidth/2, -screenHeight/2, screenDepth/2);
		glVertex3f(-screenWidth/2 + screenWidth/5, -screenHeight/2, -screenDepth/2);
		glVertex3f(screenWidth/2 - screenWidth/5, -screenHeight/2, -screenDepth/2);
		glVertex3f(screenWidth/2, -screenHeight/2, screenDepth/2);
	glEnd();
	glColor3f(1, 1, 1);

	screenMat->setMaterial();
	glBindTexture(GL_TEXTURE_2D, textureID);

	float xMax = (float)WINDOW_WIDTH / (float)texDimension;
	float yMax = (float)WINDOW_HEIGHT / (float)texDimension;

	// draw the screen of the monitor
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0.0);
		glVertex3f(-screenWidth/2, -screenHeight/2, screenDepth/2);

		glTexCoord2f(xMax, 0.0);
		glVertex3f(screenWidth/2, -screenHeight/2, screenDepth/2);

		glTexCoord2f(xMax, yMax);
		glVertex3f(screenWidth/2, screenHeight/2, screenDepth/2);

		glTexCoord2f(0.0, yMax);
		glVertex3f(-screenWidth/2, screenHeight/2, screenDepth/2);
	glEnd();

	glPopMatrix();
}