#ifndef __PLAYER_H
#define __PLAYER_H

#include "demo.h"

class Player : public ChipmunkDemo
{
public:
    Player();
    ~Player() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpBody *playerBody;
    cpShape *playerShape;

    cpFloat remainingBoost;
    cpBool grounded;
    cpBool lastJumpState;
    int jumpState;
    cpFloat jumpVelocity;

    static void SelectPlayerGroundNormal(cpBody *body, cpArbiter *arb, cpVect *groundNormal);
    static void PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);
};

#endif
