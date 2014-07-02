#include "Crane.h"

Crane::Crane()
    : dollyBody(NULL)
    , dollyServo(NULL)
    , winchServo(NULL)
    , hookJoint(NULL)
    , crate(NULL)
{
    name = "Crane";

    timestep = 1.0 / 60.0;

    lastTouchPoint.x = 0;
    lastTouchPoint.y = 0;
}

cpSpace *Crane::Init()
{
    ChipmunkDemo::Init();

    message = "Control the crane by touching the empty space.\nHold right bottom corner and tap anywhere to release the cargo.";

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -100));
    cpSpaceSetDamping(space, 0.8);

    cpBody *staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape;

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    // Add a body for the dolly.
    dollyBody = cpSpaceAddBody(space, cpBodyNew(10, INFINITY));
    cpBodySetPosition(dollyBody, cpv(0, 100));

    // Add a block so you can see it.
    cpSpaceAddShape(space, cpBoxShapeNew(dollyBody, 30, 30, 0.0));

    // Add a groove joint for it to move back and forth on.
    cpSpaceAddConstraint(space, cpGrooveJointNew(staticBody, dollyBody, cpv(-250, 100), cpv(250, 100), cpvzero));

    // Add a pivot joint to act as a servo motor controlling it's position
    // By updating the anchor points of the pivot joint, you can move the dolly.
    dollyServo = cpSpaceAddConstraint(space, cpPivotJointNew(staticBody, dollyBody, cpBodyGetPosition(dollyBody)));
    // Max force the dolly servo can generate.
    cpConstraintSetMaxForce(dollyServo, 10000);
    // Max speed of the dolly servo
    cpConstraintSetMaxBias(dollyServo, 100);
    // You can also change the error bias to control how it slows down.
    //cpConstraintSetErrorBias(dollyServo, 0.2);


    // Add the crane hook.
    cpBody *hookBody = cpSpaceAddBody(space, cpBodyNew(1, INFINITY));
    cpBodySetPosition(hookBody, cpv(0, 50));

    // Add a sensor shape for it. This will be used to figure out when the hook touches a box.
    shape = cpSpaceAddShape(space, cpCircleShapeNew(hookBody, 10, cpvzero));
    cpShapeSetSensor(shape, cpTrue);
    cpShapeSetCollisionType(shape, HOOK_SENSOR);

    // Add a slide joint to act as a winch motor
    // By updating the max length of the joint you can make it pull up the load.
    winchServo = cpSpaceAddConstraint(space, cpSlideJointNew(dollyBody, hookBody, cpvzero, cpvzero, 0, INFINITY));
    // Max force the dolly servo can generate.
    cpConstraintSetMaxForce(winchServo, 30000);
    // Max speed of the dolly servo
    cpConstraintSetMaxBias(winchServo, 60);

    // TODO: cleanup
    // Finally a box to play with
    cpBody *boxBody = cpSpaceAddBody(space, cpBodyNew(30, cpMomentForBox(30, 50, 50)));
    cpBodySetPosition(boxBody, cpv(200, -200));

    // Add a block so you can see it.
    shape = cpSpaceAddShape(space, cpBoxShapeNew(boxBody, 50, 50, 0.0));
    cpShapeSetFriction(shape, 0.7);
    cpShapeSetCollisionType(shape, CRATE);

    cpCollisionHandler *handler = cpSpaceAddCollisionHandler(space, HOOK_SENSOR, CRATE);
    handler->beginFunc = (cpCollisionBeginFunc)HookCrate;
    handler->userData = this;

    lastTouchPoint = hookBody->p;
	
	return space;
}

void Crane::Update(double dt)
{
    // Set the first anchor point (the one attached to the static body) of the dolly servo to the mouse's x position.
    cpPivotJointSetAnchorA(dollyServo, cpv(lastTouchPoint.x, 100));

    // Set the max length of the winch servo to match the mouse's height.
	cpSlideJointSetMax(winchServo, cpfmax(100 - lastTouchPoint.y, 50));
	
	ChipmunkDemo::Update(dt);
}

bool Crane::ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/)
{
    if (ChipmunkDemo::ProcessTouch(id, pos, state, rightClick))
        return true;

    if (rightClick)
    {
        if (state == TOUCH_START && hookJoint)
        {
            cpSpaceRemoveConstraint(space, hookJoint);
            cpConstraintFree(hookJoint);
            hookJoint = NULL;
        }
        return true;
    }
    else
    {
        lastTouchPoint = pos;
        return true;
    }

    return false;
}

void Crane::AttachHook(cpSpace *space, cpBody *hook, Crane *data)
{
	data->hookJoint = cpSpaceAddConstraint(space, cpPivotJointNew(hook, data->crate, cpBodyGetPosition(hook)));
}


cpBool Crane::HookCrate(cpArbiter *arb, cpSpace *space, Crane *data)
{
	if(data->hookJoint == NULL){
		// Get pointers to the two bodies in the collision pair and define local variables for them.
		// Their order matches the order of the collision types passed
		// to the collision handler this function was defined for
		CP_ARBITER_GET_BODIES(arb, hook, crate);

        data->crate = crate;

		// additions and removals can't be done in a normal callback.
		// Schedule a post step callback to do it.
		// Use the hook as the key and pass along the arbiter.
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)AttachHook, hook, data);
	}
	
	return cpTrue; // return value is ignored for sensor callbacks anyway
}
