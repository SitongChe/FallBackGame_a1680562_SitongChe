/*
/ This simple class stores a vector in three dimensions. Is used both for positions and vectors;
/ the user of the class is responsible for proper usage. Is not complete; methods has been added
/ according to need. No operator overloading has yet been added.
*/


#ifndef VECTOR3F_H
#define VECTOR3F_H

class Vector3f
{
private:
	float * comp;
public:
	Vector3f();
	Vector3f(float _x, float _y, float _z);
	Vector3f(const Vector3f &arg);
	Vector3f& operator=(const Vector3f &arg);
	~Vector3f();

	float * getComps() { return comp; }
	void setComps(float * _comps) { comp = _comps; }
	void setComps(float _x, float _y, float _z) { comp[0] = _x; comp[1] = _y; comp[2] = _z; }
	
	float getX() { return comp[0]; }
	float getY() { return comp[1]; }
	float getZ() { return comp[2]; }

	void setX(float x) { comp[0] = x; }
	void setY(float y) { comp[1] = y; }
	void setZ(float z) { comp[2] = z; }

	void addX(float x) { comp[0] += x; }
	void addY(float y) { comp[1] += y; }
	void addZ(float z) { comp[2] += z; }

	// returns the dot product of this vector and the argument vector
	float dotProduct(Vector3f &arg);

	// returns the length of the vector
	float length();
};


#endif