#include "LogoSmash.h"

static const int image_width = 188;
static const int image_height = 35;
static const int image_row_length = 24;

static const int image_bitmap[] = {
    15,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,-64,15,63,-32,-2,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,31,-64,15,127,-125,-1,-128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,127,-64,15,127,15,-1,-64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,-64,15,-2,
    31,-1,-64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,-64,0,-4,63,-1,-32,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,1,-1,-64,15,-8,127,-1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,-1,-64,0,-8,-15,-1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-31,-1,-64,15,-8,-32,
    -1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,-15,-1,-64,9,-15,-32,-1,-32,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,31,-15,-1,-64,0,-15,-32,-1,-32,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,63,-7,-1,-64,9,-29,-32,127,-61,-16,63,15,-61,-1,-8,31,-16,15,-8,126,7,-31,
    -8,31,-65,-7,-1,-64,9,-29,-32,0,7,-8,127,-97,-25,-1,-2,63,-8,31,-4,-1,15,-13,
    -4,63,-1,-3,-1,-64,9,-29,-32,0,7,-8,127,-97,-25,-1,-2,63,-8,31,-4,-1,15,-13,
    -2,63,-1,-3,-1,-64,9,-29,-32,0,7,-8,127,-97,-25,-1,-1,63,-4,63,-4,-1,15,-13,
    -2,63,-33,-1,-1,-32,9,-25,-32,0,7,-8,127,-97,-25,-1,-1,63,-4,63,-4,-1,15,-13,
    -1,63,-33,-1,-1,-16,9,-25,-32,0,7,-8,127,-97,-25,-1,-1,63,-4,63,-4,-1,15,-13,
    -1,63,-49,-1,-1,-8,9,-57,-32,0,7,-8,127,-97,-25,-8,-1,63,-2,127,-4,-1,15,-13,
    -1,-65,-49,-1,-1,-4,9,-57,-32,0,7,-8,127,-97,-25,-8,-1,63,-2,127,-4,-1,15,-13,
    -1,-65,-57,-1,-1,-2,9,-57,-32,0,7,-8,127,-97,-25,-8,-1,63,-2,127,-4,-1,15,-13,
    -1,-1,-57,-1,-1,-1,9,-57,-32,0,7,-1,-1,-97,-25,-8,-1,63,-1,-1,-4,-1,15,-13,-1,
    -1,-61,-1,-1,-1,-119,-57,-32,0,7,-1,-1,-97,-25,-8,-1,63,-1,-1,-4,-1,15,-13,-1,
    -1,-61,-1,-1,-1,-55,-49,-32,0,7,-1,-1,-97,-25,-8,-1,63,-1,-1,-4,-1,15,-13,-1,
    -1,-63,-1,-1,-1,-23,-49,-32,127,-57,-1,-1,-97,-25,-1,-1,63,-1,-1,-4,-1,15,-13,
    -1,-1,-63,-1,-1,-1,-16,-49,-32,-1,-25,-1,-1,-97,-25,-1,-1,63,-33,-5,-4,-1,15,
    -13,-1,-1,-64,-1,-9,-1,-7,-49,-32,-1,-25,-8,127,-97,-25,-1,-1,63,-33,-5,-4,-1,
    15,-13,-1,-1,-64,-1,-13,-1,-32,-49,-32,-1,-25,-8,127,-97,-25,-1,-2,63,-49,-13,
    -4,-1,15,-13,-1,-1,-64,127,-7,-1,-119,-17,-15,-1,-25,-8,127,-97,-25,-1,-2,63,
    -49,-13,-4,-1,15,-13,-3,-1,-64,127,-8,-2,15,-17,-1,-1,-25,-8,127,-97,-25,-1,
    -8,63,-49,-13,-4,-1,15,-13,-3,-1,-64,63,-4,120,0,-17,-1,-1,-25,-8,127,-97,-25,
    -8,0,63,-57,-29,-4,-1,15,-13,-4,-1,-64,63,-4,0,15,-17,-1,-1,-25,-8,127,-97,
    -25,-8,0,63,-57,-29,-4,-1,-1,-13,-4,-1,-64,31,-2,0,0,103,-1,-1,-57,-8,127,-97,
    -25,-8,0,63,-57,-29,-4,-1,-1,-13,-4,127,-64,31,-2,0,15,103,-1,-1,-57,-8,127,
    -97,-25,-8,0,63,-61,-61,-4,127,-1,-29,-4,127,-64,15,-8,0,0,55,-1,-1,-121,-8,
    127,-97,-25,-8,0,63,-61,-61,-4,127,-1,-29,-4,63,-64,15,-32,0,0,23,-1,-2,3,-16,
    63,15,-61,-16,0,31,-127,-127,-8,31,-1,-127,-8,31,-128,7,-128,0,0
};

LogoSmash::LogoSmash()
    : bodyCount(0)
{
    name = "Logo Smash";

    timestep = 1.0 / 60.0;
}

cpSpace *LogoSmash::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 1);

    // The space will contain a very large number of similary sized objects.
    // This is the perfect candidate for using the spatial hash.
    // Generally you will never need to do this.
    cpSpaceUseSpatialHash(space, 2.0, 10000);

    bodyCount = 0;

    cpBody *body;
    cpShape *shape;

    for(int y=0; y<image_height; y+=2){
        for(int x=0; x<image_width; x+=2){
            if(!GetPixel(x, y)) continue;

            cpFloat x_jitter = 0.05*frand();
            cpFloat y_jitter = 0.05*frand();

            shape = MakeBall(2*(x - image_width/2 + x_jitter), 2*(image_height/2 - y + y_jitter));
            cpSpaceAddBody(space, cpShapeGetBody(shape));
            cpSpaceAddShape(space, shape);

            bodyCount++;
        }
    }

    body = cpSpaceAddBody(space, cpBodyNew(1e9, INFINITY));
    cpBodySetPosition(body, cpv(-1000, -10));
    cpBodySetVelocity(body, cpv(400, 0));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(body, 8.0f, cpvzero));
    cpShapeSetElasticity(shape, 0.0);
    cpShapeSetFriction(shape, 0.0);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    bodyCount++;
	
	return space;
}

int LogoSmash::GetPixel(int x, int y)
{
	return ((char)image_bitmap[(x>>3) + y*image_row_length]>>(~x&0x7)) & 1;
}

void LogoSmash::DrawDotCallback(cpBody *body, void *data)
{
    cpDataPointer d = data;
	static_cast<LogoSmash*>(data)->DrawDot(3.0, cpBodyGetPosition(body), RGBAColor(200.0f/255.0f, 210.0f/255.0f, 230.0f/255.0f, 1.0f), &d);
}
/*
void LogoSmash::Draw()
{
	cpSpaceEachBody(space, DrawDotCallback, this);
	
//	ChipmunkDebugDrawCollisionPoints(space);
}
*/

cpShape *LogoSmash::MakeBall(cpFloat x, cpFloat y)
{
	cpBody *body = cpBodyNew(1.0, INFINITY);
	cpBodySetPosition(body, cpv(x, y));

	cpShape *shape = cpCircleShapeNew(body, 0.95, cpvzero);
	cpShapeSetElasticity(shape, 0.0);
	cpShapeSetFriction(shape, 0.0);
	
	return shape;
}
