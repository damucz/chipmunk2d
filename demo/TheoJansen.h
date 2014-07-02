#ifndef __THEOJANSEN_H
#define __THEOJANSEN_H

#include "demo.h"

class TheoJansen : public ChipmunkDemo
{
public:
    TheoJansen();
    ~TheoJansen() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    static const cpFloat seg_radius;
    cpConstraint *motor;
    cpBody *chassis;

    int touchId;
    cpVect lastTouchPoint;

    void MakeLeg(cpSpace *space, cpFloat side, cpFloat offset, cpBody *chassis, cpBody *crank, cpVect anchor);
};

#endif
