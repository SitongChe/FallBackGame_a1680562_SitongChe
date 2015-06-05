
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <fstream>
#include <ctime>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Viewer.h"
#include "AppManager.h"
#include "Constants.h"

using namespace std;
int programID;
Viewer *Camera;
AppManager * app;

static void DrawInit()
{
    // OpenGL one-time initialisation
    glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void render()
{
	
    //glUseProgram(programID);
    app->render();
    // First load the viewing matrix from the current camera
//    glm::mat4 viewMatrix;
//    //viewMatrix = Camera->getViewMtx();
//    
//    // Load it to the shader program
//    int viewHandle = glGetUniformLocation(programID, "view");
//    if (viewHandle == -1) {
//        std::cout << "Uniform: view is not an active uniform label\n";
//    }
//    glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );
    
    
    // Now draw the table...
//    TheTable->render(programID);
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

	if (key == 'q')
	{
		delete app;
		exit(0);
	}
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
//void initShaders() {
//    
//    glGenVertexArrays(1, &vaoHandle);
//    glBindVertexArray(vaoHandle);
//    
//    int vertLoc = glGetAttribLocation(programID, "a_vertex");
//    int colourLoc = glGetAttribLocation(programID, "a_colour");
//    
//}
int main(int argc, char ** argv)
{
	loadData();
	srand((unsigned int)time(NULL));	// for randomizing numbers

	glutInit(&argc, argv);
    //glutInitDisplayString("rgba double core");
#ifdef __APPLE__
   glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#endif
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("FINAL PROJECT");

//#ifndef MACOSX
//    GLenum glew_status = glewInit();
//    if (glew_status != GLEW_OK) {
//        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
//        return EXIT_FAILURE;
//    }
//    
//    if (!GLEW_VERSION_2_0) {
//        fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
//        return EXIT_FAILURE;
//    }
//#endif
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }


    GLenum err = glGetError();
    if (err != 0) {
        fprintf(stderr, "GLEW initialisation error %d\n", err);
    }
    
//    programID = LoadShaders("mview.vert", "mview.frag");
//    if (programID == 0) {
//        fprintf(stderr, "Error loading shaders\n");
//        return 1;
//    }
    //glUseProgram(programID);
    // Initialise OpenGL, the scene and the cameras
//    DrawInit();
    
    
//    init_resources();
    app = new AppManager();
    app->init();	// sets up OpenGL, lights, the world, and all other stuff
//    TheTable = new Table(tableWidth, tableLength, programID );
//    initShaders();
//    glBindVertexArray(vaoHandle);
	glutDisplayFunc(render);
	glutKeyboardFunc(keybDown);
	glutKeyboardUpFunc(keybUp);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutPassiveMotionFunc(motionFunc);
    // glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_CULL_FACE);
    
    // glAlphaFunc(GL_GREATER, 0.1);
    // glEnable(GL_ALPHA_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


	glutTimerFunc(1000.0 / FRAMES_PER_SECOND, update, 0);
	glutMainLoop();
    return 0;
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