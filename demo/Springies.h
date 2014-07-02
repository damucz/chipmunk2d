#ifndef __SPRINGIES_H
#define __SPRINGIES_H

#include "demo.h"

class Springies : public ChipmunkDemo
{
public:
    Springies();
    ~Springies() {};

    virtual cpSpace* Init();

protected:
    static cpFloat SpringForce(cpConstraint *spring, cpFloat dist);
    cpConstraint *NewSpring(cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, cpFloat restLength, cpFloat stiff, cpFloat damp);
    cpBody *AddBar(cpSpace *space, cpVect a, cpVect b, int group);
};

#endif
