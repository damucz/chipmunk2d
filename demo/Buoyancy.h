#ifndef __BUOYANCY_H
#define __BUOYANCY_H

#include "demo.h"

class Buoyancy : public ChipmunkDemo
{
public:
    Buoyancy();
    ~Buoyancy() {};

    virtual cpSpace* Init();
    
protected:
    static cpFloat KScalarBody(cpBody *body, cpVect point, cpVect n);
    static cpBool WaterPreSolve(cpArbiter *arb, cpSpace *space, void *ptr);
};

#endif
