#include "TheoJansen.h"

const cpFloat TheoJansen::seg_radius = 3.0f;

TheoJansen::TheoJansen()
    : motor(NULL)
    , chassis(NULL)
    , touchId(-1)
{
    name = "Theo Jansen Machine";
}

cpSpace *TheoJansen::Init()
{
    ChipmunkDemo::Init();

    message = "Touch to empty space to control the robot.";

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 20);
    cpSpaceSetGravity(space, cpv(0,-500));

    cpBody *staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape;
    cpVect a, b;

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

    cpFloat offset = 30.0f;

    // make chassis
    cpFloat chassis_mass = 2.0f;
    a = cpv(-offset, 0.0f), b = cpv(offset, 0.0f);
    chassis = cpSpaceAddBody(space, cpBodyNew(chassis_mass, cpMomentForSegment(chassis_mass, a, b, 0.0f)));

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(chassis, a, b, seg_radius));
    cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

    // make crank
    cpFloat crank_mass = 1.0f;
    cpFloat crank_radius = 13.0f;
    cpBody *crank = cpSpaceAddBody(space, cpBodyNew(crank_mass, cpMomentForCircle(crank_mass, crank_radius, 0.0f, cpvzero)));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(crank, crank_radius, cpvzero));
    cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

    cpSpaceAddConstraint(space, cpPivotJointNew2(chassis, crank, cpvzero, cpvzero));

    cpFloat side = 30.0f;

    int num_legs = 2;
    for(int i=0; i<num_legs; i++){
        MakeLeg(space, side,  offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+0)/(cpFloat)num_legs*M_PI), crank_radius));
        MakeLeg(space, side, -offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+1)/(cpFloat)num_legs*M_PI), crank_radius));
    }

    motor = cpSpaceAddConstraint(space, cpSimpleMotorNew(chassis, crank, 6.0f));

    lastTouchPoint = chassis->p;

	return space;
}

void TheoJansen::Update(double dt)
{
    cpFloat rate;
    if (touchId < 0)
        rate = 0;
    else
	    rate = (lastTouchPoint.x - chassis->p.x) / 20.0f;
	cpSimpleMotorSetRate(motor, rate);
	cpConstraintSetMaxForce(motor, (rate) ? 100000.0f : 0.0f);
	
	cpSpaceStep(space, dt);
}

void TheoJansen::MakeLeg(cpSpace *space, cpFloat side, cpFloat offset, cpBody *chassis, cpBody *crank, cpVect anchor)
{
	cpVect a, b;
	cpShape *shape;
	
	cpFloat leg_mass = 1.0f;

	// make leg
	a = cpvzero, b = cpv(0.0f, side);
	cpBody *upper_leg = cpSpaceAddBody(space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b, 0.0f)));
	cpBodySetPosition(upper_leg, cpv(offset, 0.0f));
	
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(upper_leg, a, b, seg_radius));
	cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
	
	cpSpaceAddConstraint(space, cpPivotJointNew2(chassis, upper_leg, cpv(offset, 0.0f), cpvzero));
	
	// lower leg
	a = cpvzero, b = cpv(0.0f, -1.0f*side);
	cpBody *lower_leg = cpSpaceAddBody(space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b, 0.0f)));
	cpBodySetPosition(lower_leg, cpv(offset, -side));
	
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(lower_leg, a, b, seg_radius));
	cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
	
	shape = cpSpaceAddShape(space, cpCircleShapeNew(lower_leg, seg_radius*2.0f, b));
	cpShapeSetFilter(shape, cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 1.0f);
	
	cpSpaceAddConstraint(space, cpPinJointNew(chassis, lower_leg, cpv(offset, 0.0f), cpvzero));
	
	cpSpaceAddConstraint(space, cpGearJointNew(upper_leg, lower_leg, 0.0f, 1.0f));
	
	cpConstraint *constraint;
	cpFloat diag = cpfsqrt(side*side + offset*offset);
	
	constraint = cpSpaceAddConstraint(space, cpPinJointNew(crank, upper_leg, anchor, cpv(0.0f, side)));
	cpPinJointSetDist(constraint, diag);
	
	constraint = cpSpaceAddConstraint(space, cpPinJointNew(crank, lower_leg, anchor, cpvzero));
	cpPinJointSetDist(constraint, diag);
}

bool TheoJansen::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
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
