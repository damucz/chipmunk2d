#include "demo.h"

#include <IwGeomFMat.h>
#include <IwGx.h>
#include <IwColour.h>

cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

const float ChipmunkDemo::ChipmunkDebugDrawPointLineScale = 1.0f;
const float ChipmunkDemo::ChipmunkDebugDrawOutlineWidth = 1.0f;

ChipmunkDemo::ChipmunkDemo()
    : space(NULL)
    , name("demo")
    , timestep(1.0/180.0)
    , max_arbiters(0)
    , max_points(0)
    , max_constraints(0)
    , shader(NULL)
    , resGroup(NULL)
    , demoTime(0)
    , demoTicks(0)
{

}

cpSpace* ChipmunkDemo::Init()
{
    resGroup = IwGetResManager()->LoadGroup("shaders.group");
    shader = (CIwGxShaderTechnique*)IwGetResManager()->GetResNamed("chipmunk2d", "CIwGxShaderTechnique");

    return NULL;
}

void ChipmunkDemo::Update( double dt )
{
    for (std::map<int, TouchJoint>::iterator jointIt = touchJoints.begin(); jointIt != touchJoints.end(); ++jointIt)
    {
        cpBody* mouse_body = jointIt->second.mouse_body;

        cpVect new_point = cpvlerp(mouse_body->p, jointIt->second.lastTouchPoint, 0.25f);
        mouse_body->v = cpvmult(cpvsub(new_point, mouse_body->p), 60.0f);
        mouse_body->p = new_point;
    }

    if (space)
        cpSpaceStep(space, dt);

    demoTime += dt;
    ++demoTicks;
}

void ChipmunkDemo::Draw()
{
    DrawChipmunk();
    DrawInfo();
    DrawMessage();
}

void ChipmunkDemo::DrawChipmunk()
{
    cpSpaceDebugDrawOptions drawOptions = {
        DrawCircle,
        DrawSegment,
        DrawFatSegment,
        DrawPolygon,
        DrawDot,

        static_cast<cpSpaceDebugDrawFlags>(CP_SPACE_DEBUG_DRAW_SHAPES | CP_SPACE_DEBUG_DRAW_CONSTRAINTS | CP_SPACE_DEBUG_DRAW_COLLISION_POINTS),

        {200.0f/255.0f, 210.0f/255.0f, 230.0f/255.0f, 1.0f},
        ColorForShape,
        {0.0f, 0.75f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f, 1.0f},
        this,
    };

    //IwAssert(CHIPMUNK2D, triangles.empty());          buoyance draws in update, so the triangles are not empty here
    cpSpaceDebugDraw(space, &drawOptions);

    CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
    mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
    mat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
    mat->SetShaderTechnique(shader);
    mat->SetCullMode(CIwMaterial::CULL_NONE);
    IwGxSetMaterial(mat);
    /*
    while (!triangles.empty()) triangles.pop();

    Triangle t;
    cpSpaceDebugColor col = {0.1f, 0.1f, 0.1f, 0.1f};
    cpSpaceDebugColor outcol = {1.0f, 1.0f, 1.0f, 1.0f};
    cpVect pos0 = {0.0, 0.0};
    cpVect pos1 = {0.0, 200.0};
    cpVect pos2 = {200.0, 200.0};
    t.Set(0, pos0, col, outcol);
    t.Set(1, pos1, col, outcol);
    t.Set(2, pos2, col, outcol);
    triangles.push(t);
    */

    int nvertices = (int)triangles.size() * 3;
    CIwFVec3 *verts = IW_GX_ALLOC(CIwFVec3, nvertices);
    CIwFVec2 *aacoord = IW_GX_ALLOC(CIwFVec2, nvertices);
    CIwFVec2 *alphas = IW_GX_ALLOC(CIwFVec2, nvertices);
    CIwFVec3 *fill = IW_GX_ALLOC(CIwFVec3, nvertices);
    CIwFVec3 *out = IW_GX_ALLOC(CIwFVec3, nvertices);
    //CIwColour *colors = IW_GX_ALLOC(CIwColour, nvertices);
    uint16 *indices = IW_GX_ALLOC(uint16, nvertices);
    int i = 0;
    while (!triangles.empty())
    {
        Triangle& t = triangles.front();

        for (int j = 0; j < 3; ++j)
        {
            Triangle::Vertex& v = t.v[j];

            fill[i].x = v.fill.r;
            fill[i].y = v.fill.g;
            fill[i].z = v.fill.b;

            out[i].x = v.outline.r;
            out[i].y = v.outline.g;
            out[i].z = v.outline.b;

            alphas[i].x = v.fill.a;
            alphas[i].y = v.outline.a;

            aacoord[i].x = v.aa_coord.x;
            aacoord[i].y = v.aa_coord.y;

            verts[i].x = v.vertex.x;
            verts[i].y = v.vertex.y;
            verts[i].z = 10.0f;

            // unused
            //colors[i].r = 255;
            //colors[i].g = 255;
            //colors[i].b = 255;

            indices[i] = i;

            ++i;
        }

        triangles.pop();
    }

    IwGxSetUVStream(aacoord);
    IwGxSetUVStream(alphas, 1);
    IwGxSetTangentStream(fill);
    IwGxSetBiTangentStream(out);
    //IwGxSetColStream(colors, nvertices);
    IwGxSetVertStreamViewSpace(verts, nvertices);
    IwGxSetNormStream(NULL);

    IwGxDrawPrims(IW_GX_TRI_LIST, indices, nvertices);

    //IwGxFlush();
}

void ChipmunkDemo::DrawInfo()
{
    int arbiters = space->arbiters->num;
    int points = 0;

    for(int i=0; i<arbiters; i++)
        points += ((cpArbiter *)(space->arbiters->arr[i]))->count;

    int constraints = (space->constraints->num + points)*space->iterations;

    max_arbiters = arbiters > max_arbiters ? arbiters : max_arbiters;
    max_points = points > max_points ? points : max_points;
    max_constraints = constraints > max_constraints ? constraints : max_constraints;

    char buffer[1024];
    const char *format = 
        "Arbiters: %d (%d) - "
        "Contact Points: %d (%d)\n"
        "Other Constraints: %d, Iterations: %d\n"
        "Constraints x Iterations: %d (%d)\n"
        "Time:% 5.2fs, KE:% 5.2e";

    cpArray *bodies = space->dynamicBodies;
    cpFloat ke = 0.0f;
    for(int i=0; i<bodies->num; i++){
        cpBody *body = (cpBody *)bodies->arr[i];
        if(body->m == INFINITY || body->i == INFINITY) continue;

        ke += body->m*cpvdot(body->v, body->v) + body->i*body->w*body->w;
    }

    snprintf(buffer, sizeof(buffer), format,
        arbiters, max_arbiters,
        points, max_points,
        space->constraints->num, space->iterations,
        constraints, max_constraints,
        demoTime, (ke < 1e-10f ? 0.0f : ke)
        );

    IwGxPrintString(0, IwGxGetScreenHeight() - 50, buffer);
}

void ChipmunkDemo::DrawMessage()
{
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s", message.c_str());

    IwGxPrintString(0, IwGxGetScreenHeight() - 110, buffer);
}


void ChipmunkDemo::Destroy()
{
    if (space)
    {
        // Mouse joints aren't managed by space
        for (std::map<int, TouchJoint>::iterator jointIt = touchJoints.begin(); jointIt != touchJoints.end(); ++jointIt)
        {
            // Constraints has to be removed before mouse body
            cpSpaceRemoveConstraint(space, jointIt->second.mouse_joint);
            cpConstraintFree(jointIt->second.mouse_joint);

            cpBodyFree(jointIt->second.mouse_body);
        }

        // Safe and future proof way to remove and free all objects that have been added to the space.
        // Must remove these BEFORE freeing the body or you will access dangling pointers.
        cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)PostShapeFree, space);
        cpSpaceEachConstraint(space, (cpSpaceConstraintIteratorFunc)PostConstraintFree, space);
        cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)PostBodyFree, space);

        cpSpaceFree(space);
    }

    if (shader)
        delete shader;

    if (resGroup)
        IwGetResManager()->DestroyGroup(resGroup);
}

bool ChipmunkDemo::ProcessTouch( uint32 id, cpVect pos, TouchState state, bool rightClick /*= false*/ )
{
    std::map<int, TouchJoint>::iterator jointIt = touchJoints.find(id);
    bool exists = jointIt != touchJoints.end();

    if (!rightClick)
    {
        if (state == TOUCH_START)
        {
            if (!exists)
            {
                // give the mouse click a little radius to make it easier to click small shapes.
                cpFloat radius = 5.0;

                cpPointQueryInfo info = {};
                cpShape *shape = cpSpacePointQueryNearest(space, pos, radius, GRAB_FILTER, &info);

                if(shape && cpBodyGetMass(cpShapeGetBody(shape)) < INFINITY){
                    // Use the closest point on the surface if the click is outside of the shape.
                    cpVect nearest = (info.distance > 0.0f ? info.point : pos);

                    TouchJoint joint;
                    joint.space = space;
                    joint.mouse_body = cpBodyNewKinematic();
                    joint.mouse_body->p = pos;
                    joint.lastTouchPoint = pos;

                    cpBody *body = cpShapeGetBody(shape);
                    joint.mouse_joint = cpPivotJointNew2(joint.mouse_body, body, cpvzero, cpBodyWorldToLocal(body, nearest));
                    joint.mouse_joint->maxForce = 50000.0f;
                    joint.mouse_joint->errorBias = cpfpow(1.0f - 0.15f, 60.0f);
                    cpSpaceAddConstraint(space, joint.mouse_joint);

                    touchJoints[id] = joint;

                    return true;
                }
            }
        }
        else if (state == TOUCH_MOVE)
        {
            if (exists)
            {
                jointIt->second.lastTouchPoint = pos;
                return true;
            }
        }
        else // if (state == TOUCH_END)
        {
            if(exists && jointIt->second.mouse_joint)
            {
                TouchJoint& joint = jointIt->second;

                cpSpaceRemoveConstraint(space, joint.mouse_joint);
                cpConstraintFree(joint.mouse_joint);

                cpBodyFree(joint.mouse_body);

                touchJoints.erase(jointIt);

                return true;
            }
        }
    }

    return false;
}

void ChipmunkDemo::ShapeFreeWrap( cpSpace *space, cpShape *shape, void *unused )
{
    cpSpaceRemoveShape(space, shape);
    cpShapeFree(shape);
}

void ChipmunkDemo::PostShapeFree( cpShape *shape, cpSpace *space )
{
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc)ShapeFreeWrap, shape, NULL);
}

void ChipmunkDemo::ConstraintFreeWrap( cpSpace *space, cpConstraint *constraint, void *unused )
{
    cpSpaceRemoveConstraint(space, constraint);
    cpConstraintFree(constraint);
}

void ChipmunkDemo::PostConstraintFree( cpConstraint *constraint, cpSpace *space )
{
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc)ConstraintFreeWrap, constraint, NULL);
}

void ChipmunkDemo::BodyFreeWrap( cpSpace *space, cpBody *body, void *unused )
{
    cpSpaceRemoveBody(space, body);
    cpBodyFree(body);
}

void ChipmunkDemo::PostBodyFree( cpBody *body, cpSpace *space )
{
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc)BodyFreeWrap, body, NULL);
}

cpSpaceDebugColor ChipmunkDemo::ColorForShape( cpShape *shape, cpDataPointer *data )
{
    if(cpShapeGetSensor(shape)){
        return LAColor(1.0f, 0.1f);
    } else {
        cpBody *body = cpShapeGetBody(shape);

        if(cpBodyIsSleeping(body)){
            return LAColor(0.2f, 1.0f);
        } else if(body->sleeping.idleTime > shape->space->sleepTimeThreshold) {
            return LAColor(0.66f, 1.0f);
        } else {
            uint32_t val = (uint32_t)shape->hashid;

            // scramble the bits up using Robert Jenkins' 32 bit integer hash function
            val = (val+0x7ed55d16) + (val<<12);
            val = (val^0xc761c23c) ^ (val>>19);
            val = (val+0x165667b1) + (val<<5);
            val = (val+0xd3a2646c) ^ (val<<9);
            val = (val+0xfd7046c5) + (val<<3);
            val = (val^0xb55a4f09) ^ (val>>16);

            float r = (float)((val>>0) & 0xFF);
            float g = (float)((val>>8) & 0xFF);
            float b = (float)((val>>16) & 0xFF);

            float max = (float)cpfmax(cpfmax(r, g), b);
            float min = (float)cpfmin(cpfmin(r, g), b);
            float intensity = (cpBodyGetType(body) == CP_BODY_TYPE_STATIC ? 0.15f : 0.75f);

            // Saturate and scale the color
            if(min == max){
                return RGBAColor(intensity, 0.0f, 0.0f, 1.0f);
            } else {
                float coef = (float)intensity/(max - min);
                return RGBAColor(
                    (r - min)*coef,
                    (g - min)*coef,
                    (b - min)*coef,
                    1.0f
                    );
            }
        }
    }
}

void ChipmunkDemo::DrawCircle( cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer *data )
{
    ChipmunkDemo* t = static_cast<ChipmunkDemo*>(*data);

    cpFloat r = radius + 1.0f/ChipmunkDebugDrawPointLineScale;

    cpVect a = {pos.x - r, pos.y - r};
    cpVect aa = {-1.0f, -1.0f};
    cpVect b = {pos.x - r, pos.y + r};
    cpVect bb = {-1.0f, 1.0f};
    cpVect c = {pos.x + r, pos.y + r};
    cpVect cc = {1.0f, 1.0f};
    cpVect d = {pos.x + r, pos.y - r};
    cpVect dd = {1.0f, -1.0f};

    Triangle tri;

    tri.Set(0, a, aa, fill, outline);
    tri.Set(1, b, bb, fill, outline);
    tri.Set(2, c, cc, fill, outline);
    t->triangles.push(tri);
    
    tri.Set(0, a, aa, fill, outline);
    tri.Set(1, c, cc, fill, outline);
    tri.Set(2, d, dd, fill, outline);
    t->triangles.push(tri);

    DrawSegment(pos, cpvadd(pos, cpvmult(cpvforangle(angle), radius - ChipmunkDebugDrawPointLineScale*0.5f)), outline, data);

//    ChipmunkDebugDrawCircle(p, a, r, outline, fill);
}

void ChipmunkDemo::DrawSegment( cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer *data )
{
    //ChipmunkDemo* t = static_cast<ChipmunkDemo*>(*data);

    DrawFatSegment(a, b, 0.0f, color, color, data);
//    ChipmunkDebugDrawSegment(a, b, color);
}

void ChipmunkDemo::DrawFatSegment( cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer *data )
{
    ChipmunkDemo* th = static_cast<ChipmunkDemo*>(*data);

    Triangle tri;

    cpVect n = cpvnormalize(cpvrperp(cpvsub(b, a)));
    cpVect t = cpvrperp(n);

    cpFloat half = 1.0f/ChipmunkDebugDrawPointLineScale;
    cpFloat r = radius + half;
    if(r <= half){
        r = half;
        fill = outline;
    }

    cpVect nw = (cpvmult(n, r));
    cpVect tw = (cpvmult(t, r));
    cpVect v0 = cpvsub(b, cpvadd(nw, tw)); // { 1.0, -1.0}
    cpVect v1 = cpvadd(b, cpvsub(nw, tw)); // { 1.0,  1.0}
    cpVect v2 = cpvsub(b, nw); // { 0.0, -1.0}
    cpVect v3 = cpvadd(b, nw); // { 0.0,  1.0}
    cpVect v4 = cpvsub(a, nw); // { 0.0, -1.0}
    cpVect v5 = cpvadd(a, nw); // { 0.0,  1.0}
    cpVect v6 = cpvsub(a, cpvsub(nw, tw)); // {-1.0, -1.0}
    cpVect v7 = cpvadd(a, cpvadd(nw, tw)); // {-1.0,  1.0}

    cpVect ul = {-1.0f, -1.0f};
    cpVect cl = {-1.0f, 0.0f};
    cpVect bl = {-1.0f, 1.0f};

    cpVect ur = {1.0f, -1.0f};
    cpVect cr = {1.0f, 0.0f};
    cpVect br = {1.0f, 1.0f};

    cpVect uc = {0.0f, -1.0f};
    cpVect bc = {0.0f, 1.0f};

    tri.Set(0, v0, ur, fill, outline); tri.Set(1, v1, br, fill, outline); tri.Set(2, v2, uc, fill, outline);
    th->triangles.push(tri);

    tri.Set(0, v3, bc, fill, outline); tri.Set(1, v1, br, fill, outline); tri.Set(2, v2, uc, fill, outline);
    th->triangles.push(tri);

    tri.Set(0, v3, bc, fill, outline); tri.Set(1, v4, uc, fill, outline); tri.Set(2, v2, uc, fill, outline);
    th->triangles.push(tri);

    tri.Set(0, v3, bc, fill, outline); tri.Set(1, v4, uc, fill, outline); tri.Set(2, v5, bc, fill, outline);
    th->triangles.push(tri);

    tri.Set(0, v6, ul, fill, outline); tri.Set(1, v4, uc, fill, outline); tri.Set(2, v5, bc, fill, outline);
    th->triangles.push(tri);

    tri.Set(0, v6, ul, fill, outline); tri.Set(1, v7, bl, fill, outline); tri.Set(2, v5, bc, fill, outline);
    th->triangles.push(tri);
//    ChipmunkDebugDrawFatSegment(a, b, r, outline, fill);
}

void ChipmunkDemo::DrawPolygon( int count, const cpVect *verts, cpFloat radius, cpSpaceDebugColor outline, cpSpaceDebugColor fill, cpDataPointer *data )
{
    ChipmunkDemo* t = static_cast<ChipmunkDemo*>(*data);

    struct ExtrudeVerts {cpVect offset, n;};
    size_t bytes = sizeof(ExtrudeVerts)*count;
    ExtrudeVerts *extrude = (ExtrudeVerts *)alloca(bytes);
    memset(extrude, 0, bytes);

    for(int i=0; i<count; i++){
        cpVect v0 = verts[(i-1+count)%count];
        cpVect v1 = verts[i];
        cpVect v2 = verts[(i+1)%count];

        cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v1, v0)));
        cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v2, v1)));

        cpVect offset = cpvmult(cpvadd(n1, n2), 1.0/(cpvdot(n1, n2) + 1.0f));
        extrude[i].n = n2;
        extrude[i].offset = offset;
    }

    cpFloat inset = -cpfmax(0.0f, 1.0f/ChipmunkDebugDrawPointLineScale - radius);
    for(int i=0; i<count-2; i++)
    {
        cpVect v0 = cpvadd(verts[  0], cpvmult(extrude[  0].offset, inset));
        cpVect v1 = cpvadd(verts[i+1], cpvmult(extrude[i+1].offset, inset));
        cpVect v2 = cpvadd(verts[i+2], cpvmult(extrude[i+2].offset, inset));

        Triangle tri;

        tri.Set(0, v0, fill, outline);
        tri.Set(1, v1, fill, outline);
        tri.Set(2, v2, fill, outline);
        t->triangles.push(tri);
    }

    cpFloat outset = 1.0f/ChipmunkDebugDrawPointLineScale + radius - inset;
    for(int i=0, j=count-1; i<count; j=i, i++){
        cpVect vA = verts[i];
        cpVect vB = verts[j];

        cpVect nA = extrude[i].n;
        cpVect nB = extrude[j].n;

        cpVect offsetA = extrude[i].offset;
        cpVect offsetB = extrude[j].offset;

        cpVect innerA = cpvadd(vA, cpvmult(offsetA, inset));
        cpVect innerB = cpvadd(vB, cpvmult(offsetB, inset));

        // Admittedly my variable naming sucks here...
        cpVect inner0 = innerA;
        cpVect inner1 = innerB;
        cpVect outer0 = cpvadd(innerA, cpvmult(nB, outset));
        cpVect outer1 = cpvadd(innerB, cpvmult(nB, outset));
        cpVect outer2 = cpvadd(innerA, cpvmult(offsetA, outset));
        cpVect outer3 = cpvadd(innerA, cpvmult(nA, outset));

        cpVect n0 = nA;
        cpVect n1 = nB;
        cpVect offset0 = offsetA;

        Triangle tri;

        tri.Set(0, inner0, fill, outline);
        tri.Set(1, inner1, fill, outline);
        tri.Set(2, outer1, n1, fill, outline);
        t->triangles.push(tri);

        tri.Set(0, inner0, fill, outline);
        tri.Set(1, outer0, n1, fill, outline);
        tri.Set(2, outer1, n1, fill, outline);
        t->triangles.push(tri);

        tri.Set(0, inner0, fill, outline);
        tri.Set(1, outer0, n1, fill, outline);
        tri.Set(2, outer2, offset0, fill, outline);
        t->triangles.push(tri);

        tri.Set(0, inner0, fill, outline);
        tri.Set(1, outer2, offset0, fill, outline);
        tri.Set(2, outer3, n0, fill, outline);
        t->triangles.push(tri);
    }
//    ChipmunkDebugDrawPolygon(count, verts, r, outline, fill);
}

void ChipmunkDemo::DrawDot( cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer *data )
{
    ChipmunkDemo* t = static_cast<ChipmunkDemo*>(*data);

    cpFloat r = size*0.5/ChipmunkDebugDrawPointLineScale;
    cpVect a = {pos.x - r, pos.y - r};
    cpVect aa = {-1.0f, -1.0f};
    cpVect b = {pos.x - r, pos.y + r};
    cpVect bb = {-1.0f, 1.0f};
    cpVect c = {pos.x + r, pos.y + r};
    cpVect cc = {1.0f, 1.0f};
    cpVect d = {pos.x + r, pos.y - r};
    cpVect dd = {1.0f, -1.0f};

    Triangle tri;

    tri.Set(0, a, aa, color, color);
    tri.Set(1, b, bb, color, color);
    tri.Set(2, c, cc, color, color);
    t->triangles.push(tri);

    tri.Set(0, a, aa, color, color);
    tri.Set(1, c, cc, color, color);
    tri.Set(2, d, dd, color, color);
    t->triangles.push(tri);
//    ChipmunkDebugDrawDot(size, pos, color);
}

void ChipmunkDemo::DrawBB( cpBB bb, cpSpaceDebugColor outlineColor, cpDataPointer *data )
{
    cpVect verts[] = {
        cpv(bb.r, bb.b),
        cpv(bb.r, bb.t),
        cpv(bb.l, bb.t),
        cpv(bb.l, bb.b),
    };
    DrawPolygon(4, verts, 0.0f, outlineColor, LAColor(0, 0), data);
}

ChipmunkDemo::TouchJoint::TouchJoint()
    : mouse_body(NULL)
    , mouse_joint(NULL)
    , space(NULL)
{
}

cpSpaceDebugColor ChipmunkDemo::LAColor( float l, float a )
{
    cpSpaceDebugColor color = {l, l, l, a};
    return color;
}

cpSpaceDebugColor ChipmunkDemo::RGBAColor( float r, float g, float b, float a )
{
    cpSpaceDebugColor color = {r, g, b, a};
    return color;
}

cpFloat ChipmunkDemo::frand( void )
{
    return (cpFloat)rand()/(cpFloat)RAND_MAX;
}

cpVect ChipmunkDemo::frand_unit_circle()
{
    cpVect v = cpv(frand()*2.0f - 1.0f, frand()*2.0f - 1.0f);
    return (cpvlengthsq(v) < 1.0f ? v : frand_unit_circle());
}

void ChipmunkDemo::Triangle::Set( int index, const cpVect& vertex, const cpVect& aa_coord, const cpSpaceDebugColor& fill, const cpSpaceDebugColor& outline )
{
    v[index].vertex.x = (float)vertex.x; v[index].vertex.y = (float)vertex.y;
    v[index].aa_coord.x = (float)aa_coord.x; v[index].aa_coord.y = (float)aa_coord.y;
    v[index].fill.r = fill.r; v[index].fill.g = fill.g; v[index].fill.b = fill.b; v[index].fill.a = fill.a;
    v[index].outline.r = outline.r; v[index].outline.g = outline.g; v[index].outline.b = outline.b; v[index].outline.a = outline.a;
}

void ChipmunkDemo::Triangle::Set(int index, const cpVect& vertex, const cpSpaceDebugColor& fill, const cpSpaceDebugColor& outline)
{
    cpVect v0 = {0.0f, 0.0f};
    Set(index, vertex, v0, fill, outline);
}
