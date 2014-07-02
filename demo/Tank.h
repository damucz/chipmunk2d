#ifndef __TANK_H
#define __TANK_H

#include "demo.h"

class Tank : public ChipmunkDemo
{
public:
    Tank();
    ~Tank() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpBody *tankBody, *tankControlBody;

    int touchId;
    cpVect lastTouchPoint;

    cpBody *AddBox(cpSpace *space, cpFloat size, cpFloat mass);
};

#endif
