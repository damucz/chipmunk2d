#include "Smooth.h"

Smooth::Smooth()
{
    name = "Smooth";

    timestep = 1.0 / 60.0;
}

cpSpace *Smooth::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 5);
    cpSpaceSetDamping(space, 0.1f);

    cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(space);
    handler->preSolveFunc = DrawContacts;
    handler->userData = this;

    {
        cpFloat mass = 1.0f;
        cpFloat length = 100.0f;
        cpVect a = cpv(-length/2.0f, 0.0f), b = cpv(length/2.0f, 0.0f);
        cpFloat radius = 10.0f;

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForSegment(mass, a, b, radius)));
        cpBodySetPosition(body, cpv(-160.0f, 80.0f));

        cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, 30.0f));
    }

    {
        cpFloat mass = 1.0f;
        const int NUM_VERTS = 5;
        cpFloat radius = 10.0f;

        cpVect verts[NUM_VERTS];
        for(int i=0; i<NUM_VERTS; i++){
            cpFloat angle = -2*M_PI*i/((cpFloat) NUM_VERTS);
            verts[i] = cpv(40*cos(angle), 40*sin(angle));
        }

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, NUM_VERTS, verts, cpvzero, radius)));
        cpBodySetPosition(body, cpv(-0.0f, 80.0f));

        cpSpaceAddShape(space, cpPolyShapeNew(body, NUM_VERTS, verts, cpTransformIdentity, radius));
    }

    {
        cpFloat mass = 1.0f;
        cpFloat r = 60.0f;

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, INFINITY));
        cpBodySetPosition(body, cpv(160.0, 80.0f));

        cpSpaceAddShape(space, cpCircleShapeNew(body, r, cpvzero));
    }

    cpBody *staticBody = cpSpaceGetStaticBody(space);

    cpVect terrain[] = {
        cpv(-320, -200),
        cpv(-200, -100),
        cpv(   0, -200),
        cpv( 200, -100),
        cpv( 320, -200),
    };
    int terrainCount = sizeof(terrain)/sizeof(*terrain);

    for(int i=1; i<5; i++){
        cpVect v0 = terrain[std::max(i-2, 0)];
        cpVect v1 = terrain[i-1];
        cpVect v2 = terrain[i];
        cpVect v3 = terrain[std::min(i+1, terrainCount - 1)];

        cpShape *seg = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, v1, v2, 10.0));
        cpSegmentShapeSetNeighbors(seg, v0, v3);
    }
	
	return space;
}

cpBool Smooth::DrawContacts(cpArbiter *arb, cpSpace *space, void *data){
    Smooth* th = static_cast<Smooth*>(data);

	cpContactPointSet set = cpArbiterGetContactPointSet(arb);
	
	for(int i=0; i<set.count; i++){
        cpDataPointer data = th;

		cpVect pA = set.points[i].pointA;
        cpVect pB = set.points[i].pointB;
		th->DrawDot(6.0, pA, RGBAColor(1, 0, 0, 1), &data);
		//th->DrawSegment(p, cpvadd(p, cpvmult(set.points[i].normal, 10.0)), RGBAColor(1, 0, 0, 1));
        th->DrawSegment(pA, pB, RGBAColor(1, 0, 0, 1), &data);
	}
	
	return cpFalse;
//	return cpTrue;
}
