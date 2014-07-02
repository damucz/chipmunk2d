#include "GJK.h"

GJK::GJK()
    : shape1(NULL)
    , shape2(NULL)
{
    name = "GJK";

    timestep = 1.0 / 60.0;
}

cpSpace *GJK::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 5);
    space->damping = 0.1;

    cpFloat mass = 1.0f;

    {
        cpFloat size = 100.0;

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, size, size)));
        cpBodySetPosition(body, cpv(100.0, 50.0f));

        shape1 = cpSpaceAddShape(space, cpBoxShapeNew(body, size, size, 0.0));
        //shape1->group = 1;
    }{
        cpFloat size = 100.0;

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, size, size)));
        cpBodySetPosition(body, cpv(120.0, -40.0f));
        cpBodySetAngle(body, 1e-2);

        shape2 = cpSpaceAddShape(space, cpBoxShapeNew(body, size, size, 0.0));
        //shape2->group = 1;
    }

    //	{
    //		cpFloat size = 100.0;
    //		const int NUM_VERTS = 5;
    //		
    //		cpVect verts[NUM_VERTS];
    //		for(int i=0; i<NUM_VERTS; i++){
    //			cpFloat angle = -2*M_PI*i/((cpFloat) NUM_VERTS);
    //			verts[i] = cpv(size/2.0*cos(angle), size/2.0*sin(angle));
    //		}
    //		
    //		cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, NUM_VERTS, verts, cpvzero)));
    //		cpBodySetPosition(body, cpv(100.0, 50.0f));
    //		
    //		shape1 = cpSpaceAddShape(space, cpPolyShapeNew(body, NUM_VERTS, verts, cpvzero));
    //		shape1->group = 1;
    //	}
    //	{
    //		cpFloat size = 100.0;
    //		const int NUM_VERTS = 4;
    //		
    //		cpVect verts[NUM_VERTS];
    //		for(int i=0; i<NUM_VERTS; i++){
    //			cpFloat angle = -2*M_PI*i/((cpFloat) NUM_VERTS);
    //			verts[i] = cpv(size/2.0*cos(angle), size/2.0*sin(angle));
    //		}
    //		
    //		cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, NUM_VERTS, verts, cpvzero)));
    //		cpBodySetPosition(body, cpv(100.0, -50.0f));
    //		
    //		shape2 = cpSpaceAddShape(space, cpPolyShapeNew(body, NUM_VERTS, verts, cpvzero));
    //		shape2->group = 1;
    //	}
    //	
    //	{
    //		cpFloat size = 150.0;
    //		cpFloat radius = 25.0;
    //		
    //		cpVect a = cpv( size/2.0, 0.0);
    //		cpVect b = cpv(-size/2.0, 0.0);
    //		cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForSegment(mass, a, b)));
    //		cpBodySetPosition(body, cpv(0, 25));
    //		
    //		shape1 = cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, radius));
    //		shape1->group = 1;
    //	}
    //	{
    //		cpFloat radius = 50.0;
    //		
    //		cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, radius, cpvzero)));
    //		cpBodySetPosition(body, cpv(0, -25));
    //		
    //		shape2 = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
    //		shape2->group = 1;
    //	}

	return space;
}
