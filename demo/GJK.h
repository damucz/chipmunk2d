#ifndef __GJK_H
#define __GJK_H

#include "demo.h"

class GJK : public ChipmunkDemo
{
public:
    GJK();
    ~GJK() {};

    virtual cpSpace* Init();

protected:
    cpShape *shape1, *shape2;
};

#endif
