#include "ContactPoints.h"

ContactPoints::ContactPoints()
{
    name = "Contact Points";
}

cpSpace *ContactPoints::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 5);
    cpSpaceSetDamping(space, 0.1f);

    cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(space);
    handler->beginFunc = NeverCollide;

    {
        cpFloat mass = 1.0f;
        cpFloat length = 100.0f;
        cpVect a = cpv(-length/2.0f, 0.0f), b = cpv(length/2.0f, 0.0f);

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForSegment(mass, a, b, 0.0)));
        cpBodySetPosition(body, cpv(-160.0f, -80.0f));

        cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, 30.0f));
    }

    {
        cpFloat mass = 1.0f;
        cpFloat length = 100.0f;
        cpVect a = cpv(-length/2.0f, 0.0f), b = cpv(length/2.0f, 0.0f);

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForSegment(mass, a, b, 0.0)));
        cpBodySetPosition(body, cpv(-160.0f, 80.0f));

        cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, 20.0f));
    }

    {
        cpFloat mass = 1.0f;
        const int NUM_VERTS = 5;

        cpVect verts[NUM_VERTS];
        for(int i=0; i<NUM_VERTS; i++){
            cpFloat angle = -2*M_PI*i/((cpFloat) NUM_VERTS);
            verts[i] = cpv(40*cos(angle), 40*sin(angle));
        }

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, NUM_VERTS, verts, cpvzero, 0.0)));
        cpBodySetPosition(body, cpv(-0.0f, -80.0f));

        cpSpaceAddShape(space, cpPolyShapeNew(body, NUM_VERTS, verts, cpTransformIdentity, 0.0));
    }

    {
        cpFloat mass = 1.0f;
        const int NUM_VERTS = 4;

        cpVect verts[NUM_VERTS];
        for(int i=0; i<NUM_VERTS; i++){
            cpFloat angle = -2*M_PI*i/((cpFloat) NUM_VERTS);
            verts[i] = cpv(60*cos(angle), 60*sin(angle));
        }

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, NUM_VERTS, verts, cpvzero, 0.0)));
        cpBodySetPosition(body, cpv(-0.0f, 80.0f));

        cpSpaceAddShape(space, cpPolyShapeNew(body, NUM_VERTS, verts, cpTransformIdentity, 0.0));
    }

    {
        cpFloat mass = 1.0f;
        cpFloat r = 60.0f;

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, INFINITY));
        cpBodySetPosition(body, cpv(160.0, -80.0f));

        cpSpaceAddShape(space, cpCircleShapeNew(body, r, cpvzero));
    }

    {
        cpFloat mass = 1.0f;
        cpFloat r = 40.0f;

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, INFINITY));
        cpBodySetPosition(body, cpv(160.0, 80.0f));

        cpSpaceAddShape(space, cpCircleShapeNew(body, r, cpvzero));
    }

	
	return space;
}

cpBool ContactPoints::NeverCollide(cpArbiter *arb, cpSpace *space, void *data){return cpFalse;}
