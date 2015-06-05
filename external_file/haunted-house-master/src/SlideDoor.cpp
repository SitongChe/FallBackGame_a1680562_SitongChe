#include <cmath>
#include "SlideDoor.h"
using namespace std;

SlideDoor::SlideDoor(float _x, float _y, float _z, float _width, float _height, BoundingBox * _box, Material * _mat, unsigned int _textureID, float _texWidth, float _texHeight, bool _stayOpened) 
	: Door(_x, _y, _z, _width, _height, _box, _mat, _textureID, _texWidth, _texHeight, _stayOpened), SLIDE_SPEED(0.003), OPEN_PERIOD(400)
{
	openOffset = 0.0;
}

void SlideDoor::update()
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
		openOffset += SLIDE_SPEED;
		if (openOffset >= width)
		{
			openOffset = width;
			state = OPEN;
			timeToClose = OPEN_PERIOD;
		}
	}

	else if (state == CLOSING)
	{
		openOffset -= SLIDE_SPEED;
		if (openOffset <= 0.0)
		{
			openOffset = 0.0;
			state = CLOSED;
		}
	}
}

void SlideDoor::draw()
{
	if (openOffset == width / 2)	// no need to draw the door if it is open
		return;

	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	trans.transform();
	glTranslatef(openOffset/2, 0, 0);

	// so this part of the drawing is basically the same as that of drawing the wall. check that out
	// if you want details.
	// a small change is that we don't draw the part of the wall which has been slided up, so to speak

	mat->setMaterial();

	glNormal3f(0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, textureID);

	draw_surface(width - openOffset, height, texWidth, texHeight);


	// here, we draw the backside of the door. THIS DOES NOT WORK as it is supposed to, we're not able to see the error in this program
	// either way  :P  FIX if needed.
	glRotatef(180, 0, 1, 0);
	
	draw_surface(width - openOffset, height, -texWidth, texHeight);

	glPopMatrix();
}


void SlideDoor::open(Vector3f charPos)
{
	if (state != CLOSED)	// no need to try and open the door if it isn't closed, kinda..
		return;

	// don't open the door in we're not close enough to it, and looking at it
	if (dist(pos, charPos) < 3.0)
		state = OPENING;
}


void SlideDoor::close()
{
	if (state == CLOSED)
		return;

	else
		state = CLOSING;
}

BoundingBox* SlideDoor::getBox() 
{ 
	// unless the door is completely open, prevent passing. very simplified; better solution would be
	// to adjust the position of the bounding box according to the offset of the door.
	if (state != OPEN)
		return box; 
	else 
		return NULL; 
}