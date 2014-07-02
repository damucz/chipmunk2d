#include "Chains.h"

#define CHAIN_COUNT 8
#define LINK_COUNT 10

Chains::Chains()
{
    name = "Breakable Chains";
}

cpSpace *Chains::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -100));
    cpSpaceSetSleepTimeThreshold(space, 0.5f);

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

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,240), cpv(320,240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    cpFloat mass = 1;
    cpFloat width = 20;
    cpFloat height = 30;

    cpFloat spacing = width*0.3;

    // Add lots of boxes.
    for(int i=0; i<CHAIN_COUNT; i++){
        cpBody *prev = NULL;

        for(int j=0; j<LINK_COUNT; j++){
            cpVect pos = cpv(40*(i - (CHAIN_COUNT - 1)/2.0), 240 - (j + 0.5)*height - (j + 1)*spacing);

            body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, width, height)));
            cpBodySetPosition(body, pos);

            shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, cpv(0, (height - width)/2.0), cpv(0, (width - height)/2.0), width/2.0));
            cpShapeSetFriction(shape, 0.8f);

            cpFloat breakingForce = 80000;

            cpConstraint *constraint = NULL;
            if(prev == NULL){
                constraint = cpSpaceAddConstraint(space, cpSlideJointNew(body, staticBody, cpv(0, height/2), cpv(pos.x, 240), 0, spacing));
            } else {
                constraint = cpSpaceAddConstraint(space, cpSlideJointNew(body, prev, cpv(0, height/2), cpv(0, -height/2), 0, spacing));
            }

            cpConstraintSetMaxForce(constraint, breakingForce);
            cpConstraintSetPostSolveFunc(constraint, BreakableJointPostSolve);
            cpConstraintSetCollideBodies(constraint, cpFalse);

            prev = body;
        }
    }

    cpFloat radius = 15.0f;
    body = cpSpaceAddBody(space, cpBodyNew(10.0f, cpMomentForCircle(10.0f, 0.0f, radius, cpvzero)));
    cpBodySetPosition(body, cpv(0, -240 + radius+5));
    cpBodySetVelocity(body, cpv(0, 300));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.9f);
	
	return space;
}

void Chains::BreakablejointPostStepRemove(cpSpace *space, cpConstraint *joint, void *unused)
{
	cpSpaceRemoveConstraint(space, joint);
	cpConstraintFree(joint);
}

void Chains::BreakableJointPostSolve(cpConstraint *joint, cpSpace *space)
{
	cpFloat dt = cpSpaceGetCurrentTimeStep(space);
	
	// Convert the impulse to a force by dividing it by the timestep.
	cpFloat force = cpConstraintGetImpulse(joint)/dt;
	cpFloat maxForce = cpConstraintGetMaxForce(joint);

	// If the force is almost as big as the joint's max force, break it.
	if(force > 0.9*maxForce){
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)BreakablejointPostStepRemove, joint, NULL);
	}
}
