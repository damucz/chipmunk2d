#ifndef __PUMP_H
#define __PUMP_H

#include "demo.h"

class Pump : public ChipmunkDemo
{
public:
    Pump();
    ~Pump() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpConstraint *motor;
    static const int numBalls = 5;
    cpBody *balls[numBalls];

    cpBody *AddBall(cpSpace *space, cpVect pos);

    int touchId;
    cpVect startTouchPoint;
};

#endif
