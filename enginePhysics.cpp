#include "enginePhysics.h"
#include "engine.h"


enginePhysics* physicsController = NULL;
enginePhysics* M_getPhysics(){return physicsController;}

int i = 0;

enginePhysicsObject::enginePhysicsObject(){

    M_getPhysics()->newPhysicsObjects.push_back(this);
    input = false;
    physicsObjectCriticalReDefinition = true;

    inputPosition.Clear();
    inputForce.Clear();
    position.Clear();

    physicsObjectMaterial = (char)0;

    body = nullptr;
    rotation = 0.0f;

}


void enginePhysicsObject::physicsSetPosition(Vec2 pos) {
    input = true;
    inputPosition = pos;
}

char physicsObjectLogicMode; // 0 - MAP OBJECT GROUND STATIC  1 - MAP OBJECT GROUND DYNAMIC 10 - GENERIC DYNAMIC OBJECT
void enginePhysicsObject::physicsSetLogicMode(char lm) {
    //input = true;
    physicsObjectCriticalReDefinition = true;
    physicsObjectLogicMode = lm;
}

 // FOR MAP ELEMENTS OR UNCOMMON SHAPED GEOMETRY
void enginePhysicsObject::physicsSetVertexPositions(Vec2 a, Vec2 b, Vec2 c, Vec2 d) {

    //input = true;
    physicsObjectCriticalReDefinition = true;
    physicsQuadVertRealPositions[0] = a;
    physicsQuadVertRealPositions[1] = b;
    physicsQuadVertRealPositions[2] = c;
    physicsQuadVertRealPositions[3] = d;
}




enginePhysics::enginePhysics(){

    M_getEngine()->registerModule( this , "Engine Physics" );
    physicsController = this;

    newPhysicsObjects.clear();
    PhysicsObjects.clear();

    gravity.Set(0.0f,10.5f);
	World = new b2World(gravity);
	World->SetAllowSleeping(true);

    /*
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(640.0f/50.0f, 1000.0f/50.0f);
	groundBody  = World->CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(5000.0f/50.0f, 40.0f/50.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);
    */
    
 }

enginePhysics::~enginePhysics(){

}
void enginePhysics::lockThread() { /*m.lock();*/ }
void enginePhysics::unlockThread() { /*m.unlock();*/ }

void enginePhysics::synchro() {

    for (int i=0;i<newPhysicsObjects.size();i++){
        PhysicsObjects.push_back(newPhysicsObjects[i]);
    }
    newPhysicsObjects.clear();

}

void enginePhysics::work() {
    if (!World) return;

    for (int i=0;i< PhysicsObjects.size();i++){
        enginePhysicsObject* po = PhysicsObjects[i];
        if (!po->body){

            /*
            po->physicsObjectLogicMode;
            po->physicsObjectMaterial;
            po->physicsQuadVertRealPositions[];
            */
            if (po->physicsObjectLogicMode == 0) {

                Vec2 centerPoint;
                centerPoint.x = (po->physicsQuadVertRealPositions[0].x + po->physicsQuadVertRealPositions[1].x + po->physicsQuadVertRealPositions[2].x + po->physicsQuadVertRealPositions[3].x) * 0.25f;
                centerPoint.y = (po->physicsQuadVertRealPositions[0].y + po->physicsQuadVertRealPositions[1].y + po->physicsQuadVertRealPositions[2].y + po->physicsQuadVertRealPositions[3].y) * 0.25f;

                b2BodyDef groundBodyDef;
                //groundBodyDef.angularDamping = 1.0f;
                //groundBodyDef.linearDamping = 1.0f;
                //groundBodyDef.gravityScale = 0.2f;
                //if (DynamicBody) groundBodyDef.type = b2_dynamicBody; else   
                groundBodyDef.type = b2_staticBody;

                groundBodyDef.position.Set(centerPoint.x / 50.0f , centerPoint.y / 50.0f );

                po->body = World->CreateBody(&groundBodyDef);

                //PBody -> SetUserData(NULL);

                b2Vec2 vertices[4];
                for (int a = 0; a < 4; a++) 
                    vertices[a].Set(( po->physicsQuadVertRealPositions[a].x - centerPoint.x) / 50.0f , (po->physicsQuadVertRealPositions[a].y - centerPoint.y) / 50.0f );
                b2PolygonShape polygon;
                polygon.Set(vertices, 4);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &polygon;
                fixtureDef.friction = 1.0f;
                /*if (DynamicBody) {
                    fixtureDef.density = 30.0f;
                    fixtureDef.friction = 1.0f;
                    fixtureDef.restitution = 0.0f;

                }*/

                //if (DynamicBody) fixtureDef.filter.categoryBits = CATEGORY_MOVEABLE;
                //else fixtureDef.filter.categoryBits = CATEGORY_GROUND;
                //fixtureDef.filter.maskBits = CATEGORY_GROUND | CATEGORY_PLAYER | CATEGORY_MOVEABLE | CATEGORY_MONSTER | CATEGORY_FLAKE;
                //CATEGORY_GROUND ||
                po->body->CreateFixture(&fixtureDef);
            }

            if (po->physicsObjectLogicMode == 10) {

                b2BodyDef bodyDef;
                bodyDef.type = b2_dynamicBody;
                bodyDef.position.Set(po->inputPosition.x / 50.0f, po->inputPosition.y / 50.0f);
                bodyDef.gravityScale = 1.0f;

                po->body = World->CreateBody(&bodyDef);

                b2PolygonShape BoxShape;
                BoxShape.SetAsBox(0.5f, 0.5f);

                b2FixtureDef BoxFixtureDef;
                BoxFixtureDef.shape = &BoxShape; //this is a pointer to the shape above
                BoxFixtureDef.friction = 1.0f;
                BoxFixtureDef.density = 0.75f;
                po->body->CreateFixture(&BoxFixtureDef); //add a fixture to the body
            }
            

            
        }
        if (po->body != nullptr) {
            if (po->physicsObjectCriticalReDefinition) {
                World->DestroyBody( po->body );
                po->body = nullptr;
                po->physicsObjectCriticalReDefinition = false;
            }
        }
    }

    World->Step( (float)(deltaTime*0.001f) ,2,2);



    //m.lock();
    for (int i=0;i< PhysicsObjects.size();i++){
        enginePhysicsObject* po = PhysicsObjects[i];
        if (po != nullptr) {
            po->mLock.lock();
            if (po->body) {
                po->position = Vec2(po->body->GetPosition().x * 50.0f, po->body->GetPosition().y * 50.0f);
                po->rotation = -po->body->GetAngle() * 180.0f / b2_pi;

                if (po->inputForce.Length() > 0) {
                    po->body->ApplyForceToCenter(b2Vec2(po->inputForce.x, po->inputForce.y), true);
                    po->inputForce.Clear();
                }
                //cout<<_rb->rotation<<endl;
            }
            po->mLock.unlock();
        }
    }
    //m.unlock();
}

void enginePhysics::draw(){
}
