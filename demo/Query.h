#ifndef __QUERY_H
#define __QUERY_H

#include "demo.h"

class Query : public ChipmunkDemo
{
public:
    Query();
    ~Query() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpVect QUERY_START;
    int touchId;
    cpVect lastTouchPoint;
};

#endif
