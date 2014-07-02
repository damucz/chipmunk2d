#ifndef __BENCH_H
#define __BENCH_H

#include "demo.h"

class Bench : public ChipmunkDemo
{
public:
    Bench();
    ~Bench() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);
    virtual void Destroy();

protected:
    static void AddCircle(cpSpace *space, int index, cpFloat radius);
    static void AddBox(cpSpace *space, int index, cpFloat size);
    static void AddHexagon(cpSpace *space, int index, cpFloat radius);

    static cpSpace *SetupSpace_SimpleTerrain();
    static cpSpace *Init_SimpleTerrainCircles_1000();
    static cpSpace *Init_SimpleTerrainCircles_500();
    static cpSpace *Init_SimpleTerrainCircles_100();
    static cpSpace *Init_SimpleTerrainBoxes_1000();
    static cpSpace *Init_SimpleTerrainBoxes_500();
    static cpSpace *Init_SimpleTerrainBoxes_100();
    static cpSpace *Init_SimpleTerrainHexagons_1000();
    static cpSpace *Init_SimpleTerrainHexagons_500();
    static cpSpace *Init_SimpleTerrainHexagons_100();

    static cpFloat RandSize();

    static cpSpace *Init_SimpleTerrainVCircles_200();
    static cpSpace *Init_SimpleTerrainVBoxes_200();
    static cpSpace *Init_SimpleTerrainVHexagons_200();

    static cpSpace *Init_ComplexTerrainCircles_1000();
    static cpSpace *Init_ComplexTerrainHexagons_1000();

    static cpSpace *Init_BouncyTerrainCircles_500();
    static cpSpace *Init_BouncyTerrainHexagons_500();

    static cpBool NoCollide_begin(cpArbiter *arb, cpSpace *space, void *data);
    static cpSpace *Init_NoCollide();
};

#endif
