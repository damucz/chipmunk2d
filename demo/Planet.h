#ifndef __PLANET_H
#define __PLANET_H

#include "demo.h"

class Planet : public ChipmunkDemo
{
public:
    Planet();
    ~Planet() {};

    virtual cpSpace* Init();

protected:
    cpBody *planetBody;
    static const cpFloat gravityStrength;

    static void PlanetGravityVelocityFunc(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);
    cpVect RandPos(cpFloat radius);
    void AddBox(cpSpace *space);
};

#endif
