#ifndef __STICKY_H
#define __STICKY_H

#include "demo.h"

class Sticky : public ChipmunkDemo
{
public:
    Sticky();
    ~Sticky() {};

    virtual cpSpace* Init();

protected:
    static void PostStepAddJoint(cpSpace *space, void *key, void *data);
    static cpBool StickyPreSolve(cpArbiter *arb, cpSpace *space, void *data);
    static void PostStepRemoveJoint(cpSpace *space, void *key, void *data);
    static void StickySeparate(cpArbiter *arb, cpSpace *space, void *data);
};

#endif
