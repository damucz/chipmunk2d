#ifndef __DEMO_H
#define __DEMO_H

#include <chipmunk/chipmunk_private.h>
#include <chipmunk/chipmunk.h>

#include <IwGxShaderTechnique.h>

#include <queue>
#include <string>
#include <map>

#define GRABBABLE_MASK_BIT (unsigned int)(1<<31)
extern cpShapeFilter GRAB_FILTER;
extern cpShapeFilter NOT_GRABBABLE_FILTER;

class CIwResGroup;

class ChipmunkDemo {
public:
    enum TouchState
    {
        TOUCH_START,
        TOUCH_MOVE,
        TOUCH_END,
    };

    ChipmunkDemo();
    virtual ~ChipmunkDemo() {};

	virtual cpSpace* Init();
	virtual void Update(double dt);
	virtual void Draw();
	virtual void Destroy();

    virtual void DrawChipmunk();
    virtual void DrawInfo();
    virtual void DrawMessage();

    virtual bool ProcessTouch(uint32 id, cpVect pos, TouchState state, bool rightClick = false);

    const char* GetName() const { return name; }
    double GetTimestep() const { return timestep; }
    const char* GetMessage() const { return message.c_str(); }

    static void DrawCircle(cpVect p, cpFloat a, cpFloat r, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer data);
    static void DrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer data);
    static void DrawFatSegment(cpVect a, cpVect b, cpFloat r, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer data);
    static void DrawPolygon(int count, const cpVect *verts, cpFloat radius, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer data);
    static void DrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer data);
    static void DrawBB(cpBB bb, cpSpaceDebugColor outlineColor, cpDataPointer data);

    static cpSpaceDebugColor ColorForShape(cpShape *shape, cpDataPointer data);

    static void ShapeFreeWrap(cpSpace *space, cpShape *shape, void *unused);
    static void PostShapeFree(cpShape *shape, cpSpace *space);
    static void ConstraintFreeWrap(cpSpace *space, cpConstraint *constraint, void *unused);
    static void PostConstraintFree(cpConstraint *constraint, cpSpace *space);
    static void BodyFreeWrap(cpSpace *space, cpBody *body, void *unused);
    static void PostBodyFree(cpBody *body, cpSpace *space);

protected:
    static const float ChipmunkDebugDrawPointLineScale;
    static const float ChipmunkDebugDrawOutlineWidth;

    const char *name;
    double timestep;

    cpSpace* space;

    std::string message;

    // interactivity
    class TouchJoint
    {
    public:
        TouchJoint();

        cpBody *mouse_body;
        cpConstraint *mouse_joint;
        cpVect lastTouchPoint;

        cpSpace* space;             // parent space link
    };
    std::map<int, TouchJoint> touchJoints;      // touch id to TouchJoint

    // stats
    int max_arbiters;
    int max_points;
    int max_constraints;

    double demoTime;
    int demoTicks;

    static cpSpaceDebugColor LAColor(float l, float a);
    static cpSpaceDebugColor RGBAColor(float r, float g, float b, float a);

    struct Triangle
    {
        struct Vertex
        {
            struct vec2
            {
                float x, y;
            };
            struct vec4
            {
                float r, g, b, a;
            };

            vec2 vertex;
            vec2 aa_coord;
            vec4 fill;
            vec4 outline;
        };

        Vertex v[3];

        void Set(int index, const cpVect& vertex, const cpVect& aa_coord, const cpSpaceDebugColor& fill, const cpSpaceDebugColor& outline);
        void Set(int index, const cpVect& vertex, const cpSpaceDebugColor& fill, const cpSpaceDebugColor& outline);
    };

    std::queue<Triangle> triangles;
    CIwGxShaderTechnique* shader;
    CIwResGroup* resGroup;

    static cpFloat frand(void);
    static cpVect frand_unit_circle();
};

#endif
