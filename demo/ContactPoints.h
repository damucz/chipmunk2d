#ifndef __CONTACTPOINTS_H
#define __CONTACTPOINTS_H

#include "demo.h"

class ContactPoints : public ChipmunkDemo
{
public:
    ContactPoints();
    ~ContactPoints() {};

    virtual cpSpace* Init();

protected:
    static cpBool NeverCollide(cpArbiter *arb, cpSpace *space, void *data);
};

#endif
