#ifndef __CONTACTGRAPH_H
#define __CONTACTGRAPH_H

#include "demo.h"

// If your compiler supports blocks (Clang or some GCC versions),
// You can use the block based iterators instead of the function ones to make your life easier.
#if defined(__has_extension)
#if __has_extension(blocks)

#define USE_BLOCKS 1

#endif
#endif

class ContactGraph : public ChipmunkDemo
{
public:
    ContactGraph();
    ~ContactGraph() {};

    virtual cpSpace* Init();
    virtual void Update(double dt);

protected:
    // static body that we will be making into a scale
    cpBody *scaleStaticBody;
    cpBody *ballBody;

#if !USE_BLOCKS

    struct ScaleIteratorContext
    {
        cpVect sum;
        ChipmunkDemo *th;
    };
    static void ScaleIterator(cpBody *body, cpArbiter *arb, ScaleIteratorContext *sum);

    struct BallIteratorContext
    {
        int count;
        ChipmunkDemo *th;
    };
    static void BallIterator(cpBody *body, cpArbiter *arb, BallIteratorContext *count);

    struct CrushingContext {
        cpFloat magnitudeSum;
        cpVect vectorSum;
        ChipmunkDemo *th;
    };

    static void EstimateCrushing(cpBody *body, cpArbiter *arb, struct CrushingContext *context);

#endif

};

#endif
