#include "Player.h"

#define PLAYER_VELOCITY 500.0

#define PLAYER_GROUND_ACCEL_TIME 0.1
#define PLAYER_GROUND_ACCEL (PLAYER_VELOCITY/PLAYER_GROUND_ACCEL_TIME)

#define PLAYER_AIR_ACCEL_TIME 0.25
#define PLAYER_AIR_ACCEL (PLAYER_VELOCITY/PLAYER_AIR_ACCEL_TIME)

#define JUMP_HEIGHT 50.0
#define JUMP_BOOST_HEIGHT 55.0
#define FALL_VELOCITY 900.0
#define GRAVITY 2000.0

Player::Player()
    : playerBody(NULL)
    , playerShape(NULL)
    , remainingBoost(0)
    , grounded(cpFalse)
    , lastJumpState(cpFalse)
    , jumpState(-1)
    , jumpVelocity(0)
{
    name = "Platformer Player Controls";
}

cpSpace *Player::Init()
{
    ChipmunkDemo::Init();

    space = cpSpaceNew();
    space->iterations = 10;
    space->gravity = cpv(0, -GRAVITY);
    //	space->sleepTimeThreshold = 1000;

    cpBody *body, *staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape;

    // Create segments around the edge of the screen.
    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(-320,240), 0.0f));
    shape->e = 1.0f; shape->u = 1.0f;
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(320,-240), cpv(320,240), 0.0f));
    shape->e = 1.0f; shape->u = 1.0f;
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f));
    shape->e = 1.0f; shape->u = 1.0f;
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,240), cpv(320,240), 0.0f));
    shape->e = 1.0f; shape->u = 1.0f;
    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);

    // Set up the player
    body = cpSpaceAddBody(space, cpBodyNew(1.0f, INFINITY));
    body->p = cpv(0, -200);
    body->velocity_func = PlayerUpdateVelocity;
    body->userData = this;
    playerBody = body;

    shape = cpSpaceAddShape(space, cpBoxShapeNew2(body, cpBBNew(-15.0, -27.5, 15.0, 27.5), 10.0));
    //	shape = cpSpaceAddShape(space, cpSegmentShapeNew(playerBody, cpvzero, cpv(0, radius), radius));
    shape->e = 0.0f; shape->u = 0.0f;
    shape->type = 1;
    playerShape = shape;

    // Add some boxes to jump on
    for(int i=0; i<6; i++){
        for(int j=0; j<3; j++){
            body = cpSpaceAddBody(space, cpBodyNew(4.0f, INFINITY));
            body->p = cpv(100 + j*60, -200 + i*60);

            shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 50, 50, 0.0));
            shape->e = 0.0f; shape->u = 0.7f;
        }
    }
	
	return space;
}

void Player::SelectPlayerGroundNormal(cpBody *body, cpArbiter *arb, cpVect *groundNormal){
	cpVect n = cpvneg(cpArbiterGetNormal(arb));
	
	if(n.y > groundNormal->y){
		(*groundNormal) = n;
	}
}

void Player::PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
    Player* th = static_cast<Player*>(body->userData);

	// Grab the grounding normal from last frame
	cpVect groundNormal = cpvzero;
	cpBodyEachArbiter(th->playerBody, (cpBodyArbiterIteratorFunc)SelectPlayerGroundNormal, &groundNormal);
	
	th->grounded = (groundNormal.y > 0.0);
	if(groundNormal.y < 0.0f) th->remainingBoost = 0.0f;
	
	// Do a normal-ish update
	cpBool boost = (th->jumpState >= 0 && th->remainingBoost > 0.0f);
	cpVect g = (boost ? cpvzero : gravity);
	cpBodyUpdateVelocity(body, g, damping, dt);
	
	// Target horizontal speed for air/ground control
	cpFloat target_vx = PLAYER_VELOCITY*th->jumpVelocity;
	
	// Update the surface velocity and friction
	// Note that the "feet" move in the opposite direction of the player.
	cpVect surface_v = cpv(-target_vx, 0.0);
	th->playerShape->surfaceV = surface_v;
	th->playerShape->u = (th->grounded ? PLAYER_GROUND_ACCEL/GRAVITY : 0.0);
	
	// Apply air control if not grounded
	if(!th->grounded){
		// Smoothly accelerate the velocity
		th->playerBody->v.x = cpflerpconst(th->playerBody->v.x, target_vx, PLAYER_AIR_ACCEL*dt);
	}
	
	body->v.y = cpfclamp(body->v.y, -FALL_VELOCITY, INFINITY);
}

void Player::Update(double dt)
{
	// If the jump key was just pressed this frame, jump!
	if(jumpState >= 0 && !lastJumpState && grounded){
		cpFloat jump_v = cpfsqrt(2.0*JUMP_HEIGHT*GRAVITY);
		playerBody->v = cpvadd(playerBody->v, cpv(0.0, jump_v));
		
		remainingBoost = JUMP_BOOST_HEIGHT/jump_v;
	}
	
	// Step the space
	ChipmunkDemo::Update(dt);
	
	remainingBoost -= dt;
	lastJumpState = jumpState >= 0;
}

bool Player::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
{
    if (state == TOUCH_START)
    {
        if (jumpState < 0)
        {
            jumpState = id;
            jumpVelocity = (pos.x - playerBody->p.x) / 100.0;
            return true;
        }
    }
    else if (state == TOUCH_END)
    {
        if (jumpState == (int)id)
        {
            jumpState = -1;
            jumpVelocity = 0;
            return true;
        }
    }

    return ChipmunkDemo::ProcessTouch(id, pos, state, rightClick);
}
