#ifndef PARTICALSYSTEM_H
#define PARTICALSYSTEM_H
#include <vector>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "particle.h"
#define PI 3.1415926
using namespace std;
class ParticalSystem
{
public:
    ParticalSystem();
    ParticalSystem(int _count,float _gravity){ptlCount=_count;gravity=_gravity;};
    void init();
    void simulate(float dt);
    void aging(float dt);
    void applyGravity();
    void kinematics(float dt);
    void render();
    virtual ~ParticalSystem();
protected:
private:
    int ptlCount;
    float gravity;
    GLUquadricObj *mySphere;
    vector<Particle> particles;
};

#endif // PARTICALSYSTEM_H