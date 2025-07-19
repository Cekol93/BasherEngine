#include "engineActorController.h"
#include "actorTest.h"
#include "engineRendererSimpleSprite.h"

#include "engineKeyboard.h"
#include "engineMouse.h"
//#include "engineJ.h"

#include "engineRenderer.h"
#include "enginePhysics.h"

actorTest::actorTest()
{
    //ctor
    Velocity = Vec2(Random(-900,900),Random(-900,900));
    Position = Vec2(Random(-2000,2000),Random(-2000,2000));
    Frame = (int)ceil(Random(0,256));

    RB = NULL;
    //cout<<Frame<<endl;
}

actorTest::~actorTest()
{
    //dtor
}

void actorTest::Step()
{
    float deltaT = M_getActors()->deltaTime;
    Velocity.Clear();
    engineKeyboard* K = M_getKeyboard();
    if (K)
    if (K->isAnyKey()){ //Velocity.x*=0.95f; Velocity.y*=0.95f;
            Vec2 Vel;
            if (K->key('A')) Velocity.x-=200.5f;
            if (K->key('D')) Velocity.x+=200.5f;
            if (K->key('W')) Velocity.y-=200.5f;
            if (K->key('S')) Velocity.y+=200.5f;
    }

    if (Position.x+(Velocity.x*deltaT)>4000 || Position.x+(Velocity.x*deltaT)<-4000) Velocity.x=-Velocity.x*0.85f;
    if (Position.y+(Velocity.y*deltaT)>4000 || Position.y+(Velocity.y*deltaT)<-4000) Velocity.y=-Velocity.y*0.85f;

        Vec2 Target =  M_getMouse()->getPosition();//= Vec2(get_joystick()->Axis_X()*1000,get_joystick()->Axis_Y()*1000);
    //    if ( sqrt( (Position.x*Position.x -Target.x*Target.x) + (Position.y*Position.y -Target.y*Target.y) ) <2500){
        float Angle = 180-( atan2( Position.y-Target.y, Position.x-Target.x ) / M_PI * 180 );
       // float Distance = sqrt( (Position.x*Position.x + (Position.y*Position.y );
       // Distance = (2500.0f-Distance)/2500.0f;
        //Distance*=Distance;
        float Distance = 400.0f;
        Velocity.x*=0.995f;
        Velocity.y*=0.995f;

        Velocity.x += cos(Angle* M_PI/180.0f ) * Distance;
        Velocity.y +=-sin(Angle* M_PI/180.0f ) * Distance;

    if (!RB) {
        RB = new enginePhysicsObject();
        RB->physicsSetPosition(Position);
        RB->physicsSetLogicMode(10);
    }
    else{
        RB->mLock.lock();
        //M_getPhysics()->lockThread();
        Position = RB->position;
        RB->inputForce = Velocity;
        RB->mLock.unlock();
        //M_getPhysics()->unlockThread();

        engineRendererSimpleSprite* S = get_sprite("star");
        if (S != nullptr)
            S->DrawAsSprite(Frame,Position,Vec2(1.75,1.75),RB->rotation);
    }
}
