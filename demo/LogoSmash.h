#ifndef __LOGOSMASH_H
#define __LOGOSMASH_H

#include "demo.h"

class LogoSmash : public ChipmunkDemo
{
public:
    LogoSmash();
    ~LogoSmash() {};

    virtual cpSpace* Init();
    //virtual void Draw();

protected:
    int GetPixel(int x, int y);

    int bodyCount;

    static void DrawDotCallback(cpBody *body, void *data);
    cpShape *MakeBall(cpFloat x, cpFloat y);
};

#endif
