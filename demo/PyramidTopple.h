#ifndef __PYRAMIDTOPPLE_H
#define __PYRAMIDTOPPLE_H

#include "demo.h"

class PyramidTopple : public ChipmunkDemo
{
public:
    PyramidTopple();
    ~PyramidTopple() {};

    virtual cpSpace* Init();

protected:
    static const float width;
    static const float height;

    void AddDomino(cpSpace *space, cpVect pos, cpBool flipped);
};

#endif
