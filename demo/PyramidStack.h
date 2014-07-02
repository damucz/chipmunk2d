#ifndef __PYRAMIDSTACK_H
#define __PYRAMIDSTACK_H

#include "demo.h"

class PyramidStack : public ChipmunkDemo
{
public:
    PyramidStack();
    ~PyramidStack() {};

    virtual cpSpace* Init();
};

#endif
