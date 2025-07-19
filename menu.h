#ifndef MENU_H
#define MENU_H

#include "engine.h"
#include "math_utl.h"
#include "draw_buffer.h"



class menu: public module
{
    public:
        menu();
        ~menu();
        void load();
        void synchro();
        void work();
        void draw();

        void find_maps();

        //FPS COUNTER;
        int DrawFrames;

        int CountedFrames;
        float OneSecTimer;
        //MENU STUFF...
        bool active;
        float alpha; // smooth menu fade...
        int selector; // cursor position
        int selector_max; // maximum cursor position
        int state; // menu action state
        bool selected; // enter lock
        float play_sound; // do i shoud play a sound?
        Vec2 MousePos;
        float ShowMouse;
        //DRAWING STUFF...
        void draw_option(Vec2 , Vec2 , string , int , int);
        //float *TexCoords; // preloaded texture charset

        vector<string> Maps;

        draw_buffer* Chars;
        bool visible;
        GLuint GL_Texture;

};

#endif // MENU_H
