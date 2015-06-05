#include <cmath>
#include "Constants.h"
#include "BezierSpline.h"
#include <GLUT.h>
using namespace std;

BezierSpline::BezierSpline(Vector3f &_c0, Vector3f &_c1, Vector3f &_c2, Vector3f &_c3, int _subDivisions, float _radius, float _extension)
{
	radius = _radius;
	extension = _extension;

	quadric = gluNewQuadric();

	// here starts the work of calculating the curvePoints.

	// we use this variable in calculating the number of points which will be used for the curve
	size = 2;
	int tmpN = _subDivisions;

	while (tmpN > 0)
	{
		size += size-1;
		--tmpN;
	}

	curvePoints = new Vector3f[size];
	
	// we already know the first and the last point
	curvePoints[0] = _c0;
	curvePoints[size-1] = _c3;

	divide(_c0, _c1, _c2, _c3, _subDivisions, size/2, size/4);	// it just works  ;D

	// calculate the angles and lengths between consecutive points
	calcValues();
}

BezierSpline::~BezierSpline()
{
	delete [] curvePoints;
	delete [] angles;
	delete [] lengths;
	gluDeleteQuadric(quadric);
}

void BezierSpline::divide(Vector3f &_p0, Vector3f &_p1, Vector3f &_p2, Vector3f &_p3, int n, int newPosition, int stepSize)
{
	if (n == 0)
		return;

	// midpoint of edge between _p0 and _p1
	Vector3f mid1( (_p0.getX() + _p1.getX()) / 2, (_p0.getY() + _p1.getY()) / 2, (_p0.getZ() + _p1.getZ()) / 2);
	// midpoint of edge between _p1 and _p2
	Vector3f mid2( (_p1.getX() + _p2.getX()) / 2, (_p1.getY() + _p2.getY()) / 2, (_p1.getZ() + _p2.getZ()) / 2);
	// midpoint of edge between _p2 and _p3
	Vector3f mid3( (_p2.getX() + _p3.getX()) / 2, (_p2.getY() + _p3.getY()) / 2, (_p2.getZ() + _p3.getZ()) / 2);

	// midpoint of edge between mid1 and mid2
	Vector3f mid12( (mid1.getX() + mid2.getX()) / 2, (mid1.getY() + mid2.getY()) / 2, (mid1.getZ() + mid2.getZ()) / 2);
	// midpoint of edge between mid2 and mid3
	Vector3f mid23( (mid2.getX() + mid3.getX()) / 2, (mid2.getY() + mid3.getY()) / 2, (mid2.getZ() + mid3.getZ()) / 2);

	// final midpoint, which is added as a point on the curve!
	Vector3f curvePoint( (mid12.getX() + mid23.getX()) / 2, (mid12.getY() + mid23.getY()) / 2, (mid12.getZ() + mid23.getZ()) / 2);

	curvePoints[newPosition] = curvePoint;

	// subdivide! This is pure magic.
	divide(_p0, mid1, mid12, curvePoint, n-1, newPosition - stepSize, stepSize/2);
	divide(curvePoint, mid23, mid3, _p3, n-1, newPosition + stepSize, stepSize/2);
}

void BezierSpline::calcValues()
{
	angles = new float[size-1];
	lengths = new float[size-1];

	for (int i = 0; i < size-1; i++)
	{
		angles[i] = 90 - atan2(curvePoints[i+1].getY() - curvePoints[i].getY(), curvePoints[i+1].getX() - curvePoints[i].getX()) * 180.0 / 3.1416;
		lengths[i] = sqrt(dist(curvePoints[i+1], curvePoints[i]) ) + extension;
	}
}

void BezierSpline::draw()
{
	for (int i = 0; i < size-1; i++)
	{
		glPushMatrix();
		glTranslatef(curvePoints[i].getX(), curvePoints[i].getY(), curvePoints[i].getZ());
		glRotatef(-90, 1, 0, 0);
		glRotatef(angles[i], 0, 1, 0);

		gluCylinder(quadric, radius, radius, lengths[i], 10, 10);

		glPopMatrix();
	}
}