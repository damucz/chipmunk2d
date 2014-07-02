#ifndef __TUMBLE_H
#define __TUMBLE_H

#include "demo.h"

class Tumble : public ChipmunkDemo
{
public:
    Tumble();
    ~Tumble() {};

    virtual cpSpace* Init();

protected:
    cpBody *KinematicBoxBody;

    void AddBox(cpSpace *space, cpVect pos, cpFloat mass, cpFloat width, cpFloat height);
    void AddSegment(cpSpace *space, cpVect pos, cpFloat mass, cpFloat width, cpFloat height);
    void AddCircle(cpSpace *space, cpVect pos, cpFloat mass, cpFloat radius);
};

#endif
