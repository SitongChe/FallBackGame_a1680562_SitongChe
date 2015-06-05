/*
/ This is a pretty cool class, representing a surveillance camera and its corresponding monitor.
/ When constructing, the user passes the position of the camera, the direction of the camera, and
/ the position and size of the screen. It must be used in a certain way; instead of rendering the
/ scene once, we render it twice (this must be done "manually", outside the class) - one time
/ after having called prepareCam(), which makes the scene to be drawn from the perspective of the
/ camera. After this, call createTexture() to create a texture of the contents of the back buffer.
/ Then, clear the buffers and draw the scene again, as per usual, along with a call to drawCam()
/ and drawScreen().
/
/ A small difference from how it usually works in the other classes: the position of the monitor
/ specifies the bottom center, not the center of the whole monitor. This is so we can make sure
/ that the monitor is placed upon another object (like a table), no matter what its size is
*/


#ifndef SURVCAMERA_H
#define SURVCAMERA_H

#include "Constants.h"
#include "Vector3f.h"
#include "Material.h"

class SurvCamera
{
private:
	Vector3f camPos;	// the position of the camera
	Vector3f camDir;	// the direction of the camera lens
	Vector3f screenPos;	// the position of the screen connected to the camera

	// screenWidth is provided by the user; screenHeight is calculated using ASPECT_RATIO in Constants.h. screenDepth is set appropriatly
	float screenWidth, screenHeight, screenDepth;

	float screenYRot;	// how much to rotate the screen about the y-axis

	// this stores the dimension used for the texture. It depends on the properties of the screen;
	// if the window is 640 x 480, then the closest power-of-2-square is 1024 x 1024, and so on
	int texDimension;

	unsigned int textureID;	// texture for the screen
	unsigned int cameraTex;	// texture for the camera

	// materials for the monitor and the camera
	Material * monitorMat;
	
	// material for the screen itself
	Material * screenMat;

	float theta, phi;	// angles used to rotate the camera
public:
	SurvCamera(float _camX, float _camY, float _camZ, float _dirX, float _dirY, float _dirZ, float _screenX, float _screenY, float _screenZ, float _screenWidth, float _screenYRot = 0);
	~SurvCamera();

	// positions and directs the ModelView-matrix according to camPos and camDir
	void prepareCam();

	// creates a texture from the contents of the back color buffer
	void createTexture();

	// draws the camera
	void drawCam();

	// draws the monitor
	void drawScreen();
};

#endif