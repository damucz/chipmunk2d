#include "Slice.h"

#define DENSITY (1.0/10000.0)

Slice::Slice()
    : touchId(-1)
{
    name = "Slice";

    timestep = 1.0 / 60.0;

    sliceStart.x = 0.0;
    sliceStart.y = 0.0;
}

cpSpace *Slice::Init()
{
    ChipmunkDemo::Init();

    message = "Hold right bottom corner and slice with touch.";

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -500));
    cpSpaceSetSleepTimeThreshold(space, 0.5f);
    cpSpaceSetCollisionSlop(space, 0.5f);

    cpBody *body, *staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape;

    // Create segments around the edge of the screen.
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-1000,-240), cpv(1000,-240), 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    cpFloat width = 200.0f;
    cpFloat height = 300.0f;
    cpFloat mass = width*height*DENSITY;
    cpFloat moment = cpMomentForBox(mass, width, height);

    body = cpSpaceAddBody(space, cpBodyNew(mass, moment));

    shape = cpSpaceAddShape(space, cpBoxShapeNew(body, width, height, 0.0));
    cpShapeSetFriction(shape, 0.6f);
	
	return space;
}

void Slice::ClipPoly(cpSpace *space, cpShape *shape, cpVect n, cpFloat dist)
{
	cpBody *body = cpShapeGetBody(shape);
	
	int count = cpPolyShapeGetCount(shape);
	int clippedCount = 0;
	
	cpVect *clipped = (cpVect *)alloca((count + 1)*sizeof(cpVect));
	
	for(int i=0, j=count-1; i<count; j=i, i++){
		cpVect a = cpBodyLocalToWorld(body, cpPolyShapeGetVert(shape, j));
		cpFloat a_dist = cpvdot(a, n) - dist;
		
		if(a_dist < 0.0){
			clipped[clippedCount] = a;
			clippedCount++;
		}
		
		cpVect b = cpBodyLocalToWorld(body, cpPolyShapeGetVert(shape, i));
		cpFloat b_dist = cpvdot(b, n) - dist;
		
		if(a_dist*b_dist < 0.0f){
			cpFloat t = cpfabs(a_dist)/(cpfabs(a_dist) + cpfabs(b_dist));
			
			clipped[clippedCount] = cpvlerp(a, b, t);
			clippedCount++;
		}
	}
	
	cpVect centroid = cpCentroidForPoly(clippedCount, clipped);
	cpFloat mass = cpAreaForPoly(clippedCount, clipped, 0.0f)*DENSITY;
	cpFloat moment = cpMomentForPoly(mass, clippedCount, clipped, cpvneg(centroid), 0.0f);
	
	cpBody *new_body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
	cpBodySetPosition(new_body, centroid);
	cpBodySetVelocity(new_body, cpBodyGetVelocityAtWorldPoint(body, centroid));
	cpBodySetAngularVelocity(new_body, cpBodyGetAngularVelocity(body));
	
	cpTransform transform = cpTransformTranslate(cpvneg(centroid));
	cpShape *new_shape = cpSpaceAddShape(space, cpPolyShapeNew(new_body, clippedCount, clipped, transform, 0.0));
	// Copy whatever properties you have set on the original shape that are important
	cpShapeSetFriction(new_shape, cpShapeGetFriction(shape));
}

// Context structs are annoying, use blocks or closures instead if your compiler supports them.
struct SliceContext {
	cpVect a, b;
	cpSpace *space;
};

void Slice::SliceShapePostStep(cpSpace *space, cpShape *shape, struct SliceContext *context)
{
	cpVect a = context->a;
	cpVect b = context->b;
	
	// Clipping plane normal and distance.
	cpVect n = cpvnormalize(cpvperp(cpvsub(b, a)));
	cpFloat dist = cpvdot(a, n);
	
	ClipPoly(space, shape, n, dist);
	ClipPoly(space, shape, cpvneg(n), -dist);
	
	cpBody *body = cpShapeGetBody(shape);
	cpSpaceRemoveShape(space, shape);
	cpSpaceRemoveBody(space, body);
	cpShapeFree(shape);
	cpBodyFree(body);
}

void Slice::SliceQuery(cpShape *shape, cpVect point, cpVect normal, cpFloat alpha, struct SliceContext *context)
{
	cpVect a = context->a;
	cpVect b = context->b;
	
	// Check that the slice was complete by checking that the endpoints aren't in the sliced shape.
	if(cpShapePointQuery(shape, a, NULL) > 0.0f && cpShapePointQuery(shape, b, NULL) > 0.0f){
		// Can't modify the space during a query.
		// Must make a post-step callback to do the actual slicing.
		cpSpaceAddPostStepCallback(context->space, (cpPostStepFunc)SliceShapePostStep, shape, context);
	}
}

void Slice::Update(double dt)
{
    if (touchId >= 0)
    {
        cpDataPointer data = this;
        DrawSegment(sliceStart, lastTouchPoint, RGBAColor(1, 0, 0, 1), data);
    }

    ChipmunkDemo::Update(dt);
}

bool Slice::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
{
    if (rightClick)
    {
        if (state == ChipmunkDemo::TOUCH_START)
        {
            if (touchId < 0)
            {
                touchId = id;
                lastTouchPoint = pos;
			    sliceStart = pos;

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
                struct SliceContext context = {sliceStart, pos, space};
                cpSpaceSegmentQuery(space, sliceStart, pos, 0.0, GRAB_FILTER, (cpSpaceSegmentQueryFunc)SliceQuery, &context);

                lastTouchPoint = pos;
                touchId = -1;
                return true;
            }
        }
    }

    return ChipmunkDemo::ProcessTouch(id, pos, state, rightClick);
}
