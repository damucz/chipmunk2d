#ifndef __CONVEX_H
#define __CONVEX_H

#include "demo.h"

class Convex : public ChipmunkDemo
{
public:
    Convex();
    ~Convex() {};

    virtual cpSpace* Init();

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpShape *shape;
};

#endif
