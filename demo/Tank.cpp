#include "Tank.h"

Tank::Tank()
    : tankBody(NULL)
    , tankControlBody(NULL)
{
    name = "Tank";

    timestep = 1.0 / 60.0;
}

cpSpace *Tank::Init()
{
    ChipmunkDemo::Init();

    message = "Use the touch to drive the tank, it will follow the finger.";

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 10);
    cpSpaceSetSleepTimeThreshold(space, 0.5f);

    cpBody *staticBody = cpSpaceGetStaticBody(space);
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

    for(int i=0; i<50; i++){
        cpBody *body = AddBox(space, 20, 1);

        cpConstraint *pivot = cpSpaceAddConstraint(space, cpPivotJointNew2(staticBody, body, cpvzero, cpvzero));
        cpConstraintSetMaxBias(pivot, 0); // disable joint correction
        cpConstraintSetMaxForce(pivot, 1000.0f); // emulate linear friction

        cpConstraint *gear = cpSpaceAddConstraint(space, cpGearJointNew(staticBody, body, 0.0f, 1.0f));
        cpConstraintSetMaxBias(gear, 0); // disable joint correction
        cpConstraintSetMaxForce(gear, 5000.0f); // emulate angular friction
    }

    // We joint the tank to the control body and control the tank indirectly by modifying the control body.
    tankControlBody = cpSpaceAddBody(space, cpBodyNewKinematic());
    tankBody = AddBox(space, 30, 10);

    cpConstraint *pivot = cpSpaceAddConstraint(space, cpPivotJointNew2(tankControlBody, tankBody, cpvzero, cpvzero));
    cpConstraintSetMaxBias(pivot, 0); // disable joint correction
    cpConstraintSetMaxForce(pivot, 10000.0f); // emulate linear friction

    cpConstraint *gear = cpSpaceAddConstraint(space, cpGearJointNew(tankControlBody, tankBody, 0.0f, 1.0f));
    cpConstraintSetErrorBias(gear, 0); // attempt to fully correct the joint each step
    cpConstraintSetMaxBias(gear, 1.2f);  // but limit it's angular correction rate
    cpConstraintSetMaxForce(gear, 50000.0f); // emulate angular friction
	
    lastTouchPoint = tankBody->p;

	return space;
}

void Tank::Update(double dt)
{
	// turn the control body based on the angle relative to the actual body
	cpVect mouseDelta = cpvsub(lastTouchPoint, cpBodyGetPosition(tankBody));
	cpFloat turn = cpvtoangle(cpvunrotate(cpBodyGetRotation(tankBody), mouseDelta));
	cpBodySetAngle(tankControlBody, cpBodyGetAngle(tankBody) - turn);
	
	// drive the tank towards the mouse
	if(cpvnear(lastTouchPoint, cpBodyGetPosition(tankBody), 30.0)){
		cpBodySetVelocity(tankControlBody, cpvzero); // stop
	} else {
		cpFloat direction = (cpvdot(mouseDelta, cpBodyGetRotation(tankBody)) > 0.0 ? 1.0 : -1.0);
		cpBodySetVelocity(tankControlBody, cpvrotate(cpBodyGetRotation(tankBody), cpv(30.0f*direction, 0.0f)));
	}
	
	ChipmunkDemo::Update(dt);
}

bool Tank::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
{
    if (ChipmunkDemo::ProcessTouch(id, pos, state, rightClick))
        return true;

    if (!rightClick)
    {
        if (state == ChipmunkDemo::TOUCH_START)
        {
            if (touchId < 0)
            {
                touchId = id;
                lastTouchPoint = pos;
                return true;
            }
        }
        else if (state == ChipmunkDemo::TOUCH_MOVE)
        {
            if (touchId == id)
            {
                lastTouchPoint = pos;
                return true;
            }
        }
        else /* if (state == ChipmunkDemo::TOUCH_END) */
        {
            if (touchId == id)
            {
                touchId = -1;
                lastTouchPoint = pos;
                return true;
            }
        }
    }

    return false;
}

cpBody *Tank::AddBox(cpSpace *space, cpFloat size, cpFloat mass)
{
	cpFloat radius = cpvlength(cpv(size, size));

	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, size, size)));
	cpBodySetPosition(body, cpv(frand()*(640 - 2*radius) - (320 - radius), frand()*(480 - 2*radius) - (240 - radius)));
	
	cpShape *shape = cpSpaceAddShape(space, cpBoxShapeNew(body, size, size, 0.0));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.7f);
	
	return body;
}
