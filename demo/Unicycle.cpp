#include "Unicycle.h"

Unicycle::Unicycle()
    : balance_body(NULL)
    , balance_sin(0.0)
    //, last_v(0.0)
    , wheel_body(NULL)
    , motor(NULL)
    , touchId(-1)
{
    name = "Unicycle";

    timestep = 1.0 / 60.0;

    lastTouchPoint.x = 0;
    lastTouchPoint.y = 0;
}

cpSpace *Unicycle::Init()
{
    ChipmunkDemo::Init();

    message = "This unicycle is completely driven and balanced by a single cpSimpleMotor.\nTouch the empty space to make the unicycle follow it.";

    space = cpSpaceNew();
    space->userData = this;
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -500));

    {
        cpShape *shape = NULL;
        cpBody *staticBody = cpSpaceGetStaticBody(space);

        shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-3200,-240), cpv(3200,-240), 0.0f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction(shape, 1.0f);
        cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

        shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(0,-200), cpv(240,-240), 0.0f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction(shape, 1.0f);
        cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

        shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-240,-240), cpv(0,-200), 0.0f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction(shape, 1.0f);
        cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);
    }


    {
        cpFloat radius = 20.0;
        cpFloat mass = 1.0;

        cpFloat moment = cpMomentForCircle(mass, 0.0, radius, cpvzero);
        wheel_body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
        cpBodySetPosition(wheel_body, cpv(0.0, -160.0 + radius));

        cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(wheel_body, radius, cpvzero));
        cpShapeSetFriction(shape, 0.7);
        cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
    }

    {
        cpFloat cog_offset = 30.0;

        cpBB bb1 = cpBBNew(-5.0, 0.0 - cog_offset, 5.0, cog_offset*1.2 - cog_offset);
        cpBB bb2 = cpBBNew(-25.0, bb1.t, 25.0, bb1.t + 10.0);

        cpFloat mass = 3.0;
        cpFloat moment = cpMomentForBox2(mass, bb1) + cpMomentForBox2(mass, bb2);

        balance_body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
        cpBodySetPosition(balance_body, cpv(0.0, cpBodyGetPosition(wheel_body).y + cog_offset));

        cpShape *shape = NULL;

        shape = cpSpaceAddShape(space, cpBoxShapeNew2(balance_body, bb1, 0.0));
        cpShapeSetFriction(shape, 1.0);
        cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

        shape = cpSpaceAddShape(space, cpBoxShapeNew2(balance_body, bb2, 0.0));
        cpShapeSetFriction(shape, 1.0);
        cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
    }

    cpVect anchorA = cpBodyWorldToLocal(balance_body, cpBodyGetPosition(wheel_body));
    cpVect groove_a = cpvadd(anchorA, cpv(0.0,  30.0));
    cpVect groove_b = cpvadd(anchorA, cpv(0.0, -10.0));
    cpSpaceAddConstraint(space, cpGrooveJointNew(balance_body, wheel_body, groove_a, groove_b, cpvzero));
    cpSpaceAddConstraint(space, cpDampedSpringNew(balance_body, wheel_body, anchorA, cpvzero, 0.0, 6.0e2, 30.0));

    motor = cpSpaceAddConstraint(space, cpSimpleMotorNew(wheel_body, balance_body, 0.0));
    cpConstraintSetPreSolveFunc(motor, MotorPreSolve);

    {
        cpFloat width = 100.0;
        cpFloat height = 20.0;
        cpFloat mass = 3.0;

        cpBody *boxBody = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, width, height)));
        cpBodySetPosition(boxBody, cpv(200, -100));

        cpShape *shape = cpSpaceAddShape(space, cpBoxShapeNew(boxBody, width, height, 0.0));
        cpShapeSetFriction(shape, 0.7);
    }
	
	return space;
}

cpFloat Unicycle::BiasCoef(cpFloat errorBias, cpFloat dt)
{
	return 1.0f - cpfpow(errorBias, dt);
}

void Unicycle::MotorPreSolve(cpConstraint *motor, cpSpace *space)
{
    Unicycle* th = static_cast<Unicycle*>(space->userData);
    cpDataPointer data = th;

	cpFloat dt = cpSpaceGetCurrentTimeStep(space);
	
	cpFloat target_x = th->lastTouchPoint.x;
	th->DrawSegment(cpv(target_x, -1000.0), cpv(target_x, 1000.0), RGBAColor(1.0, 0.0, 0.0, 1.0), data);
	
	cpFloat max_v = 500.0;
	cpFloat target_v = cpfclamp(bias_coef(0.5, dt/1.2)*(target_x - cpBodyGetPosition(th->balance_body).x)/dt, -max_v, max_v);
	cpFloat error_v = (target_v - cpBodyGetVelocity(th->balance_body).x);
	cpFloat target_sin = 3.0e-3*bias_coef(0.1, dt)*error_v/dt;
	
	cpFloat max_sin = cpfsin(0.6);
	th->balance_sin = cpfclamp(th->balance_sin - 6.0e-5*bias_coef(0.2, dt)*error_v/dt, -max_sin, max_sin);
	cpFloat target_a = asin(cpfclamp(-target_sin + th->balance_sin, -max_sin, max_sin));
	cpFloat angular_diff = asin(cpvcross(cpBodyGetRotation(th->balance_body), cpvforangle(target_a)));
	cpFloat target_w = bias_coef(0.1, dt/0.4)*(angular_diff)/dt;
	
	cpFloat max_rate = 50.0;
	cpFloat rate = cpfclamp(cpBodyGetAngularVelocity(th->wheel_body) + cpBodyGetAngularVelocity(th->balance_body) - target_w, -max_rate, max_rate);
	cpSimpleMotorSetRate(motor, cpfclamp(rate, -max_rate, max_rate));
	cpConstraintSetMaxForce(motor, 8.0e4);
}

bool Unicycle::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
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
            if (touchId == (int)id)
            {
                lastTouchPoint = pos;
                return true;
            }
        }
        else /* if (state == ChipmunkDemo::TOUCH_END) */
        {
            if (touchId == (int)id)
            {
                touchId = -1;
                lastTouchPoint = pos;
                return true;
            }
        }
    }

    return false;
}
