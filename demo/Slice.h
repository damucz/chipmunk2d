#ifndef __SLICE_H
#define __SLICE_H

#include "demo.h"

class Slice : public ChipmunkDemo
{
public:
    Slice();
    ~Slice() {};

    virtual cpSpace* Init();
    void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    static void ClipPoly(cpSpace *space, cpShape *shape, cpVect n, cpFloat dist);
    static void SliceShapePostStep(cpSpace *space, cpShape *shape, struct SliceContext *context);
    static void SliceQuery(cpShape *shape, cpVect point, cpVect normal, cpFloat alpha, struct SliceContext *context);

    cpVect sliceStart;

    int touchId;
    cpVect lastTouchPoint;
};

#endif
