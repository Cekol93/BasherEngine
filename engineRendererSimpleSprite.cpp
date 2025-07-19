#include <STB/stb_image.h>

#include "engineRendererSimpleSprite.h"
#include "engine.h"
#include "engineRenderer.h"
#include "engineKeyboard.h"
#include "engineRendererDrawBuffer.h"

std::map<std::string, engineRendererSimpleSprite*>mapSprites;

engineRendererSimpleSprite* MYSPR = NULL;
engineRendererSimpleSprite* get_myspr() {return MYSPR; }
engineRendererSimpleSprite* get_sprite(std::string _name) {
    return mapSprites[_name];
}


engineRendererSimpleSprite::engineRendererSimpleSprite(std::string _name,std::string filedir,int W,int H, int Xorgin,int Yorgin,int Anim_Columns,int Anim_Rows){

    mapSprites[_name] = this;
    Buf = new engineRenderDrawBuffer(20000); //NEW

    MYSPR = this;


    //mapTextures[Filename]= this ; //Texture "by name" mapping

    Filename = (std::string)filedir;
    std::string Filepath = "textures/"+Filename;
    std::cout<<" ("<<Filepath<<")";


    MaxDistance = 0;
    ImageCenter.Set(0.5,0.5);

    SpritesToDraw_IN = 0;                  //Count of sprites in buffer (imput calls)
    SpritesToDraw_OUT = 0;                  //Count of sprites in buffer (editing calls)
    SpritesToDraw_VBO = 0;                 //Count of sprites in buffer (in vbo to draw...)

    GLuint TexID = -1;

    
    int w, h, c;
    stbi_uc* pixels = stbi_load(Filepath.c_str(), &w, &h, &c, 4);

    if (pixels != NULL) {
        //GLuint GLtexture;
        glGenTextures(1, &GL_Texture);

        glBindTexture(GL_TEXTURE_2D, GL_Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        float aniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(pixels);
        Loaded = 1;
    }
    else {
        Loaded = 0;
        cout << " Cannot load texture! " << stbi_failure_reason() << endl;
    }

    Width = w;
    Height = h;

    MaxDistance = pointDistance(Vec2(0,0),Vec2(Width,Height));
    Anim_Col = Anim_Columns;
    Anim_Row = Anim_Rows;
    Anim_Frames = Anim_Col*Anim_Row;
    ImageCenter.Set(Xorgin,Yorgin);

    TexCoords = new float[8*Anim_Frames];  //TexCoords -- For Animation frames
    float Tx = 1.00000000f/(float)Anim_Col;
    float Ty = 1.00000000f/(float)Anim_Row;
    if (Loaded) std::cout<<"A columns:"<<Anim_Col<<" A Rows:"<<Anim_Row<<" Frames:"<<Anim_Frames<<" ("<<(Width*Height*4.000000f)/1048576<<") MB"<<std::endl;
    /*for (int yy=0;yy<Anim_Row;yy++)
    {
            for (int xx=0;xx<Anim_Col;xx++)
            {
            int s=(xx+(yy*Anim_Col))*8;
            //std::cout<<s/8<<" /"<<xx<<"/"<<yy<<std::endl;
            TexCoords[0+s] = 0+(Tx*xx);
            TexCoords[1+s] = 0+(Ty*yy);
            TexCoords[2+s] = Tx+(Tx*xx);
            TexCoords[3+s] = 0+(Ty*yy);
            TexCoords[4+s] = Tx+(Tx*xx);
            TexCoords[5+s] = Ty+(Ty*yy);
            TexCoords[6+s] = 0+(Tx*xx);
            TexCoords[7+s] = Ty+(Ty*yy);
            }
    }
        Vertexes = new float[8*Anim_Frames]; //Vertexes - For sprite orgin and size
        for (int a=0;a<Anim_Frames;a++)
        {
            Vertexes[a*8 +0] = 0-ImageCenter.x;
            Vertexes[a*8 +1] = 0-ImageCenter.y;
            Vertexes[a*8 +2] = Width-ImageCenter.x;
            Vertexes[a*8 +3] = 0-ImageCenter.y;
            Vertexes[a*8 +4] = Width-ImageCenter.x;
            Vertexes[a*8 +5] = Height-ImageCenter.y;
            Vertexes[a*8 +6] = 0-ImageCenter.x;
            Vertexes[a*8 +7] = Height-ImageCenter.y;
        }*/

        int spritelimit = 20000;
        for (int a = 0 ; a<spritelimit;a++){
            SpritesBuffer_IN[a].Clear();           //Sprites buffer;
            SpritesBuffer_OUT[a].Clear();           //Sprites buffer;
        }
        /*TexCoords_buffer = new float[spritelimit*8]; // cpu memory spritelimit*2(x,y)*4(4cords)
        Vertexes_buffer = new float[spritelimit*8];

        glGenBuffersARB(1, &GL_VBO_T); //gpu memory
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, GL_VBO_T);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, spritelimit*8,NULL, GL_STREAM_DRAW_ARB);

        glGenBuffersARB(1, &GL_VBO_V);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, GL_VBO_V);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, spritelimit*8,NULL, GL_STREAM_DRAW_ARB);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); // UNLOCK THIS SHIT!
*/
    M_getEngine()->registerModule( this , "Engine Simple Sprite ("+ _name +")");

}
engineRendererSimpleSprite::~engineRendererSimpleSprite(){
    cout<<"Cleaning ("<<Filename.c_str()<<") texture...";
    delete []TexCoords;
    delete []Vertexes;
    delete []TexCoords_buffer;
    delete []Vertexes_buffer;

    glDeleteTextures( 1, &GL_Texture );
    glDeleteBuffersARB(1, &GL_VBO_T);
    glDeleteBuffersARB(1, &GL_VBO_V);

    if (!glIsTexture(GL_Texture)) cout<<" OK!"<<endl;
    else cout<<" Failed!"<<endl;
}

void engineRendererSimpleSprite::DrawAsSprite( int Frame , Vec2 Position ,Vec2 Scale , float Angle){
     //m.lock();
     SpritesBuffer_IN[SpritesToDraw_IN].Set( max(0,Frame) ,Position,Scale,Angle);
     SpritesToDraw_IN++;
     //m.unlock();
 }

void engineRendererSimpleSprite::synchro() {


    Buf->swapBuffers();
    for (unsigned int a = 0; a<SpritesToDraw_IN ; a++){
    SpritesBuffer_OUT[a] = SpritesBuffer_IN[a];
    }
    SpritesToDraw_OUT = SpritesToDraw_IN;
    SpritesToDraw_IN = 0;

    if ( M_getRender() ) M_getRender()->add_module( this , 0 );


/*
    if (SpritesToDraw_VBO>0){
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, GL_VBO_T);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*SpritesToDraw_VBO*8, TexCoords_buffer, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, GL_VBO_V);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*SpritesToDraw_VBO*8, Vertexes_buffer, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); // UNLOCK THIS SHIT
    if (get_render()) get_render()->add_module( this , -20 );
    }*/

}
void engineRendererSimpleSprite::work() {

    //if (SpritesToDraw == 0) return;
            /*m.lock();

            m.unlock();
*/
        engineRender* R = M_getRender();
        if (R){
        float R_res = sqrt( R->get_resolution_h()*R->get_resolution_h() + R->get_resolution_w()*R->get_resolution_w())/1.5f;
        float R_zoom = sqrt( R->get_scale_h()*R->get_scale_h() + R->get_scale_w()*R->get_scale_w());
        float R_fac = R_res/R_zoom;
            for( int j=0;j<SpritesToDraw_OUT;j++ )
            {
                float X = SpritesBuffer_OUT[j].Position.x-R->get_position_x();
                float Y = SpritesBuffer_OUT[j].Position.y-R->get_position_y();
                if ( sqrt( X*X + Y*Y)>R_fac )
                {
                    SpritesBuffer_OUT[j] = SpritesBuffer_OUT[SpritesToDraw_OUT-1];
                    SpritesToDraw_OUT -=1;
                    j-=1;
                }
            }
        }
        for( int j=0;j<SpritesToDraw_OUT;j++ )
        {
            float X = SpritesBuffer_OUT[j].Position.x;
            float Y = SpritesBuffer_OUT[j].Position.y;
            int frame =      (int) SpritesBuffer_OUT[j].Frame ;
            Buf->drawTile(frame,Vec2(X,Y),Vec2(Width,Height),SpritesBuffer_OUT[j].Angle,Vec2(0.5,0.5),Anim_Col,Anim_Row);
        }


}


void engineRendererSimpleSprite::draw(){

            //glBindTexture( GL_TEXTURE_2D, GL_Texture );

            /*glBindBuffer(GL_ARRAY_BUFFER_ARB, GL_VBO_V);
            glVertexPointer(2, GL_FLOAT, 0,(char*)NULL);

            glBindBuffer(GL_ARRAY_BUFFER_ARB, GL_VBO_T);
            glTexCoordPointer(2, GL_FLOAT, 0,(char*)NULL);

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glBindTexture( GL_TEXTURE_2D, GL_Texture );
            glDrawArrays(GL_QUADS,0,SpritesToDraw_VBO*4);
            glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);*/
            //m.lock();
            Buf->render(GL_Texture);
            //m.unlock();

            //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            //glDisableClientState(GL_VERTEX_ARRAY);

            //glEnableClientState(GL_VERTEX_ARRAY);
            //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            //glDrawArrays(GL_QUADS, 0, amountOfVertex);
            //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            //glDisableClientState(GL_VERTEX_ARRAY);

    /*glBindTexture( GL_TEXTURE_2D, GL_Texture );
    glVertexPointer(2,GL_FLOAT,0,Vertexes_drawed_buffer);
    glTexCoordPointer(2,GL_FLOAT,0,TexCoords_drawed_buffer);
    glDrawArrays(GL_QUADS, 0 ,SpritesToDraw*4);
*/
}
