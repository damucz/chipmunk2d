#ifndef __PLINK_H
#define __PLINK_H

#include "demo.h"

class Plink : public ChipmunkDemo
{
public:
    Plink();
    ~Plink() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    static cpFloat pentagon_mass;
    static cpFloat pentagon_moment;

    static void EachBody(cpBody *body, void *unused);
};

#endif
