#include "Planet.h"

const cpFloat Planet::gravityStrength = 5.0e6f;

Planet::Planet()
    : planetBody(NULL)
{
    name = "Planet";

    timestep = 1.0 / 60.0;
}

cpSpace *Planet::Init()
{
    ChipmunkDemo::Init();

    // Create a rouge body to control the planet manually.
    space = cpSpaceNew();
    cpSpaceSetIterations(space, 20);

    planetBody = cpSpaceAddBody(space, cpBodyNewKinematic());
    cpBodySetAngularVelocity(planetBody, 0.2f);

    for(int i=0; i<30; i++){
        AddBox(space);
    }

    cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(planetBody, 70.0f, cpvzero));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);
	
	return space;
}

void Planet::PlanetGravityVelocityFunc(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
	// Gravitational acceleration is proportional to the inverse square of
	// distance, and directed toward the origin. The central planet is assumed
	// to be massive enough that it affects the satellites but not vice versa.
	cpVect p = cpBodyGetPosition(body);
	cpFloat sqdist = cpvlengthsq(p);
	cpVect g = cpvmult(p, -gravityStrength / (sqdist * cpfsqrt(sqdist)));
	
	cpBodyUpdateVelocity(body, g, damping, dt);
}

cpVect Planet::RandPos(cpFloat radius)
{
	cpVect v;
	do {
		v = cpv(frand()*(640 - 2*radius) - (320 - radius), frand()*(480 - 2*radius) - (240 - radius));
	} while(cpvlength(v) < 85.0f);
	
	return v;
}

void Planet::AddBox(cpSpace *space)
{
	const cpFloat size = 10.0f;
	const cpFloat mass = 1.0f;
	
	cpVect verts[] = {
		cpv(-size,-size),
		cpv(-size, size),
		cpv( size, size),
		cpv( size,-size),
	};
	
	cpFloat radius = cpvlength(cpv(size, size));
	cpVect pos = RandPos(radius);
	
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, 4, verts, cpvzero, 0.0f)));
	cpBodySetVelocityUpdateFunc(body, PlanetGravityVelocityFunc);
	cpBodySetPosition(body, pos);

	// Set the box's velocity to put it into a circular orbit from its
	// starting position.
	cpFloat r = cpvlength(pos);
	cpFloat v = cpfsqrt(gravityStrength / r) / r;
	cpBodySetVelocity(body, cpvmult(cpvperp(pos), v));

	// Set the box's angular velocity to match its orbital period and
	// align its initial angle with its position.
	cpBodySetAngularVelocity(body, v);
	cpBodySetAngle(body, cpfatan2(pos.y, pos.x));

	cpShape *shape = cpSpaceAddShape(space, cpPolyShapeNew(body, 4, verts, cpTransformIdentity, 0.0));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.7f);
}
