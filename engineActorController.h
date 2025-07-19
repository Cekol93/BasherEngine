#ifndef AC__CONTROLLER_H
#define AC__CONTROLLER_H
#include "engine.h"


class Actor
{
    public:
        unsigned char TYPE_ID;
        bool toDestroy;                  // Czy usunac obiekt? 1-tak / 0-nie
        bool informOthersAboutDestroy;
        Actor();                         // konstruktor
        virtual ~Actor();                    // destruktor
        virtual void Step() {}
        virtual void informationAboutDestroy(Actor* DestroyedActorPointer) {}

};

class engineActorController : public module
{
    public:
        engineActorController();
        ~engineActorController();
        void synchro();
        void work();
        void draw();
        void register_actor( Actor * NM , std::string Name);
        void informActorsAboutDestroy(Actor* DA);


        bool visible;
        std::vector<Actor*> actorsContainer; // Kontener z naszymi Actorami
        std::vector<Actor*> newActorsContainer; // Kontener z naszymi NOWYMI Actorami
};

engineActorController* M_getActors();
#endif // AC__CONTROLLER_H
