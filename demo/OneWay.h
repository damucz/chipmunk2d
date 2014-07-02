#ifndef __ONEWAY_H
#define __ONEWAY_H

#include "demo.h"

class OneWay : public ChipmunkDemo
{
public:
    OneWay();
    ~OneWay() {};

    virtual cpSpace* Init();

protected:
    enum CollisionTypes {
        COLLISION_TYPE_ONE_WAY = 1,
    };

    typedef struct OneWayPlatform {
        cpVect n; // direction objects may pass through
    } OneWayPlatform;

    OneWayPlatform platformInstance;

    static cpBool OneWay::PreSolve(cpArbiter *arb, cpSpace *space, void *ignore);
};

#endif
