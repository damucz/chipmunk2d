#ifndef __SHATTER_H
#define __SHATTER_H

#include "demo.h"

class Shatter : public ChipmunkDemo
{
public:
    Shatter();
    ~Shatter() {};

    virtual cpSpace* Init();

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpVect HashVect(uint32_t x, uint32_t y, uint32_t seed);
    cpVect WorleyPoint(int i, int j, struct WorleyContex *context);
    int ClipCell(cpShape *shape, cpVect center, int i, int j, struct WorleyContex *context, cpVect *verts, cpVect *clipped, int count);
    void ShatterCell(cpSpace *space, cpShape *shape, cpVect cell, int cell_i, int cell_j, struct WorleyContex *context);
    void ShatterShape(cpSpace *space, cpShape *shape, cpFloat cellSize, cpVect focus);
};

#endif
