#include "PyramidStack.h"

PyramidStack::PyramidStack()
{
    name = "Pyramid Stack";
}

cpSpace *PyramidStack::Init()
{
    ChipmunkDemo::Init();

	space = cpSpaceNew();
	cpSpaceSetIterations(space, 30);
	cpSpaceSetGravity(space, cpv(0, -100));
	cpSpaceSetSleepTimeThreshold(space, 0.5f);
	cpSpaceSetCollisionSlop(space, 0.5f);
	
	cpBody *body, *staticBody = cpSpaceGetStaticBody(space);
	cpShape *shape;
	
	// Create segments around the edge of the screen.
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(-320,240), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(320,-240), cpv(320,240), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);
	
	// Add lots of boxes.
	for(int i=0; i<14; i++){
		for(int j=0; j<=i; j++){
			body = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForBox(1.0f, 30.0f, 30.0f)));
			cpBodySetPosition(body, cpv(j*32 - i*16, 300 - i*32));
			
			shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 30.0f, 30.0f, 0.5f));
			cpShapeSetElasticity(shape, 0.0f);
			cpShapeSetFriction(shape, 0.8f);
		}
	}
	
	// Add a ball to make things more interesting
	cpFloat radius = 15.0f;
	body = cpSpaceAddBody(space, cpBodyNew(10.0f, cpMomentForCircle(10.0f, 0.0f, radius, cpvzero)));
	cpBodySetPosition(body, cpv(0, -240 + radius+5));

	shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.9f);
	
	return space;
}
