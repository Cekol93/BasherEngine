#ifndef PHYSICS_H
#define PHYSICS_H
#include "engine.h"
#include "engineMath.h"
#include <box2d/Box2D.h>

    class SimpleRayCastCallback : public b2QueryCallback {
      public:
          std::vector<b2Body*> foundBodies;

          bool ReportFixture(b2Fixture* fixture) {
              foundBodies.push_back( fixture->GetBody() );
              return true;//keep going to find all fixtures in the query area
          }
      };

    class SimpleQueryCallback : public b2QueryCallback {
    public:
      std::vector<b2Fixture*> foundFixtures;

      bool ReportFixture(b2Fixture* fixture) {
          foundFixtures.push_back( fixture );
          return true;//keep going to find all fixtures in the query area
      }
    };

    class DynamicB_QueryCallback : public b2QueryCallback {
    public:
      std::vector<b2Fixture*> foundFixtures;

      bool ReportFixture(b2Fixture* fixture) {
        if (fixture->GetBody()->GetMass()>0)
            foundFixtures.push_back( fixture );
          return true;//keep going to find all fixtures in the query area
      }
    };

    class StaticB_QueryCallback : public b2QueryCallback {
    public:
      std::vector<b2Fixture*> foundFixtures;

      bool ReportFixture(b2Fixture* fixture) {
        if (fixture->GetBody()->GetMass()<=0)
            foundFixtures.push_back( fixture );
          return true;//keep going to find all fixtures in the query area
      }
    };

class enginePhysicsObject
{
    public:
        enginePhysicsObject();

        bool input; // do we got input data ?!
        bool physicsObjectCriticalReDefinition; // Do we need reset (destroy and recreate) phys body?


        char physicsObjectMaterial;

        void physicsSetPosition(Vec2);

        char physicsObjectLogicMode; // 0 - MAP OBJECT GROUND STATIC  1 - MAP OBJECT GROUND DYNAMIC 10 - GENERIC DYNAMIC OBJECT
        void physicsSetLogicMode(char); // 0 - MAP OBJECT GROUND STATIC  1 - MAP OBJECT GROUND DYNAMIC 10 - GENERIC DYNAMIC OBJECT

        Vec2 physicsQuadVertRealPositions[4]; // FOR MAP ELEMENTS OR UNCOMMON SHAPED GEOMETRY
        void physicsSetVertexPositions( Vec2 , Vec2 , Vec2 , Vec2 );

        
        Vec2 inputPosition;
        Vec2 inputForce;

        b2Body* body;

        Vec2 position;
        Vec2 velocity;
        float rotation;

        mutex mLock;
};

class enginePhysics: public module
{
    public:
        std::vector< enginePhysicsObject* > newPhysicsObjects;
        std::vector< enginePhysicsObject* > PhysicsObjects;
        b2Vec2 gravity;
        b2World* World;
        b2Body* groundBody;

        enginePhysics();
        ~enginePhysics();
        void draw();
        void synchro();
        void work();

        void lockThread();
        void unlockThread();
};

enginePhysics* M_getPhysics();

#endif // PHYSICS_H
