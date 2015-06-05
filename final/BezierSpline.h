/*
/	This class creates a bezier spline, drawn as a collection of cylinders following the spline created by the given control points.
/	It uses the recursive subdivision algorithm for calculating points along the curve, then calculates angles and distances between
/	these points, which is then used along with the gluCylinder() function.
/	Please note that this has not been made to work in three dimensions quite yet; all control points should lie in a plane.
*/


#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "Vector3f.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class BezierSpline
{
private:
	// The points on the curve, generated by recursive subdivision
	Vector3f * curvePoints;
	int size;	// the size of the above array

	float * angles;		// the angles between two consecutive points
	float * lengths;	// the lengths between two consecutive points

	float radius;	// the radius of the cylinder
	float extension;// this is how much we will prolong each cylinder, to avoid gaps. Is set by the user of the class depending on his need

	unsigned int textureID;	// if we want to apply a texture, we use this value

	GLUquadric * quadric;	// used for drawing the cylinders

	// recursive method used to calculate the points on the curve
	void divide(Vector3f &_p0, Vector3f &_p1, Vector3f &_p2, Vector3f &_p3, int n, int newPosition, int stepSize);

	// calculates the angles and lengths between consecutive points. Must be called after calculating the curvePoints!
	void calcValues();
public:
	BezierSpline(Vector3f &_c0, Vector3f &_c1, Vector3f &_c2, Vector3f &_c3, int _subDivisions, float _radius, float _extension);
	~BezierSpline();

	// draws the curved cylinder thingy!
	void draw();

	void update() {}
};

#endif