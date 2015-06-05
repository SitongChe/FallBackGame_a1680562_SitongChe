#ifndef PARTICLE_H
#define PARTICLE_H
#include "Vector3f.h"
typedef struct
{
    float r;
    float g;
    float b;
    float alpha;
}Color;

typedef struct
{
    Vector3f position;
    Vector3f velocity;
    Vector3f acceleration;
    Color color;
    float age;
    float life;
    float size;
}Particle;

#endif // PARTICLE_H