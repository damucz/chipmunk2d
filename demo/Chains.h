#ifndef __CHAINS_H
#define __CHAINS_H

#include "demo.h"

class Chains : public ChipmunkDemo
{
public:
    Chains();
    ~Chains() {};

    virtual cpSpace* Init();

protected:
    static void BreakablejointPostStepRemove(cpSpace *space, cpConstraint *joint, void *unused);
    static void BreakableJointPostSolve(cpConstraint *joint, cpSpace *space);
};

#endif
