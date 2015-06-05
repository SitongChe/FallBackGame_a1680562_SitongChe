#include "Constants.h"
#include <GLUT.h>


// these variables are declared here, extern:ed in Constants.h to be made available for all files in the project including that file, and are set in main.cpp by reading a text-file,
// so that we don't have to compile the project each time we change one of these values
int FRAMES_PER_SECOND;
int WINDOW_WIDTH;
int WINDOW_HEIGHT;
float ASPECT_RATIO;
float FIELD_OF_VIEW;

float MOVE_SPEED;	// how fast the user can move. Please observe: if the speed is too high, the collision detection breaks down
float GRAVITATIONAL_CONSTANT;

// how high/wide are the polygons making up the walls and stuff?
float POLYGON_WIDTH;
float POLYGON_HEIGHT;


float dist(Vector3f &a, Vector3f &b)
{
	return ( (a.getX() - b.getX())*(a.getX() - b.getX()) + (a.getY() - b.getY())*(a.getY() - b.getY()) + (a.getZ() - b.getZ())*(a.getZ() - b.getZ()) );
}

void draw_surface(float width, float height, float texWidth, float texHeight)
{
	float y = height/2;
	const float yMax = height/2;

	// this part is quite complicated. Given the width and height of the wall, including the width
	// and height of the polygons, we draw the wall in two phases, basically.
	// We use this rather than a function which draws a wall of fixed size which we can scale, 
	// since we want to make sure the normal vector has unit length.
	// texWidth and texHeight specifies the desired width and height of the texture, which will be
	// repeated if it is smaller than the size of the wall.


	for (y = -height/2; y < height/2 - POLYGON_HEIGHT; y += POLYGON_HEIGHT)
	{
		glBegin(GL_TRIANGLE_STRIP);
			for (float x = -width/2; x <= width/2; x += POLYGON_WIDTH)
			{
				// observe the magic here. the division with texWidth/texHeight, and the proper
				// handling by openGL of texture coordinates above 1.0, makes sure that the texture
				// is mapped as we intend.
				// please note that if we want the bottom left corner to be mapped to the bottom
				// left corner of the wall, then we need to add width/2 to each x-term, and height/2
				// to each y-term, in the glTexCoord-calls. We don't do this here, because it's
				// not necessary for our needs.
				glTexCoord2f((x + width/2) / texWidth, (y + height/2 + POLYGON_HEIGHT) / texHeight);
				glVertex3f(x, y + POLYGON_HEIGHT, 0.0);
				glTexCoord2f((x + width/2) / texWidth, (y + height/2) / texHeight);
				glVertex3f(x, y, 0.0);
			}
			// this makes sure that the last column is drawn
			glTexCoord2f(width / texWidth, (y + height/2 + POLYGON_HEIGHT) / texHeight);
			glVertex3f(width/2, y + POLYGON_HEIGHT, 0.0);
			glTexCoord2f(width / texWidth, (y + height/2) / texHeight);
			glVertex3f(width/2, y, 0.0);
		glEnd();
	}

	// a'right, so the wall is drawn? well, maybe not completely. For the same reason we need
	// to add some extra code for the right column in the above iterations, we might have a
	// row at the top of the wall missing. This is because the y += polHeight might not hit
	// the exact value height/2. This solution is not very pretty, but hey, it works.

	glBegin(GL_TRIANGLE_STRIP);
		for (float x = -width/2; x <= width/2; x += POLYGON_WIDTH)
		{
			glTexCoord2f((x + width/2) / texWidth, height / texHeight);
			glVertex3f(x, yMax, 0.0);
			glTexCoord2f((x + width/2) / texWidth, (y + height/2) / texHeight);
			glVertex3f(x, y, 0.0);
		}
		//...and the final corner yay!
		glTexCoord2f(width / texWidth, height / texHeight);
		glVertex3f(width/2, yMax, 0.0);
		glTexCoord2f(width / texWidth, (y + height/2) / texHeight);
		glVertex3f(width/2, y, 0.0);
	glEnd();
}