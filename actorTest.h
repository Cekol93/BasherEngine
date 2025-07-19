#ifndef AC_TEST_H
#define AC_TEST_H
#include "engine.h"
#include "engineMath.h"
#include "engineActorController.h"
#include "enginePhysics.h"

class actorTest: public Actor
{
    public:
        enginePhysicsObject* RB;
        int Frame;
        actorTest();
        ~actorTest();
        void Step();
        Vec2 Position = Vec2(0,0);
        Vec2 Velocity = Vec2(0,0);
        //void Information_About_destroy(Actor* DestroyedActorPointer) {}
};

#endif // AC_TEST_H
