/*
/ This is the core class of the application. It takes care of everything: setting up openGL (including
/ lights and textures), builds and positions the world and objects in the world, handles user input,
/ and manages the application events. Pretty huge. The biggest part of AppManager.cpp is initWorld,
/ which creates all the objects (walls and things like that) of the application.
/
/ The application consists of a number of rooms - three, to be exact, plus two corridors
/ connecting the rooms. These have different ID-numbers to identify them; this is needed
/ for some of the cool scripting stuff, and for some other things. The ID is contained within
/ the bounding box of the floor in each room; the player will "collide" with the floor in each
/ frame, and can thus always keep track of where he is by asking the bounding box for its
/ ID.
/ Please note that this only works IF GRAVITATION IS ACTIVATED
/
/ The ID's are as follows:
/	ROOM 1 (east room)		ID #1
/	ROOM 2 (north room)		ID #2
/	ROOM 3 (west room)		ID #3
/	EAST CORRIDOR			ID #4
/	WEST CORRIDOR			ID #5
*/

#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <vector>
#include "Constants.h"
#include "Camera.h"
#include "Object.h"
#include "Door.h"
#include "SlideDoor.h"
#include "SurvCamera.h"
#include "Flash.h"
#include "Ghost.h"
#include "Window.h"
#include "Material.h"
#include "Mirror.h"
#include "Snowman.h"
#include "Key.h"
#include "Wall.h"
#include "SkyBox.h"
#include "Carousel.h"
#include "GrandfatherClock.h"
#include "Cube.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class AppManager
{
private:
	Camera * cam;	// this is the camera which the player controlls.
    Camera * cam1;	// this is the camera which the player controlls.
	bool flashlightOn;	// is the flashlight on or off?

	std::vector<Object*> objects;	// a vector containing pointers to all Objects of the application
	std::vector<Window*> windows;	// we want to keep the windows separated from the other objects, since we want to make sure they are drawn last
    std::vector<Mirror*> mirrors;
	std::vector<Door*> doors;	// a vector of all doors
	SlideDoor * lockedDoor;		// a pointer to the locked door
	Door * westWindowDoor;		// pointer to the door of the western room window. Needed for modifying the state.

	SurvCamera * survCam;	// a surveillance camera!
	Flash * flash;			// a flash/lightning. Generated at random times. NOT IMPLEMENTED YET
	Ghost * ghost;		// A GHOST!!
	Snowman * snowman;	// an even more evil ghost, in the shape of a creepy snowman
	SkyBox * skybox;	// the skybox
	Carousel * carousel;// the carousel in the middle
	GrandfatherClock * clock;	// a grandfather clock for room 3

	Key * key;		// pointer to the key which is placed in the room in one of the states
    //Cube * cube;
	bool hasKey;	// does the player have the key to the sliding door?
	bool fillPolygons;	// used to switch between GL_LINE and GL_FILL in glPolygonMode. Mostly for fun

	GLuint * textures;					// vector of texture handles
	std::vector<Material*> materials;	// vector of materials used for different stuff
	enum {	WALL, OUTER_WALL, FLOOR, WOOD_DOOR, METAL_DOOR, WINDOW, FINAL_CORRIDOR, CAKE, DEMON, BLACK, 
			SKY_UP, SKY_DOWN, SKY_NORTH, SKY_SOUTH, SKY_EAST, SKY_WEST };	// this enumeration is used to index into the textures-array and materials-vector

	// instead of using boolean flags, we have a simple int, where a value of
	// 1 means a certain state, a value of 2 means another, and so on. Very
	// clumsy system, but for such a small project, it'll do. This variable
	// matters in the update()-method. A list of what the values mean is given
	// at the bottom of this file.
	int state;

	// keeps track of the current room number. Of course, this can be read from the Camera
	// object, but we need this variable to keep track of when the player moves from one
	// room to another.
	int roomNo;

	// these are used when freaking out the perspective angle and aspect ratio. Used with freakOutPersp()
	float freakoutAspectAngle;
	float freakoutAspectAmplitude;
	float freakoutAspectPeriod;
	float freakoutPerspAngle;
	float freakoutPerspAmplitude;
	float freakoutPerspPeriod;

	void initGL();		// initiate openGL
	void initLights();	// initiate the lights
	void initMaterials();	// initiates the materials
	void initTextures();	// initiates textures
	void initWorld();		// initiates the world (HUGE! Read at own risk!!)

	void setGlobalFlashlight();	// sets the flashlight position and direction in global coordinates
	void setLocalFlashlight();	// sets the flashlight position and direction for first person-use

	void setLights();	// positions all stationary lights after setting the modelview matrix

	void interact();	// is called when an "interact"-key has been pressed

	void freakOutPersp();		// called in update() when state is set to PROJECTION_FREAKOUT
	void resetPersp();			// resets the projection matrix
public:
	AppManager();
	~AppManager();

	void init();	// initiates the application

	void render();	// called by render() in main.cpp

	void update();	// called by update() in main.cpp

	// these functions handles input
	void keybDown(unsigned char key, int x, int y);
	void keybUp(unsigned char key, int x, int y);
	void mouseFunc(int button, int state, int x, int y);

	// returns the vector of Objects in the application. Used in camera for collision detection
	std::vector<Object*>& getObjects() { return objects; } 
};


/*
/	LIST OF STATE VALUES
/	
/	1	Startup state
/	2	Window in room 3 has been opened. Snowman appears in room 1, looking
/		at the player through the window.
/	3	The player has exited room 3 after state 2. Snowman disappears
/	4	The player as moved from room 2 to the eastern corridor. Demon flashes
/		in front of the players eyes
/	5	The player has entered room 1 again. Perspective and aspect ratio freaks
/		out. Key can be seen on the floor where the snowman was.
/	6	The player has picked up the key. [spin-thingy in the middle of the
/		garden freaks out]. Perspective- and aspect ratio freakout intensifies.
/	7	The player has opened the door to the corridor. SNOWMAN CHARGES!!
/	8	The snowman has reached the player. Black flash, takes a while to
/		recover, but afterwards, no problems. No more freakouts. The player
/		can now open the sliding door in room 3. Uwehehe.
*/


#endif