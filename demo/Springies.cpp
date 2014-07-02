#include "Springies.h"

Springies::Springies()
{
    name = "Springies";

    timestep = 1.0 / 60.0;
}

cpSpace *Springies::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpBody *staticBody = cpSpaceGetStaticBody(space);

    cpBody *body1  = AddBar(space, cpv(-240,  160), cpv(-160,   80), 1);
    cpBody *body2  = AddBar(space, cpv(-160,   80), cpv( -80,  160), 1);
    cpBody *body3  = AddBar(space, cpv(   0,  160), cpv(  80,    0), 0);
    cpBody *body4  = AddBar(space, cpv( 160,  160), cpv( 240,  160), 0);
    cpBody *body5  = AddBar(space, cpv(-240,    0), cpv(-160,  -80), 2);
    cpBody *body6  = AddBar(space, cpv(-160,  -80), cpv( -80,    0), 2);
    cpBody *body7  = AddBar(space, cpv( -80,    0), cpv(   0,    0), 2);
    cpBody *body8  = AddBar(space, cpv(   0,  -80), cpv(  80,  -80), 0);
    cpBody *body9  = AddBar(space, cpv( 240,   80), cpv( 160,    0), 3);
    cpBody *body10 = AddBar(space, cpv( 160,    0), cpv( 240,  -80), 3);
    cpBody *body11 = AddBar(space, cpv(-240,  -80), cpv(-160, -160), 4);
    cpBody *body12 = AddBar(space, cpv(-160, -160), cpv( -80, -160), 4);
    cpBody *body13 = AddBar(space, cpv(   0, -160), cpv(  80, -160), 0);
    cpBody *body14 = AddBar(space, cpv( 160, -160), cpv( 240, -160), 0);

    cpSpaceAddConstraint(space, cpPivotJointNew2( body1,  body2, cpv( 40,-40), cpv(-40,-40)));
    cpSpaceAddConstraint(space, cpPivotJointNew2( body5,  body6, cpv( 40,-40), cpv(-40,-40)));
    cpSpaceAddConstraint(space, cpPivotJointNew2( body6,  body7, cpv( 40, 40), cpv(-40,  0)));
    cpSpaceAddConstraint(space, cpPivotJointNew2( body9, body10, cpv(-40,-40), cpv(-40, 40)));
    cpSpaceAddConstraint(space, cpPivotJointNew2(body11, body12, cpv( 40,-40), cpv(-40,  0)));

    cpFloat stiff = 100.0f;
    cpFloat damp = 0.5f;
    cpSpaceAddConstraint(space, NewSpring(staticBody,  body1, cpv(-320,  240), cpv(-40, 40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody,  body1, cpv(-320,   80), cpv(-40, 40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody,  body1, cpv(-160,  240), cpv(-40, 40), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody,  body2, cpv(-160,  240), cpv( 40, 40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody,  body2, cpv(   0,  240), cpv( 40, 40), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody,  body3, cpv(  80,  240), cpv(-40, 80), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody,  body4, cpv(  80,  240), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody,  body4, cpv( 320,  240), cpv( 40,  0), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody,  body5, cpv(-320,   80), cpv(-40, 40), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody,  body9, cpv( 320,  80), cpv( 40, 40), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody, body10, cpv( 320,   0), cpv( 40,-40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody, body10, cpv( 320,-160), cpv( 40,-40), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody, body11, cpv(-320,-160), cpv(-40, 40), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody, body12, cpv(-240,-240), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody, body12, cpv(   0,-240), cpv( 40,  0), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody, body13, cpv(   0,-240), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody, body13, cpv(  80,-240), cpv( 40,  0), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring(staticBody, body14, cpv(  80,-240), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody, body14, cpv( 240,-240), cpv( 40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(staticBody, body14, cpv( 320,-160), cpv( 40,  0), 0.0f, stiff, damp));

    cpSpaceAddConstraint(space, NewSpring( body1,  body5, cpv( 40,-40), cpv(-40, 40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body1,  body6, cpv( 40,-40), cpv( 40, 40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body2,  body3, cpv( 40, 40), cpv(-40, 80), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body3,  body4, cpv(-40, 80), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body3,  body4, cpv( 40,-80), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body3,  body7, cpv( 40,-80), cpv( 40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body3,  body7, cpv(-40, 80), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body3,  body8, cpv( 40,-80), cpv( 40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body3,  body9, cpv( 40,-80), cpv(-40,-40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body4,  body9, cpv( 40,  0), cpv( 40, 40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body5, body11, cpv(-40, 40), cpv(-40, 40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body5, body11, cpv( 40,-40), cpv( 40,-40), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body7,  body8, cpv( 40,  0), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body8, body12, cpv(-40,  0), cpv( 40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body8, body13, cpv(-40,  0), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body8, body13, cpv( 40,  0), cpv( 40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring( body8, body14, cpv( 40,  0), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(body10, body14, cpv( 40,-40), cpv(-40,  0), 0.0f, stiff, damp));
    cpSpaceAddConstraint(space, NewSpring(body10, body14, cpv( 40,-40), cpv(-40,  0), 0.0f, stiff, damp));
	
	return space;
}

cpFloat Springies::SpringForce(cpConstraint *spring, cpFloat dist)
{
	cpFloat clamp = 20.0f;
	return cpfclamp(cpDampedSpringGetRestLength(spring) - dist, -clamp, clamp)*cpDampedSpringGetStiffness(spring);
}

cpConstraint *Springies::NewSpring(cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, cpFloat restLength, cpFloat stiff, cpFloat damp)
{
	cpConstraint *spring = cpDampedSpringNew(a, b, anchorA, anchorB, restLength, stiff, damp);
	cpDampedSpringSetSpringForceFunc(spring, SpringForce);
	
	return spring;
}

cpBody *Springies::AddBar(cpSpace *space, cpVect a, cpVect b, int group)
{
	cpVect center = cpvmult(cpvadd(a, b), 1.0f/2.0f);
	cpFloat length = cpvlength(cpvsub(b, a));
	cpFloat mass = length/160.0f;
	
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, mass*length*length/12.0f));
	cpBodySetPosition(body, center);
	
	cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, cpvsub(a, center), cpvsub(b, center), 10.0f));
	cpShapeSetFilter(shape, cpShapeFilterNew(group, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
	
	return body;
}
