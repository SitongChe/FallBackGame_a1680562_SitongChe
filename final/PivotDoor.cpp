#include <cmath>
#include "PivotDoor.h"
using namespace std;

PivotDoor::PivotDoor(float _x, float _y, float _z, float _xPivot, float _yPivot, float _zPivot, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight, bool _stayOpened) 
	: Door(_x, _y, _z, _width, _height, _box, _mat, _textureID, _texWidth, _texHeight, _stayOpened), pivot(_xPivot, _yPivot, _zPivot), MAX_ANGLE(100), ROTATION_SPEED(0.7), OPEN_PERIOD(400)
{
	angle = 0.0;
	doorAngle = atan2( (pivot.getZ() - pos.getZ()), (pivot.getX() - pos.getX()) ) * 180.0 / 3.1416;
}

void PivotDoor::update()
{
	if (state == CLOSED)	// no changes to be done if it's closed/open
		return;

	else if (!stayOpened && state == OPEN)	// if it is open, check to see whether it should close
	{
		timeToClose--;
		if (timeToClose <= 0)
			close();
	}

	else if (state == OPENING)	// if the door is opening...
	{
		angle += ROTATION_SPEED;
		if (angle >= MAX_ANGLE)
		{
			angle = MAX_ANGLE;
			state = OPEN;
			timeToClose = OPEN_PERIOD;
		}
	}

	else if (state == CLOSING)
	{
		angle -= ROTATION_SPEED;
		if (angle <= 0.0)
		{
			angle = 0.0;
			state = CLOSED;
		}
	}
}

void PivotDoor::draw()
{
	glPushMatrix();
	glTranslatef(pivot.getX() - cos((angle - doorAngle) * 3.1416 / 180.0) * width/2.0, pivot.getY(), pivot.getZ() + sin((angle - doorAngle) * 3.1416 / 180.0) * width/2.0);
	glRotatef(angle - doorAngle, 0, 1, 0);
	trans.transform();

	// so this part of the drawing is basically the same as that of drawing the wall. check that out
	// if you want details

	mat->setMaterial();


	glNormal3f(0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, textureID);

	draw_surface(width, height, texWidth, texHeight);

	// here, we draw the backside of the door. 
	glRotatef(180, 0, 1, 0);
	
	draw_surface(width, height, -texWidth, texHeight);

	glPopMatrix();
}


void PivotDoor::open(Vector3f charPos)
{
	if (state != CLOSED)	// no need to try and open the door if it isn't closed, kinda..
		return;

	// don't open the door in we're not close enough to it, and looking at it
	if (dist(pos, charPos) < 3.0)
		state = OPENING;
}


void PivotDoor::close()
{
	if (state == CLOSED)
		return;

	else
		state = CLOSING;
}

BoundingBox* PivotDoor::getBox() 
{ 
	if (state == CLOSED) 
		return box; 
	else 
		return NULL; 
}