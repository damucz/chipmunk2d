#include "demo.h"

#include "Bench.h"
#include "Buoyancy.h"
#include "Chains.h"
#include "ContactGraph.h"
#include "ContactPoints.h"
#include "Convex.h"
#include "Crane.h"
#include "GJK.h"
#include "Joints.h"
#include "LogoSmash.h"
#include "OneWay.h"
#include "Planet.h"
#include "Player.h"
#include "Plink.h"
#include "Pump.h"
#include "PyramidStack.h"
#include "PyramidTopple.h"
#include "Query.h"
#include "Shatter.h"
#include "Slice.h"
#include "Smooth.h"
#include "Springies.h"
#include "Sticky.h"
#include "Tank.h"
#include "TheoJansen.h"
#include "Tumble.h"
#include "Unicycle.h"

#include <IwGx.h>
#include <s3eKeyboard.h>
#include <s3ePointer.h>

enum Demo
{
    DFirst,

    DBench,
    DBuoyancy,
    DChains,
    DContactGraph,
    DContactPoints,
    DConvex,
    DCrane,
    DGJK,
    DJoints,
    DLogoSmash,
    DOneWay,
    DPlanet,
    DPlayer,
    DPlink,
    DPump,
    DPyramidStack,
    DPyramidTopple,
    DQuery,
    DShatter,
    DSlice,
    DSmooth,
    DSpringies,
    DSticky,
    DTank,
    DTheoJansen,
    DTumble,
    DUnicycle,

    DLast,
};

ChipmunkDemo *demo = NULL;
Demo currentDemo = DPyramidStack;
static const int viewSize = 650;
uint32 screenWidth = 0;
uint32 screenHeight = 0;
int32 w2 = 0;
int32 h2 = 0;

// mouse right-click simulation
int rightClick = -1;
int rightClickAreaSize = 100;

// fixed time-step
int64 start = s3eTimerGetMs();
double accumulator = 0;

ChipmunkDemo* CreateDemo(Demo d)
{
    ChipmunkDemo* demo = NULL;

    switch(d)
    {
    case DPyramidStack: demo = new PyramidStack(); break;
    case DPyramidTopple: demo = new PyramidTopple(); break;
    case DBench: demo = new Bench(); break;
    case DBuoyancy: demo = new Buoyancy(); break;
    case DChains: demo = new Chains(); break;
    case DContactGraph: demo = new ContactGraph(); break;
    case DContactPoints: demo = new ContactPoints(); break;
    case DConvex: demo = new Convex(); break;
    case DCrane: demo = new Crane(); break;
    case DGJK: demo = new GJK(); break;
    case DJoints: demo = new Joints(); break;
    case DLogoSmash: demo = new LogoSmash(); break;
    case DOneWay: demo = new OneWay(); break;
    case DPlanet: demo = new Planet(); break;
    case DPlayer: demo = new Player(); break;
    case DPlink: demo = new Plink(); break;
    case DPump: demo = new Pump(); break;
    case DQuery: demo = new Query(); break;
    case DShatter: demo = new Shatter(); break;
    case DSlice: demo = new Slice(); break;
    case DSmooth: demo = new Smooth(); break;
    case DSpringies: demo = new Springies(); break;
    case DSticky: demo = new Sticky(); break;
    case DTank: demo = new Tank(); break;
    case DTheoJansen: demo = new TheoJansen(); break;
    case DTumble: demo = new Tumble(); break;
    case DUnicycle: demo = new Unicycle(); break;
    }

    if (demo)
        demo->Init();

    return demo;
}

void NextDemo()
{
    int current = currentDemo + 1;
    if (current == DLast)
        current = DFirst + 1;

    currentDemo = (Demo)current;
    if (demo)
        delete demo;
    demo = CreateDemo(currentDemo);

    start = s3eTimerGetMs();
    accumulator = 0;
}

void PrevDemo()
{
    int current = currentDemo - 1;
    if (current == DFirst)
        current = DLast - 1;

    currentDemo = (Demo)current;
    if (demo)
        delete demo;
    demo = CreateDemo(currentDemo);

    start = s3eTimerGetMs();
    accumulator = 0;
}

cpVect ScreenToSpace(int32 x, int32 y)
{
    cpVect pos;
    if (screenWidth < screenHeight)
    {
        pos.x = (cpFloat)x / screenWidth * w2*2 - w2;
        pos.y = - ((cpFloat)y / screenHeight * h2*2 - h2);
    }

    //IwTrace(X, ("%lf %lf", pos.x, pos.y));

    return pos;
}

void ProcessTouch(uint32 id, uint32 x, uint32 y, ChipmunkDemo::TouchState state)
{
    if (state == ChipmunkDemo::TOUCH_START)
    {
        if (x > screenWidth - rightClickAreaSize && y > screenHeight - rightClickAreaSize)
        {
            rightClick = id;
            return;
        }
    }
    else if (state == ChipmunkDemo::TOUCH_MOVE)
    {

    }
    else /* if (state == ChipmunkDemo::TOUCH_END) */
    {
        if (id == rightClick)
        {
            rightClick = -1;
            return;
        }
    }

    if (y < 50)
    {
        if (state == ChipmunkDemo::TOUCH_END)
        {
            if (x < screenWidth / 2)
                PrevDemo();
            else
                NextDemo();
        }
        return;
    }

    if (demo)
        demo->ProcessTouch(id, ScreenToSpace(x, y), state, rightClick >= 0);
}

void MultiTouchButtonCB(s3ePointerTouchEvent* event)
{
    ProcessTouch(event->m_TouchID, event->m_x, event->m_y, event->m_Pressed ? ChipmunkDemo::TOUCH_START : ChipmunkDemo::TOUCH_END);
}

void MultiTouchMotionCB(s3ePointerTouchMotionEvent* event)
{
    ProcessTouch(event->m_TouchID, event->m_x, event->m_y, ChipmunkDemo::TOUCH_MOVE);
}

void SingleTouchButtonCB(s3ePointerEvent* event)
{
    ProcessTouch(0, event->m_x, event->m_y, event->m_Pressed ? ChipmunkDemo::TOUCH_START : ChipmunkDemo::TOUCH_END);
}

void SingleTouchMotionCB(s3ePointerMotionEvent* event)
{
    ProcessTouch(0, event->m_x, event->m_y, ChipmunkDemo::TOUCH_MOVE);
}

void Init()
{
	IwGxInit();
    IwGxLightingOff();

    IwResManagerInit();

    screenWidth = IwGxGetScreenWidth();
    screenHeight = IwGxGetScreenHeight();

    if (screenWidth < screenHeight)
    {
        // portrait
        w2 = viewSize / 2;
        h2 = screenHeight * w2 / screenWidth;
    }
    else
    {
        // landscape
        h2 = viewSize / 2;
        w2 = screenWidth * h2 / screenHeight;
    }
    IwGxSetOrtho(-w2, w2, h2, -h2, 1.0f, 1000.0f);

    demo = CreateDemo(currentDemo);

    if (s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE)) {
        s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouchButtonCB, NULL);
        s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MultiTouchMotionCB, NULL);
    } else {
        s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)SingleTouchButtonCB, NULL);
        s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)SingleTouchMotionCB, NULL);
    }
}

void ShutDown()
{
    if (s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE)) {
        s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouchButtonCB);
        s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MultiTouchMotionCB);
    } else {
        s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)SingleTouchButtonCB);
        s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)SingleTouchMotionCB);
    }

    if (demo)
    {
        demo->Destroy();
        delete demo;
    }

    IwResManagerTerminate();
    IwGxTerminate();
}

bool Update(double dt)
{  
    if (demo)
        demo->Update(dt);

	return true;
}

void RenderRightClickSimulation()
{
    const char *str;
    
    if (rightClick >= 0)
        str = "`x0000ffRIGHT \nCLICK!";
    else
        str = "hold \nhere \nfor \nright \nclick";

    IwGxPrintString(screenWidth - rightClickAreaSize, screenHeight - rightClickAreaSize, str);
}

void RenderName()
{
    if (!demo) return;

    const char *name = demo->GetName();
    int length = strlen(name) * IwGxPrintGetScale() * 6;

    char buf[256];
    snprintf(buf, sizeof(buf), "`1%s", name);

    IwGxPrintString((screenWidth - length) / 2, 20, buf);
}

void RenderNavigation()
{
    IwGxPrintString(2 * IwGxPrintGetScale() * 6, 20, "<");
    IwGxPrintString(screenWidth - 3 * IwGxPrintGetScale() * 6, 20, ">");
}

void Render()
{
    // Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

    if (demo)
        demo->Draw();

    IwGxFlush();
    IwGxPrintSetScale(2);
    RenderRightClickSimulation();
    RenderNavigation();
    RenderName();

    IwGxFlush();
    IwGxSwapBuffers();

    IwGxPrintSetScale(1);
}

S3E_MAIN_DECL void IwMain()
{
    Init();

    // Set screen clear colour
    IwGxSetColClear(0, 0, 0, 0xff);
    IwGxPrintSetColour(128, 128, 128);

    start = s3eTimerGetMs();
    accumulator = 0;

    while (1)
    {
        s3eDeviceYield(0);
        s3eKeyboardUpdate();
        s3ePointerUpdate();

        uint64 end = s3eTimerGetMs();
        uint64 diff = end-start;
        start = end;
        if (diff > 200) diff = 200;

        double fixed_dt = demo ? demo->GetTimestep() : 1.0 / 60.0;
        //double fixed_dt = 1.0 / 60.0;
        bool result = true;
        for (accumulator += diff / 1000.0; accumulator > fixed_dt; accumulator -= fixed_dt)
        {
            result = Update(fixed_dt);
            if (!result) break;
        }

        if  (
            ((result == false) ||
            (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN) ||
            (s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN) ||
            (s3eDeviceCheckQuitRequest()))
            )
            break;

        Render();

        s3eDeviceYield(std::max(0, (int32)((fixed_dt - accumulator) * 1000.0)));
    }

    ShutDown();
}
