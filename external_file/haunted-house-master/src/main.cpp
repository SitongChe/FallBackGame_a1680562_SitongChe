#include <fstream>
#include <ctime>
#include <iostream>
#include "AppManager.h"
#include "Constants.h"
#include <GLUT.h>
using namespace std;

AppManager * app;


void render()
{
	app->render();
}

void keybDown(unsigned char key, int x, int y)
{
	app->keybDown(key, x, y);

	if (key == 'n')
	{
		delete app;
		app = new AppManager();
		app->init();
	}

	/*if (key == 'q')
	{
		delete app;
		exit(0);
	}*/
}

void keybUp(unsigned char key, int x, int y)
{
	app->keybUp(key, x, y);
}

void mouseFunc(int button, int state, int x, int y)
{
	app->mouseFunc(button, state, x, y);
}

void motionFunc(int x, int y)
{
	app->mouseFunc(0, 0, x, y);
}

void update(int a)
{
	// center the mouse pointer, so that it doesn't jump out of the window
	glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	app->update();

	glutPostRedisplay();
	glutTimerFunc(1000.0 / FRAMES_PER_SECOND, update, 0);	// this will limit the program to a FPS of about 60
}

// forward declaration of the function which will load the values for the variables in Constants.h
void loadData();

int main(int argc, char ** argv)
{
	loadData();
	srand((unsigned int)time(NULL));	// for randomizing numbers

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("SUPER PROJECT WOOH");

	app = new AppManager();
	app->init();	// sets up OpenGL, lights, the world, and all other stuff

	glutDisplayFunc(render);
	
	glutKeyboardFunc(keybDown);
	glutKeyboardUpFunc(keybUp);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutPassiveMotionFunc(motionFunc);

	glutTimerFunc(1000.0 / FRAMES_PER_SECOND, update, 0);
	glutMainLoop();
}


void loadData()
{
	ifstream file("config.txt", ios::in);

	if (file.fail())
	{
		cerr << "Unable to open config.txt" << endl;
		exit(1);
	}


	file >> FRAMES_PER_SECOND;
	file >> WINDOW_WIDTH;
	file >> WINDOW_HEIGHT;
	ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
	file >> FIELD_OF_VIEW;
	file >> MOVE_SPEED;
	file >> GRAVITATIONAL_CONSTANT;
	file >> POLYGON_WIDTH;
	file >> POLYGON_HEIGHT;

	file.close();
}