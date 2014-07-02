#include "Convex.h"
#include "chipmunk/chipmunk_unsafe.h"

#define DENSITY (1.0/10000.0)

Convex::Convex()
{
    name = "Convex";
}

cpSpace *Convex::Init()
{
    ChipmunkDemo::Init();

    message = "Hold right bottom corner and tap around the object to change its shape.";

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -500));
    cpSpaceSetSleepTimeThreshold(space, 0.5f);
    cpSpaceSetCollisionSlop(space, 0.5f);

    cpBody *body, *staticBody = cpSpaceGetStaticBody(space);

    // Create segments around the edge of the screen.
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    cpFloat width = 50.0f;
    cpFloat height = 70.0f;
    cpFloat mass = width*height*DENSITY;
    cpFloat moment = cpMomentForBox(mass, width, height);

    body = cpSpaceAddBody(space, cpBodyNew(mass, moment));

    shape = cpSpaceAddShape(space, cpBoxShapeNew(body, width, height, 0.0));
    cpShapeSetFriction(shape, 0.6f);
	
	return space;
}

bool Convex::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
{
    if (rightClick)
    {
        if (state == ChipmunkDemo::TOUCH_START)
        {
            cpFloat tolerance = 2.0;
            if(cpShapePointQuery(shape, pos, NULL) > tolerance){
                cpBody *body = cpShapeGetBody(shape);
                int count = cpPolyShapeGetCount(shape);

                // Allocate the space for the new vertexes on the stack.
                cpVect *verts = (cpVect *)alloca((count + 1)*sizeof(cpVect));

                for(int i=0; i<count; i++){
                    verts[i] = cpPolyShapeGetVert(shape, i);
                }

                verts[count] = cpBodyWorldToLocal(body, pos);

                // This function builds a convex hull for the vertexes.
                // Because the result array is the same as verts, it will reduce it in place.
                int hullCount = cpConvexHull(count + 1, verts, verts, NULL, tolerance);

                // Figure out how much to shift the body by.
                cpVect centroid = cpCentroidForPoly(hullCount, verts);

                // Recalculate the body properties to match the updated shape.
                cpFloat mass = cpAreaForPoly(hullCount, verts, 0.0f)*DENSITY;
                cpBodySetMass(body, mass);
                cpBodySetMoment(body, cpMomentForPoly(mass, hullCount, verts, cpvneg(centroid), 0.0f));
                cpBodySetPosition(body, cpBodyLocalToWorld(body, centroid));

                // Use the setter function from chipmunk_unsafe.h.
                // You could also remove and recreate the shape if you wanted.
                cpPolyShapeSetVerts(shape, hullCount, verts, cpTransformTranslate(cpvneg(centroid)));

                return true;   // don't process further
            }
        }
    }

    return ChipmunkDemo::ProcessTouch(id, pos, state, rightClick);
}
