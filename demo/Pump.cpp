#include "Pump.h"

Pump::Pump()
    : motor(NULL)
    , touchId(-1)
{
    name = "Pump";

    timestep = 1.0 / 120.0;

    for (int i = 0; i < numBalls; ++i)
        balls[i] = NULL;
}

cpSpace *Pump::Init()
{
    ChipmunkDemo::Init();

    message = "Use touch and drag to apply force (negative or positive by x and y axis).\nStop touching to stop the machine.";

    space = cpSpaceNew();
    cpSpaceSetGravity(space, cpv(0, -600));

    cpBody *staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape;

    // beveling all of the line segments slightly helps prevent things from getting stuck on cracks
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-256,16), cpv(-256,300), 2.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-256,16), cpv(-192,0), 2.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-192,0), cpv(-192, -64), 2.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-128,-64), cpv(-128,144), 2.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-192,80), cpv(-192,176), 2.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-192,176), cpv(-128,240), 2.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-128,144), cpv(192,64), 2.0f));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    cpVect verts[] = {
        cpv(-30,-80),
        cpv(-30, 80),
        cpv( 30, 64),
        cpv( 30,-80),
    };

    cpBody *plunger = cpSpaceAddBody(space, cpBodyNew(1.0f, INFINITY));
    cpBodySetPosition(plunger, cpv(-160,-80));

    shape = cpSpaceAddShape(space, cpPolyShapeNew(plunger, 4, verts, cpTransformIdentity, 0.0));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 0.5f);
    cpShapeSetFilter(shape, cpShapeFilterNew(CP_NO_GROUP, 1, 1));

    // add balls to hopper
    for(int i=0; i<numBalls; i++)
        balls[i] = AddBall(space, cpv(-224 + i,80 + 64*i));

    // add small gear
    cpBody *smallGear = cpSpaceAddBody(space, cpBodyNew(10.0f, cpMomentForCircle(10.0f, 80, 0, cpvzero)));
    cpBodySetPosition(smallGear, cpv(-160,-160));
    cpBodySetAngle(smallGear, -M_PI_2);

    shape = cpSpaceAddShape(space, cpCircleShapeNew(smallGear, 80.0f, cpvzero));
    cpShapeSetFilter(shape, CP_SHAPE_FILTER_NONE);

    cpSpaceAddConstraint(space, cpPivotJointNew2(staticBody, smallGear, cpv(-160,-160), cpvzero));

    // add big gear
    cpBody *bigGear = cpSpaceAddBody(space, cpBodyNew(40.0f, cpMomentForCircle(40.0f, 160, 0, cpvzero)));
    cpBodySetPosition(bigGear, cpv(80,-160));
    cpBodySetAngle(bigGear, M_PI_2);

    shape = cpSpaceAddShape(space, cpCircleShapeNew(bigGear, 160.0f, cpvzero));
    cpShapeSetFilter(shape, CP_SHAPE_FILTER_NONE);

    cpSpaceAddConstraint(space, cpPivotJointNew2(staticBody, bigGear, cpv(80,-160), cpvzero));

    // connect the plunger to the small gear.
    cpSpaceAddConstraint(space, cpPinJointNew(smallGear, plunger, cpv(80,0), cpv(0,0)));
    // connect the gears.
    cpSpaceAddConstraint(space, cpGearJointNew(smallGear, bigGear, -M_PI_2, -2.0f));


    // feeder mechanism
    cpFloat bottom = -300.0f;
    cpFloat top = 32.0f;
    cpBody *feeder = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForSegment(1.0f, cpv(-224.0f, bottom), cpv(-224.0f, top), 0.0f)));
    cpBodySetPosition(feeder, cpv(-224, (bottom + top)/2.0f));

    cpFloat len = top - bottom;
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(feeder, cpv(0.0f, len/2.0f), cpv(0.0f, -len/2.0f), 20.0f));
    cpShapeSetFilter(shape, GRAB_FILTER);

    cpSpaceAddConstraint(space, cpPivotJointNew2(staticBody, feeder, cpv(-224.0f, bottom), cpv(0.0f, -len/2.0f)));
    cpVect anchr = cpBodyWorldToLocal(feeder, cpv(-224.0f, -160.0f));
    cpSpaceAddConstraint(space, cpPinJointNew(feeder, smallGear, anchr, cpv(0.0f, 80.0f)));

    // motorize the second gear
    motor = cpSpaceAddConstraint(space, cpSimpleMotorNew(staticBody, bigGear, 3.0f));
	
	return space;
}

void Pump::Update(double dt)
{
    ChipmunkDemo::Update(dt);
	
	for(int i=0; i<numBalls; i++){
		cpBody *ball = balls[i];
		cpVect pos = cpBodyGetPosition(ball);
		
		if(pos.x > 320.0f){
			cpBodySetVelocity(ball, cpvzero);
			cpBodySetPosition(ball, cpv(-224.0f, 200.0f));
		}
	}
}

cpBody *Pump::AddBall(cpSpace *space, cpVect pos)
{
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForCircle(1.0f, 30, 0, cpvzero)));
	cpBodySetPosition(body, pos);
	
	cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(body, 30, cpvzero));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.5f);
	
	return body;
}

bool Pump::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
{
    if (ChipmunkDemo::ProcessTouch(id, pos, state, rightClick))
        return true;

    if (!rightClick)
    {
        if (state == TOUCH_START)
        {
            if (touchId < 0)
            {
                startTouchPoint = pos;
                touchId = id;
                return true;
            }
        }
        else if (state == TOUCH_MOVE)
        {
            if (touchId == id)
            {
                cpFloat coef = (2.0f + (pos.y - startTouchPoint.y))/3.0f;
                cpFloat rate = (pos.x - startTouchPoint.x)*30.0f*coef;

                cpSimpleMotorSetRate(motor, rate);
                cpConstraintSetMaxForce(motor, rate ? 1000000.0f : 0.0f);

                return true;
            }
        }
        else /* if (state == TOUCH_END) */
        {
            if (touchId == id)
            {
                touchId = -1;
                cpSimpleMotorSetRate(motor, 0);
                cpConstraintSetMaxForce(motor, 0);

                return true;
            }
        }
    }

    return false;
}
