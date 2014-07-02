#ifndef __UNICYCLE_H
#define __UNICYCLE_H

#include "demo.h"

class Unicycle : public ChipmunkDemo
{
public:
    Unicycle();
    ~Unicycle() {};

    virtual cpSpace* Init();

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpBody *balance_body;
    cpFloat balance_sin;
    //cpFloat last_v = 0.0;

    cpBody *wheel_body;
    cpConstraint *motor;

    int touchId;
    cpVect lastTouchPoint;

    static cpFloat BiasCoef(cpFloat errorBias, cpFloat dt);
    static void MotorPreSolve(cpConstraint *motor, cpSpace *space);
};

#endif
