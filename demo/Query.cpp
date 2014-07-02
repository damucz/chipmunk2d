#include "Query.h"

Query::Query()
    : touchId(-1)
{
    name = "Segment Query";

    timestep = 1.0 / 60.0;

    QUERY_START.x = 0;
    QUERY_START.y = 0;

    lastTouchPoint.x = 100.0;
    lastTouchPoint.y = 100.0;
}

cpSpace *Query::Init()
{
    ChipmunkDemo::Init();

    QUERY_START = cpvzero;

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 5);

    { // add a fat segment
        cpFloat mass = 1.0f;
        cpFloat length = 100.0f;
        cpVect a = cpv(-length/2.0f, 0.0f), b = cpv(length/2.0f, 0.0f);

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForSegment(mass, a, b, 0.0f)));
        cpBodySetPosition(body, cpv(0.0f, 100.0f));

        cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, 20.0f));
    }

    { // add a static segment
        cpSpaceAddShape(space, cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(0, 300), cpv(300, 0), 0.0f));
    }

    { // add a pentagon
        cpFloat mass = 1.0f;
        const int NUM_VERTS = 5;

        cpVect verts[NUM_VERTS];
        for(int i=0; i<NUM_VERTS; i++){
            cpFloat angle = -2*M_PI*i/((cpFloat) NUM_VERTS);
            verts[i] = cpv(30*cos(angle), 30*sin(angle));
        }

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, NUM_VERTS, verts, cpvzero, 0.0f)));
        cpBodySetPosition(body, cpv(50.0f, 30.0f));

        cpSpaceAddShape(space, cpPolyShapeNew(body, NUM_VERTS, verts, cpTransformIdentity, 10.0f));
    }

    { // add a circle
        cpFloat mass = 1.0f;
        cpFloat r = 20.0f;

        cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, r, cpvzero)));
        cpBodySetPosition(body, cpv(100.0f, 100.0f));

        cpSpaceAddShape(space, cpCircleShapeNew(body, r, cpvzero));
    }
	
	return space;
}

void Query::Update(double dt)
{
	ChipmunkDemo::Update(dt);

    cpDataPointer data = this;
	
	cpVect start = QUERY_START;
	cpVect end = lastTouchPoint;
	cpFloat radius = 10.0;
	DrawSegment(start, end, RGBAColor(0,1,0,1), &data);
	
    message = "Hold right bottom corner and drag in scene to set raycast vector.\n";

    char buf[128];
    snprintf(buf, sizeof(buf), "Query: Dist(%f) Point(%5.2f, %5.2f), ", cpvdist(start, end), end.x, end.y);
	message += buf;
	
	cpSegmentQueryInfo segInfo = {};
	if(cpSpaceSegmentQueryFirst(space, start, end, radius, CP_SHAPE_FILTER_ALL, &segInfo)){
		cpVect point = segInfo.point;
		cpVect n = segInfo.normal;
		
		// Draw blue over the occluded part of the query
		DrawSegment(cpvlerp(start, end, segInfo.alpha), end, RGBAColor(0,0,1,1), &data);
		
		// Draw a little red surface normal
		DrawSegment(point, cpvadd(point, cpvmult(n, 16)), RGBAColor(1,0,0,1), &data);
		
		// Draw a little red dot on the hit point.
		DrawDot(3, point, RGBAColor(1,0,0,1), &data);

        snprintf(buf, sizeof(buf), "Segment Query: Dist(%f) Normal(%5.2f, %5.2f)", segInfo.alpha*cpvdist(start, end), n.x, n.y);
	} else {
        snprintf(buf, sizeof(buf), "Segment Query (None)");
	}
    message += buf;
	
	// Draw a fat green line over the unoccluded part of the query
	DrawFatSegment(start, cpvlerp(start, end, segInfo.alpha), radius, RGBAColor(0,1,0,1), LAColor(0,0), &data);
	
	cpPointQueryInfo nearestInfo = {};
	cpSpacePointQueryNearest(space, lastTouchPoint, 100.0, CP_SHAPE_FILTER_ALL, &nearestInfo);
	if(nearestInfo.shape){
		// Draw a grey line to the closest shape.
		DrawDot(3, lastTouchPoint, RGBAColor(0.5, 0.5, 0.5, 1.0), &data);
		DrawSegment(lastTouchPoint, nearestInfo.point, 	RGBAColor(0.5, 0.5, 0.5, 1.0), &data);
		
		// Draw a red bounding box around the shape under the mouse.
		if(nearestInfo.distance < 0) DrawBB(cpShapeGetBB(nearestInfo.shape), RGBAColor(1,0,0,1), &data);
	}
}

bool Query::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
{
    if (rightClick)
    {
        if (state == TOUCH_START)
        {
            if (touchId < 0)
            {
                QUERY_START = pos;
                touchId = id;
                lastTouchPoint = pos;

                return true;
            }
        }
        else if (state == TOUCH_MOVE)
        {
            if (touchId == id)
            {
                lastTouchPoint = pos;
                return true;
            }
        }
        else /* if (state == TOUCH_END) */
        {
            if (touchId == id)
            {
                lastTouchPoint = pos;
                touchId = -1;
                return true;
            }
        }
    }

    return ChipmunkDemo::ProcessTouch(id, pos, state, rightClick);
}
