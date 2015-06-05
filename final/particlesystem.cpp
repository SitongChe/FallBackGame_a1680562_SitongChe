#include "particlesystem.h"

ParticalSystem::ParticalSystem()
{
    //ctor
}

ParticalSystem::~ParticalSystem()
{
    //dtor
}

void ParticalSystem::init()
{
    int i;
    srand(unsigned(time(0)));
    Color colors[3]={{1,0,0,1},{1,0,1,1}};
    for(i=0;i<ptlCount;i++)
    {
        //theta =(rand()%361)/360.0* 2*PI;
        Particle tmp={Vector3f(0,0,0),Vector3f(((rand()%50)-26.0f),((rand()%50)-26.0f),((rand()%50)-26.0f)),Vector3f(0,0,0),colors[0],0.0f,0.0001*(rand()%10),0.003f};
        particles.push_back(tmp);
    }
    mySphere=gluNewQuadric();
}
void ParticalSystem::simulate(float dt)
{
    aging(dt);
    applyGravity();
    kinematics(dt);
}
void ParticalSystem::aging(float dt)
{
    for(vector<Particle>::iterator iter=particles.begin();iter!=particles.end();iter++)
    {
        iter->age+=dt;
        if(iter->age>iter->life)
        {
            iter->position=Vector3f(0,0,0);
            iter->age=0.0;
            iter->velocity=Vector3f(((rand()%30)-15.0f),((rand()%30)-11.0f),((rand()%30)-15.0f));
        }
    }
}
void ParticalSystem::applyGravity()
{
    for(vector<Particle>::iterator iter=particles.begin();iter!=particles.end();iter++)
        iter->acceleration=Vector3f(0,gravity,0);
}

void ParticalSystem::kinematics(float dt)
{
    for(vector<Particle>::iterator iter=particles.begin();iter!=particles.end();iter++)
    {
        Vector3f a(iter->position.getX()+iter->velocity.getX()*dt,iter->position.getY()+iter->velocity.getY()*dt,iter->position.getZ()+iter->velocity.getZ()*dt);
        Vector3f b(iter->velocity.getX()+iter->acceleration.getX()*dt,iter->velocity.getY()+iter->acceleration.getY()*dt,iter->velocity.getZ()+iter->acceleration.getZ()*dt);
        iter->velocity = b;
        iter->position = a;
    }
}
void ParticalSystem::render()
{
    
    for(vector<Particle>::iterator iter=particles.begin();iter!=particles.end();iter++)
    {
        float alpha = 1 - iter->age / iter->life;//calculate the alpha value according to the age of particle.
        Vector3f tmp=iter->position;
        glColor4f(iter->color.r,iter->color.g,iter->color.b,alpha);
        glPushMatrix();
        glTranslatef(tmp.getX(),tmp.getY(),tmp.getZ());
        gluSphere(mySphere,iter->size, 32, 16);
        glPopMatrix();
    }
    
}