/*
/ This class should maybe be called "FlatSurface" or something like that instead, since it is used
/ for more things than just walls (it works just as well for floors and ceilings and stuff like that).
/ Has a width and height, and a texture which is repeatedly applied to the surface. texWidth and
/ texHeight specifies the size of the texture, so that the size does not depend of the number
/ of polygons making up the wall (this number is specified in config.txt)
*/

#ifndef WALL_H
#define WALL_H


class Cube
{
private:

public:
	Cube(float _x, float _y, float _z);
	~Cube();

	// if this is called, then the wall will be given a backside to, using the specified texture information
    int init_resources();
    void free_resources();
    void logic();
	void draw();
};

#endif