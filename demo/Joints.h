#ifndef __JOINTS_H
#define __JOINTS_H

#include "demo.h"

class Joints : public ChipmunkDemo
{
public:
    Joints();
    ~Joints() {};

    virtual cpSpace* Init();

protected:
    cpBody *AddBall(cpSpace *space, cpVect pos, cpVect boxOffset);
    cpBody *AddLever(cpSpace *space, cpVect pos, cpVect boxOffset);
    cpBody *AddBar(cpSpace *space, cpVect pos, cpVect boxOffset);
    cpBody *AddWheel(cpSpace *space, cpVect pos, cpVect boxOffset);
    cpBody *AddChassis(cpSpace *space, cpVect pos, cpVect boxOffset);
};

#endif
