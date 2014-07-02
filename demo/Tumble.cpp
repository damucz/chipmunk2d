#include "Tumble.h"

Tumble::Tumble()
    : KinematicBoxBody(NULL)
{
    name = "Tumble";
}

cpSpace *Tumble::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetGravity(space, cpv(0, -600));

    cpShape *shape;

    // We create an infinite mass rogue body to attach the line segments too
    // This way we can control the rotation however we want.
    KinematicBoxBody = cpSpaceAddBody(space, cpBodyNewKinematic());
    cpBodySetAngularVelocity(KinematicBoxBody, 0.4f);

    // Set up the static box.
    cpVect a = cpv(-200, -200);
    cpVect b = cpv(-200,  200);
    cpVect c = cpv( 200,  200);
    cpVect d = cpv( 200, -200);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(KinematicBoxBody, a, b, 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(KinematicBoxBody, b, c, 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(KinematicBoxBody, c, d, 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(KinematicBoxBody, d, a, 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    cpFloat mass = 1;
    cpFloat width = 30;
    cpFloat height = width*2;

    // Add the bricks.
    for(int i=0; i<7; i++){
        for(int j=0; j<3; j++){
            cpVect pos = cpv(i*width - 150, j*height - 150);

            int type = (rand()%3000)/1000;
            if(type ==0){
                AddBox(space, pos, mass, width, height);
            } else if(type == 1){
                AddSegment(space, pos, mass, width, height);
            } else {
                AddCircle(space, cpvadd(pos, cpv(0.0, (height - width)/2.0)), mass, width/2.0);
                AddCircle(space, cpvadd(pos, cpv(0.0, (width - height)/2.0)), mass, width/2.0);
            }
        }
    }
	
	return space;
}

void Tumble::AddBox(cpSpace *space, cpVect pos, cpFloat mass, cpFloat width, cpFloat height)
{
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, width, height)));
	cpBodySetPosition(body, pos);
	
	cpShape *shape = cpSpaceAddShape(space, cpBoxShapeNew(body, width, height, 0.0));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.7f);
}

void Tumble::AddSegment(cpSpace *space, cpVect pos, cpFloat mass, cpFloat width, cpFloat height)
{
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, width, height)));
	cpBodySetPosition(body, pos);
	
	cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, cpv(0.0, (height - width)/2.0), cpv(0.0, (width - height)/2.0), width/2.0));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.7f);
}

void Tumble::AddCircle(cpSpace *space, cpVect pos, cpFloat mass, cpFloat radius)
{
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0, radius, cpvzero)));
	cpBodySetPosition(body, pos);
	
	cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.7f);
}
