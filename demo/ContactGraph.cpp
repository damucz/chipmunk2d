#include "ContactGraph.h"

ContactGraph::ContactGraph()
{
    name = "Contact Graph";
}

#define WIDTH 4.0f
#define HEIGHT 30.0f

cpSpace *ContactGraph::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0, -300));
    cpSpaceSetCollisionSlop(space, 0.5);
    cpSpaceSetSleepTimeThreshold(space, 1.0f);

    cpBody *body, *staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape;

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

    scaleStaticBody = cpSpaceAddBody(space, cpBodyNewStatic());
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(scaleStaticBody, cpv(-240,-180), cpv(-140,-180), 4.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    // add some boxes to stack on the scale
    for(int i=0; i<5; i++){
        body = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForBox(1.0f, 30.0f, 30.0f)));
        cpBodySetPosition(body, cpv(0, i*32 - 220));

        shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 30.0f, 30.0f, 0.0));
        cpShapeSetElasticity(shape, 0.0f);
        cpShapeSetFriction(shape, 0.8f);
    }

    // Add a ball that we'll track which objects are beneath it.
    cpFloat radius = 15.0f;
    ballBody = cpSpaceAddBody(space, cpBodyNew(10.0f, cpMomentForCircle(10.0f, 0.0f, radius, cpvzero)));
    cpBodySetPosition(ballBody, cpv(120, -240 + radius+5));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
    cpShapeSetElasticity(shape, 0.0f);
    cpShapeSetFriction(shape, 0.9f);
	
	return space;
}

#if !USE_BLOCKS

void ContactGraph::ScaleIterator(cpBody *body, cpArbiter *arb, ScaleIteratorContext *context)
{
	context->sum = cpvadd(context->sum, cpArbiterTotalImpulse(arb));
}

void ContactGraph::BallIterator(cpBody *body, cpArbiter *arb, BallIteratorContext *context)
{
	// body is the body we are iterating the arbiters for.
	// CP_ARBITER_GET_*() in an arbiter iterator always returns the body/shape for the iterated body first.
	CP_ARBITER_GET_SHAPES(arb, ball, other);
    cpDataPointer data = context->th;
	context->th->DrawBB(cpShapeGetBB(other), RGBAColor(1, 0, 0, 1), &data);

	context->count++;
}

struct CrushingContext {
	cpFloat magnitudeSum;
	cpVect vectorSum;
};

void ContactGraph::EstimateCrushing(cpBody *body, cpArbiter *arb, struct CrushingContext *context)
{
	cpVect j = cpArbiterTotalImpulse(arb);
	context->magnitudeSum += cpvlength(j);
	context->vectorSum = cpvadd(context->vectorSum, j);
}

#endif

void ContactGraph::Update(double dt)
{
	ChipmunkDemo::Update(dt);

	message = "Place objects on the scale to weigh them.\nThe ball marks the shapes it's sitting on.\n";
	
	// Sum the total impulse applied to the scale from all collision pairs in the contact graph.
	// If your compiler supports blocks, your life is a little easier.
	// You can use the "Block" versions of the functions without needing the callbacks above.
	#if USE_BLOCKS
		__block cpVect impulseSum = cpvzero;
		cpBodyEachArbiter_b(scaleStaticBody, ^(cpArbiter *arb){
			impulseSum = cpvadd(impulseSum, cpArbiterTotalImpulse(arb));
		});
	#else
        ScaleIteratorContext contextS;
		contextS.sum = cpvzero;
        contextS.th = this;
		cpBodyEachArbiter(scaleStaticBody, (cpBodyArbiterIteratorFunc)ScaleIterator, &contextS);
        cpVect impulseSum = contextS.sum;
	#endif
	
	// Force is the impulse divided by the timestep.
	cpFloat force = cpvlength(impulseSum)/dt;
		
	// Weight can be found similarly from the gravity vector.
	cpVect g = cpSpaceGetGravity(space);
	cpFloat weight = cpvdot(g, impulseSum)/(cpvlengthsq(g)*dt);
	
    char buff[128];
    snprintf(buff, sizeof(buff), "Total force: %5.2f, Total weight: %5.2f. ", force, weight);
    message += buff;

	
	// Highlight and count the number of shapes the ball is touching.
	#if USE_BLOCKS
		__block int count = 0;
		cpBodyEachArbiter_b(ballBody, ^(cpArbiter *arb){
			// body is the body we are iterating the arbiters for.
			// CP_ARBITER_GET_*() in an arbiter iterator always returns the body/shape for the iterated body first.
			CP_ARBITER_GET_SHAPES(arb, ball, other);
			ChipmunkDebugDrawBB(cpShapeGetBB(other), RGBAColor(1, 0, 0, 1));
			
			count++;
		});
	#else
		BallIteratorContext contextB;
        contextB.count = 0;
        contextB.th = this;
		cpBodyEachArbiter(ballBody, (cpBodyArbiterIteratorFunc)BallIterator, &contextB);
        int count = contextB.count;
	#endif
	
    snprintf(buff, sizeof(buff), "The ball is touching %d shapes.\n", count);
    message += buff;
	
	#if USE_BLOCKS
		__block cpFloat magnitudeSum = 0.0f;
		__block cpVect vectorSum = cpvzero;
		cpBodyEachArbiter_b(ballBody, ^(cpArbiter *arb){
			cpVect j = cpArbiterTotalImpulse(arb);
			magnitudeSum += cpvlength(j);
			vectorSum = cpvadd(vectorSum, j);
		});
		
		cpFloat crushForce = (magnitudeSum - cpvlength(vectorSum))*dt;
	#else
		CrushingContext contextC = {0.0f, cpvzero, this};
		cpBodyEachArbiter(ballBody, (cpBodyArbiterIteratorFunc)EstimateCrushing, &contextC);
		
		cpFloat crushForce = (contextC.magnitudeSum - cpvlength(contextC.vectorSum))*dt;
	#endif
	
	
	if(crushForce > 10.0f){
        snprintf(buff, sizeof(buff), "The ball is being crushed. (f: %.2f)", crushForce);
	} else {
        snprintf(buff, sizeof(buff), "The ball is not being crushed. (f: %.2f)", crushForce);
	}
    message += buff;
}
