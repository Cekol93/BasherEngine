#ifndef TESTENTITIE_H
#define TESTENTITIE_H
#include "engine.h"

class testentitie: public module
{
    public:
        testentitie();
        ~testentitie();
        void synchro();
        void work();
        void draw();
        float ang,x,y,xspeed,yspeed;

        float* front_buffer;
        float* back_buffer;
        float* edited_buffer;
        float* drawed_buffer;
        bool visible;
};

#endif // TESTENTITIE_H
