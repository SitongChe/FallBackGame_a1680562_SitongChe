/*
/
/ So this is the camera-class. It's not very efficient - above all, some trigonometric 
/ calculations will be done in each frame. When trying to improve performance, look at this
/ first of all.
/
/ It's supposed to provide the type of controls you have in your everyday-FPS. You can move
/ forward and back, strafe left and right, and look in different directions.
/
/ It uses two angles (theta and phi) to keep track of the direction you're looking. The camera
/ has a position, and the position is updated depending on the buttons you're pressing, and
/ the direction you're looking.
/
/ The setCamera() method is supposed to be called each time you render the scene, and before
/ any other transformations are done. It uses glTranslatef and glRotatef to modify the
/ GL_MODELVIEW matrix, in effect turning and translating the whole scene.
/
/ So alright, there's a small confusing thing which will need some clarification. The pos-vector member
/ of this class stores the position of the character, where the y-component is the bottom position,
/ basically. However, the BoundingBox is measured from the middle of the camera. Also, we don't actually
/ see the scene from the pos-vector position, but rather from a position measured charHeight from this
/ position (about 1.8 metres)
/
*/ 

#ifndef CAMERA_H
#define CAMERA_H

#include "BoundingBox.h"
#include "Vector3f.h"

class Camera
{
private:
	Vector3f pos;	// position
	Vector3f vel;	// velocity vector
	float theta, phi;			// the direction we're looking; theta = around y-axis, phi = up/down

	const float CHAR_HEIGHT, CHAR_WIDTH, CHAR_DEPTH;

	const float SPEED;

	BoundingBox * box;

	// keeps track of the room the user is in. See BoundingBox.h and AppManager.h for more info
	int roomNo;

	enum { FWD, BACK, LEFT, RIGHT };	// ENUM HACK!
	bool buttonPressed[4];	// used to keep track of the buttons (UP, DOWN, and so on) that are pressed
public:
	Camera(float x, float y, float z);
	~Camera();

	// this is where we update the position of the camera
	void update();

	// updates the MODELVIEW matrix according to the camera's attributes. Should be called before
	// any other transformations of objects
	void setCamera();

	void drawCharacter();

	void moveForward(bool mov);
	void moveBack(bool mov);
	void moveLeft(bool mov);
	void moveRight(bool mov);
	void rotLeftRight(float rot);
	void rotUpDown(float rot);

	float getTheta() { return theta; }
	float getPhi() { return phi; }
	float getCharHeight() { return CHAR_HEIGHT; }

	Vector3f& getPos() { return pos; }

	int roomNumber() { return roomNo; }
};


#endif