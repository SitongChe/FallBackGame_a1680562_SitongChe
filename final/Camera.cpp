#include <vector>
#include <cmath>
#include "Camera.h"
#include "AppManager.h"
#include "Constants.h"
#include "Material.h"
#include "tiny_obj_loader.h"

#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define BASE_HEIGHT 4.0/2
#define BASE_RADIUS 1.0/2
#define HEAD_HEIGHT 1.25/2
#define HEAD_RADIUS 0.75/2
#define NECK_HEIGHT 0.5/2
#define EYE_LEVEL 0.75/2
#define NOSE_LENGTH 0.5/2
#define LOWER_ARM_HEIGHT 2.0/2
#define LOWER_ARM_WIDTH 0.5/2
#define UPPER_ARM_HEIGHT 1.25/2
#define UPPER_ARM_WIDTH 0.5/2
#define ARM_TRANSLATION 0.22/2
#define alpha 0.0
#define pi 3.14159265
static GLfloat thetaa[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static GLint axis = 0;
GLUquadricObj *p = gluNewQuadric();
GLfloat x = 0.0;
GLfloat y = 0.0;
GLfloat xpos = 0.0;
GLfloat ypos = 0.0;
GLfloat zpos = 0.0;
GLfloat ambient[3];
GLfloat diffuse[3];
GLfloat specular[3];
GLfloat shiness[] = {50.0f};
float width = 500;
float height = 500;
using namespace std;

extern AppManager * app;



Camera::Camera(float x, float y, float z) : pos(x, y, z), vel(0.0, 0.0, 0.0), SPEED(MOVE_SPEED), CHAR_HEIGHT(1.6), CHAR_WIDTH(0.5), CHAR_DEPTH(0.5)
{
	phi = 0.0f;
	theta = 0.0f;

	for (int i = 0; i < 4; i++)
		buttonPressed[i] = false;

	box = new BoundingBox(pos, CHAR_WIDTH, CHAR_HEIGHT, CHAR_DEPTH);

	roomNo = -1;
}

Camera::~Camera()
{
	delete box;
}

void Camera::update()
{
	// brace yourself... xVel is set to -SPEED, 0 or SPEED. However, this is just the relative
	// speed in the axis which goes perpendicular to the view vector and the y-axis, so to speak.
	// Same reasoning applies to zVel. Thus, the actual change in xPos and zPos needs to take
	// into account these variables, along with the value of theta, in the following rather
	// complex assignments:
	std::vector<Object*> objects = app->getObjects();


	pos.addX(vel.getX() * cos(theta * 3.1416 / 180.0) + vel.getZ() * sin(theta * 3.1416 / 180.0));
	box->setPos(pos.getX(), pos.getY() + CHAR_HEIGHT / 2, pos.getZ());
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (box->collision(objects[i]->getBox()))	// we collided with a box! undo the move
		{
            cout << '\a';
			pos.addX(-(vel.getX() * cos(theta * 3.1416 / 180.0) + vel.getZ() * sin(theta * 3.1416 / 180.0)));
			int no = objects[i]->getBox()->getID();
			if (no != -1)	// if it's a proper room number, assign it to roomNo
				roomNo = no;
			break;
		}
	}


	vel.addY(-GRAVITATIONAL_CONSTANT);
	pos.addY(vel.getY());	// doesn't do anything here, but if we include, say, ability to jump, it'll be important
	box->setPos(pos.getX(), pos.getY() + CHAR_HEIGHT / 2, pos.getZ());
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (box->collision(objects[i]->getBox()))	// we collided with a box! undo the move
		{
            cout << '\a';
			pos.addY(-vel.getY());
			int no = objects[i]->getBox()->getID();
			if (no != -1)	// if it's a proper room number, assign it to roomNo
				roomNo = no;
			vel.setY(0.0);
			break;
		}
	}


	pos.addZ(vel.getX() * sin(theta * 3.1416 / 180.0) - vel.getZ() * cos(theta * 3.1416 / 180.0));
	box->setPos(pos.getX(), pos.getY() + CHAR_HEIGHT / 2, pos.getZ());
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (box->collision(objects[i]->getBox()))	// we collided with a box! undo the move
		{
            cout << '\a';
			pos.addZ(-(vel.getX() * sin(theta * 3.1416 / 180.0) - vel.getZ() * cos(theta * 3.1416 / 180.0)));
			int no = objects[i]->getBox()->getID();
			if (no != -1)	// if it's a proper room number, assign it to roomNo
				roomNo = no;
			break;
		}
	}
}

void Camera::setCamera()
{

	glRotatef(theta, 0, 1, 0);	// rotate about the y-axis
	glRotatef(phi, cos(theta * 3.1416 / 180.0f), 0, sin(theta * 3.1416 / 180.0f));	// rotate about the axis perpendicular to our up-vector and our forward-vector...
	glTranslatef(-pos.getX(), -pos.getY() - CHAR_HEIGHT, -pos.getZ());
}

int CreateVBO(GLfloat *vertices, int size)
{
    GLuint vboId;                              // ID of VBO
    
    // generate a new VBO and get the associated ID
    glGenBuffersARB(1, &vboId);
    
    // bind VBO in order to use
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId);
    
    // upload data to VBO
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, vertices, GL_STATIC_DRAW_ARB);
    
    // it is safe to delete after copying data to VBO

    return vboId;

}
void base (void) {
    double angle, angleInc;
    int i;
    angle = pi / 180;
    angleInc = angle;
    glPushMatrix();
    
    ambient[0] = 1.0; ambient[1] = 0.0; ambient[2] = 0.0;
    diffuse[0] = 1.0; diffuse[1] = 0.0; diffuse[2] = 0.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    glRotatef (-90.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(p, GLU_FILL);
    gluCylinder (p, BASE_RADIUS, BASE_RADIUS, BASE_HEIGHT, 20, 20);
    
    glPopMatrix();
    
    glPushMatrix();
    
    gluQuadricDrawStyle (p, GLU_FILL);
    glTranslatef (0.0, BASE_HEIGHT, 0.0);
    glRotatef (-90.0, 1.0, 0.0, 0.0);
    gluDisk (p, 0.0, BASE_RADIUS, 20, 20);
    glTranslatef (0.0, 0.0, -BASE_HEIGHT);
    gluDisk (p, 0.0, BASE_RADIUS, 20, 20);
    glPopMatrix();
}


void neck (void) {
    glPushMatrix();
    
    ambient[0] = 1.0; ambient[1] = 1.0; ambient[2] = 0.0;
    diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 0.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    glTranslatef(0.0, BASE_HEIGHT, 0.0);
    glRotatef (-90.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(p, GLU_FILL);
    gluCylinder (p, HEAD_RADIUS/2, HEAD_RADIUS/2, HEAD_HEIGHT, 8, 6);
    glPopMatrix();
}

void head (void) {
    glPushMatrix();
    
    ambient[0] = 1.0; ambient[1] = 0.0; ambient[2] = 1.0;
    diffuse[0] = 1.0; diffuse[1] = 0.0; diffuse[2] = 1.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    glRotatef (-90.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(p, GLU_FILL);
    gluCylinder (p, HEAD_RADIUS, HEAD_RADIUS, HEAD_HEIGHT, 20, 20);
    
    glPushMatrix();
    
    gluDisk (p, 0.0, HEAD_RADIUS, 20, 20);
    glTranslatef (0.0, 0.0, HEAD_HEIGHT);
    gluDisk (p, 0.0, HEAD_RADIUS, 20, 20);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef (0.25, -HEAD_RADIUS+0.12, EYE_LEVEL);
    
    ambient[0] = 1.0; ambient[1] = 1.0; ambient[2] = 1.0;
    diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 1.0;
    specular[0] = 0.5; specular[1] = 0.5; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    gluQuadricDrawStyle(p, GLU_FILL);
    gluSphere (p, 0.125, 6, 6);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef (-0.25, -HEAD_RADIUS+0.12, EYE_LEVEL);
    ambient[0] = 1.0; ambient[1] = 1.0; ambient[2] = 1.0;
    diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 1.0;
    specular[0] = 0.5; specular[1] = 0.5; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    
    gluQuadricDrawStyle(p, GLU_FILL);
    gluSphere (p, 0.125, 6, 6);
    glPopMatrix();
    
    glPushMatrix();
    ambient[0] = 1.0; ambient[1] = 0.5; ambient[2] = 0.0;
    diffuse[0] = 1.0; diffuse[1] = 0.5; diffuse[2] = 0.0;
    specular[0] = 0.5; specular[1] = 0.5; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    glTranslatef(0.0, -HEAD_RADIUS, NOSE_LENGTH);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(p, GLU_FILL);
    gluCylinder(p, 0.125, 0, NOSE_LENGTH, 8,6);
    glPopMatrix();
    
    glPopMatrix();
}

void lower_rarm(void) {
    glPushMatrix();
    
    ambient[0] = 0.0; ambient[1] = 1.0; ambient[2] = 0.0;
    diffuse[0] = 0.0; diffuse[1] = 1.0; diffuse[2] = 0.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    glTranslatef(0.0, 0.5 * LOWER_ARM_HEIGHT, ARM_TRANSLATION);
    glScalef(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
    glutSolidCube(1.0);
    glPopMatrix();
}

void lower_larm(void) {
    glPushMatrix();
    
    ambient[0] = 0.0; ambient[1] = 1.0; ambient[2] = 0.0;
    diffuse[0] = 0.0; diffuse[1] = 1.0; diffuse[2] = 0.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    glTranslatef(0.0, 0.5 * LOWER_ARM_HEIGHT, -ARM_TRANSLATION);
    glScalef(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
    glutSolidCube(1.0);
    glPopMatrix();
}

void upper_rarm(void) {
    glPushMatrix();
    
    ambient[0] = 0.0; ambient[1] = 0.0; ambient[2] = 1.0;
    diffuse[0] = 0.0; diffuse[1] = 0.0; diffuse[2] = 1.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    glTranslatef(0.0, 0.5 * UPPER_ARM_HEIGHT, ARM_TRANSLATION);
    glScalef(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
    glutSolidCube(1.0);
    glPopMatrix();
}

void upper_larm(void) {
    glPushMatrix();
    
    ambient[0] = 0.0; ambient[1] = 0.0; ambient[2] = 1.0;
    diffuse[0] = 0.0; diffuse[1] = 0.0; diffuse[2] = 1.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    
    glTranslatef(0.0, 0.5 * UPPER_ARM_HEIGHT, -ARM_TRANSLATION);
    glScalef(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
    glutSolidCube(1.0);
    glPopMatrix();
}
void Camera::drawCharacter()
{
//    char* filename="obj.obj";
//    string a=LoadObj(shape,material,filename,NULL);
//    if(a!="")
//        cout<<"fail to open obj file"<<endl;

	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	glRotatef(180 - theta, 0, 1, 0);
    
    glPushMatrix();
    ambient[0] = 1.0; ambient[1] = 0.3; ambient[2] = 0.3;
    diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 1.0;
    specular[0] = 0.7; specular[1] = 0.6; specular[2] = 0.5;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);

    
    glTranslatef (xpos, ypos, zpos);
    glRotatef(thetaa[0], 0.0, 1.0, 0.0);
    base();
    neck();
    
    glPushMatrix();
    glTranslatef(0.0, BASE_HEIGHT + HEAD_HEIGHT/2, 0.0);
    glRotatef(thetaa[2], 1.0, 0.0, 0.0);
    glRotatef(thetaa[1], 0.0, 1.0, 0.0);
    head();
    glPopMatrix();
    
    
    glPushMatrix();
    glTranslatef(BASE_RADIUS, BASE_HEIGHT - BASE_RADIUS / 2, 0.0);
    glRotatef(180.0, 0.0, 0.0, 1.0);
    glRotatef(270.0, 0.0, 1.0, 0.0);
    glRotatef(thetaa[4], 0.0, 0.0, 1.0);
    lower_rarm();
    glTranslatef(0.0, LOWER_ARM_HEIGHT, 0.0);
    glRotatef(0.0, 0.0, 0.0, 180.0);
    glRotatef(thetaa[6], 0.0, 0.0, 1.0);
    upper_rarm();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-BASE_RADIUS, BASE_HEIGHT - BASE_RADIUS / 2, 0.0);
    glRotatef(180.0, 0.0, 0.0, 1.0);
    glRotatef(270.0, 0.0, 1.0, 0.0);
    glRotatef(thetaa[3], 0.0, 0.0, 1.0);
    lower_larm();
    glTranslatef(0.0, LOWER_ARM_HEIGHT, 0.0);
    glRotatef(0.0, 0.0, 0.0, 180.0);
    glRotatef(thetaa[5], 0.0, 0.0, 1.0);
    upper_larm();
    glPopMatrix();
//    gluQuadricDrawStyle (p, GLU_FILL);
//    glTranslatef (0.0, BASE_HEIGHT, 0.0);
//    glRotatef (-90.0, 1.0, 0.0, 0.0);
//    gluDisk (p, 0.0, BASE_RADIUS, 20, 20);
//    glTranslatef (0.0, 0.0, -BASE_HEIGHT);
//    gluDisk (p, 0.0, BASE_RADIUS, 20, 20);


//    int size = shape[0].mesh.positions.size() * sizeof(GLfloat);
//    int m_vboId = CreateVBO(&shape[0].mesh.positions[0], size);
//    size=0;
//    if(size>0)
//    {
//	glBindTexture(GL_TEXTURE_2D, 0);
//	Material::setWhiteMaterial();
//
//    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboId);
//    
//    // Activate array-based data
//    glEnableClientState(GL_VERTEX_ARRAY);
//    
//    // Stride of 3, floats
//    glVertexPointer(3, GL_FLOAT, 0, 0);
//    
//    // Draw triangles
//    glDrawArrays(GL_TRIANGLES, 0, shape[0].mesh.positions.size());
//    
//    // Switch off vertex array data
//    glDisableClientState(GL_VERTEX_ARRAY);
//    
//    // Bind with 0 to switch back to default pointer operation
//    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
//    }
	// draw the front
    
  

    
//    
//	glNormal3f(0, 0, 1);
//	glBegin(GL_TRIANGLES);
//		glVertex3f(-1.0, 1.0, 0.0);
//		glVertex3f(1.0, 1.0, 0.0);
//		glVertex3f(0.0, 2.0, 0.0);
//	glEnd();
//
//	// draw the back
//	glRotatef(180, 0, 1, 0);
//	glBegin(GL_TRIANGLES);
//		glVertex3f(-1.0, 1.0, 0.0);
//		glVertex3f(1.0, 1.0, 0.0);
//		glVertex3f(0.0, 2.0, 0.0);
//	glEnd();

	glPopMatrix();
}

void Camera::moveForward(bool mov)
{
	if (buttonPressed[FWD] == mov)	// makes no difference; just return
		return;

	buttonPressed[FWD] = mov;

	if (mov == true)	// if we're set to start moving forward, move forward
		vel.setZ(SPEED);
	else if (buttonPressed[BACK] == true)	// otherwise, if we're still pressing back, move back
		vel.setZ(-SPEED);
	else	// otherwise, stop moving in either direction
		vel.setZ(0.0);
}

void Camera::moveBack(bool mov)
{
	if (buttonPressed[BACK] == mov)
		return;

	buttonPressed[BACK] = mov;

	if (mov == true)	// if we're set to start moving back, move back
		vel.setZ(-SPEED);
	else if (buttonPressed[FWD] == true)	// otherwise, if we're still pressing forward, move forward
		vel.setZ(SPEED);
	else	// otherwise, stop moving in either direction
		vel.setZ(0.0);
}

void Camera::moveLeft(bool mov)
{
	if (buttonPressed[LEFT] == mov)
		return;

	buttonPressed[LEFT] = mov;

	if (mov == true)	// if we're set to start moving left, move left
		vel.setX(-SPEED);
	else if (buttonPressed[RIGHT] == true)	// otherwise, if we're still pressing right, move right
		vel.setX(SPEED);
	else	// otherwise, stop moving in either direction
		vel.setX(0.0);
}

void Camera::moveRight(bool mov)
{
	if (buttonPressed[RIGHT] == mov)
		return;

	buttonPressed[RIGHT] = mov;

	if (mov == true)	// if we're set to start moving right, move right
		vel.setX(SPEED);
	else if (buttonPressed[LEFT] == true)	// otherwise, if we're still pressing left, move left
		vel.setX(-SPEED);
	else	// otherwise, stop moving in either direction
		vel.setX(0.0);
}

void Camera::rotLeftRight(float rot)
{
	theta += rot *8.0f;

	if (theta > 360)
		theta -= 360;

	else if (theta < 0)
		theta += 360;
}

void Camera::rotUpDown(float rot)
{
	phi += rot *8.0f;

	if (phi > 90)
		phi = 90;
	else if (phi < -90)
		phi = -90;
}