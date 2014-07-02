#include "PyramidTopple.h"

const float PyramidTopple::width = 4.0f;
const float PyramidTopple::height = 30.0f;

PyramidTopple::PyramidTopple()
{
    name = "Pyramid Topple";
}

cpSpace *PyramidTopple::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -300));
    cpSpaceSetSleepTimeThreshold(space, 0.5f);
    cpSpaceSetCollisionSlop(space, 0.5f);

    // Add a floor.
    cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(-600,-240), cpv(600,-240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);


    // Add the dominoes.
    int n = 12;
    for(int i=0; i<n; i++){
        for(int j=0; j<(n - i); j++){
            cpVect offset = cpv((j - (n - 1 - i)*0.5f)*1.5f*height, (i + 0.5f)*(height + 2*width) - width - 240);
            AddDomino(space, offset, cpFalse);
            AddDomino(space, cpvadd(offset, cpv(0, (height + width)/2.0f)), cpTrue);

            if(j == 0){
                AddDomino(space, cpvadd(offset, cpv(0.5f*(width - height), height + width)), cpFalse);
            }

            if(j != n - i - 1){
                AddDomino(space, cpvadd(offset, cpv(height*0.75f, (height + 3*width)/2.0f)), cpTrue);
            } else {
                AddDomino(space, cpvadd(offset, cpv(0.5f*(height - width), height + width)), cpFalse);
            }
        }
    }

	return space;
}

void PyramidTopple::AddDomino( cpSpace *space, cpVect pos, cpBool flipped )
{
    cpFloat mass = 1.0f;
    cpFloat radius = 0.5f;
    cpFloat moment = cpMomentForBox(mass, width, height);

    cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(body, pos);

    cpShape *shape = (flipped ? cpBoxShapeNew(body, height, width, 0.0) : cpBoxShapeNew(body, width - radius*2.0f, height, radius));
    cpSpaceAddShape(space, shape);
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.6f);
}
