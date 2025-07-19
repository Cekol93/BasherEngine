#ifndef SIMPLESPRITE_H
#define SIMPLESPRITE_H
#include "engine.h"
#include "engineRendererDrawBuffer.h"

struct Sprite{
    int Frame;
    Vec2 Position;
    Vec2 Scale;
    float Angle;
    void Clear(){Frame=0; Position.Clear(); Scale.Clear(); Angle=0;};
    void Set(int Fr , Vec2 Pos ,Vec2 Sc , float Ang)
    {
    Frame = Fr; Position=Pos; Scale=Sc; Angle=Ang;
    };
};

class engineRendererSimpleSprite: public module
{
    public:
        engineRenderDrawBuffer* Buf;
        GLuint GL_Texture;
        GLuint GL_VBO_T;
        GLuint GL_VBO_V;

        std::string  Filename;
        bool Loaded;

        int Width;
        int Height;
        int  Anim_Col;      //columns
        int  Anim_Row;       //row
        int  Anim_Frames;
        Vec2 ImageCenter;

        float *TexCoords;
        float *Vertexes;

        float *TexCoords_buffer;
        float *Vertexes_buffer;

        float MaxDistance;

        unsigned int SpritesToDraw_IN;                  //Count of sprites in buffer (imput calls)
        unsigned int SpritesToDraw_OUT;                  //Count of sprites in buffer (editing calls)
        unsigned int SpritesToDraw_VBO;                 //Count of sprites in buffer (in vbo to draw...)
        Sprite SpritesBuffer_IN[20000];           //Sprites buffer;
        Sprite SpritesBuffer_OUT[20000];           //Sprites buffer;

        //Textureunit();
        GLuint GetTexture();
        engineRendererSimpleSprite(std::string _name,std::string _file, int Width,int Height, int Xorgin,int Yorgin,int Anim_Columns,int Anim_Rows);
        ~engineRendererSimpleSprite();
        void DrawAsSprite( int Frame , Vec2 Position ,Vec2 Scale , float Angle);
        void UpdateParticles();
        void draw();
        void synchro();
        void work();
};

engineRendererSimpleSprite* get_myspr();
engineRendererSimpleSprite* get_sprite(std::string _name);

#endif // SIMPLESPRITE_H
