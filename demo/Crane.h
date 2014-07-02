#ifndef __CRANE_H
#define __CRANE_H

#include "demo.h"

class Crane : public ChipmunkDemo
{
public:
    Crane();
    ~Crane() {};

    virtual cpSpace* Init();
    void Update(double dt);

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

protected:
    cpBody *dollyBody;
    // Constraint used as a servo motor to move the dolly back and forth.
    cpConstraint *dollyServo;

    // Constraint used as a winch motor to lift the load.
    cpConstraint *winchServo;

    // Temporary joint used to hold the hook to the load.
    cpConstraint *hookJoint;


    cpVect lastTouchPoint;

    enum COLLISION_TYPES {
        HOOK_SENSOR = 1,
        CRATE,
    };

    static void AttachHook(cpSpace *space, cpBody *hook, Crane *data);
    static cpBool HookCrate(cpArbiter *arb, cpSpace *space, Crane *data);
    cpBody *crate;   // helper variable to pass arbiter from HookCrate to AttachHook
};

#endif
