#include "engine.h"

#include "engineRenderer.h"
#include "engineRendererSimpleSprite.h"

#include "engineActorController.h"
#include "actorTest.h"

engineActorController* AC_CON = nullptr;
engineActorController* M_getActors() {return AC_CON;}

 Actor::Actor(){
        engineActorController* actCon = M_getActors();
    if (actCon != nullptr)
        actCon->newActorsContainer.push_back(this);
    else
        delete (Actor*)this;

    TYPE_ID = (char)0;


    toDestroy = false;
    informOthersAboutDestroy = false;

}

Actor::~Actor(){}

engineActorController::engineActorController()
{
    M_getEngine()->registerModule( this , "engine Actor Controller" );
    AC_CON = this;
    visible = true;

    //for (int i = 0; i < 250; i++)
    //    new actorTest();
}

engineActorController::~engineActorController()
{

}

void engineActorController::synchro() {

}
void engineActorController::work() {

    if (this->newActorsContainer.size() + this->actorsContainer.size() <=0) return;

            int NewAc,ExistedAc,DestroyedAc;
            NewAc= 0;
            ExistedAc = 0;
            DestroyedAc = 0;
            NewAc =  newActorsContainer.size();

            for( std::size_t i = 0; i < newActorsContainer.size(); i++ )
                actorsContainer.push_back(newActorsContainer[i]);

            newActorsContainer.clear();

            for( std::size_t i = 0; i < actorsContainer.size(); i++ )
            {
                Actor *AC = (Actor*)actorsContainer[i];
                if (!AC->toDestroy)
                {
                    AC->Step();
                    ExistedAc++;
                }
                else
                {
                    if (AC->informOthersAboutDestroy) informActorsAboutDestroy(AC);
                    actorsContainer.erase (actorsContainer.begin()+i);
				    delete AC;
				    DestroyedAc++;
                }
            }
            /*
            float k = 0;
            for (int i = 0; i < 999999; i++) {
                int j = sin(cos(i));
                k += (float)j * 0.00001;
            }
            cout << k << endl;
            */
}

void engineActorController::informActorsAboutDestroy(Actor* DA){

}

void engineActorController::draw() {}
