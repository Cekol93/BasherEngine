#ifndef LEVEL_BLOCK_H
#define LEVEL_BLOCK_H
#include "engine.h"
#include "draw_buffer.h"

class level_block: public module
{
    public:
        GLuint GL_Texture;
        //GLuint GL_VBO_T;
        //GLuint GL_VBO_V;

        draw_buffer* myQuad;

        bool Active;
        bool visible;

        Vec2 TexCoords[4];              //4 punkty pozycji tekstury
        Vec2 VisibleVertPOS[4] ;               //4 punkty pozycji vectorow (Do edycji/zapisu)
        Vec2 Vertex_POS[4] ;               //4 punkty pozycji vectorow (Widzialnych)

        float* front_bufferV;
        float* front_bufferT;
        float* back_bufferV;
        float* back_bufferT;
        float* edited_bufferV;
        float* edited_bufferT;
        float* drawed_bufferV;
        float* drawed_bufferT;

        float* color;

        float ParallaxMultipler;   //far 1-0 near
        //Textureunit();

        level_block();
        ~level_block();
        void load();
        void draw();
        void synchro();
        void work();

        void SetVertexPos(float a,float b,float c,float d, float e, float f,float g,float h);
        void SetTexCoords(float a,float b,float c,float d, float e, float f,float g,float h);

};


#endif // LEVEL_BLOCK_H
