#ifndef __SMOOTH_H
#define __SMOOTH_H

#include "demo.h"

class Smooth : public ChipmunkDemo
{
public:
    Smooth();
    ~Smooth() {};

    virtual cpSpace* Init();

protected:
    static cpBool DrawContacts(cpArbiter *arb, cpSpace *space, void *data);
};

#endif
