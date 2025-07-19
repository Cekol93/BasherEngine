#include "simplesprite.h"
#include "level_block.h"
#include "engine.h"
#include "render.h"
#include "keyboard.h"
#include "mouse.h"


level_block::level_block(){
    GL_Texture = -1;//get_myspr()->GL_Texture;

    //myQuad = new draw_buffer(128);


    //string Filepath = "textures/test.png";
    //GL_Texture = SOIL_load_OGL_texture(Filepath.c_str(),SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID,  SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA  ); // | SOIL_FLAG_MIPMAPS

    for (int g = 0;g<4;g++)
    {
        TexCoords[g].Set(0.0f,0.0f);
        Vertex_POS[g].Set(0.0f,0.0f);
        //VisibleVertPOS[g].Set(0.0f,0.0f);
    }

    front_bufferV = new float[8]; // 4(quad)x2(point)x2(texture&verts) FOR MULTITHREADING
    front_bufferT = new float[8];
    back_bufferV = new float[8];
    back_bufferT = new float[8];

    color = new float[16];
    for (int a = 0 ; a<16; a++)
        color[a]=1.0f;

    edited_bufferV = front_bufferV;
    edited_bufferT = front_bufferT;
    drawed_bufferV = back_bufferV;
    drawed_bufferT = back_bufferT;

    visible = false;

    ParallaxMultipler = Random(-0.5,0.01);   //far 1-0 near
    get_engine()->register_module( this , "Level_block" );

}
level_block::~level_block(){
  /*  cout<<"Cleaning ("<<Filename.c_str()<<") texture...";
    delete []TexCoords;
    delete []Vertexes;
    delete []TexCoords_buffer;
    delete []Vertexes_buffer;

    glDeleteTextures( 1, &GL_Texture );
    glDeleteBuffersARB(1, &GL_VBO_T);
    glDeleteBuffersARB(1, &GL_VBO_V);

    if (!glIsTexture(GL_Texture)) cout<<" OK!"<<endl;
    else cout<<" Failed!"<<endl;*/
}
void level_block::load(){

        string FILE = "";
        int R = RandomChoose(0,3);
        switch( R ){
        case 0: FILE = "test1.png"; break;
        case 1: FILE = "test2.png"; break;
        case 2: FILE = "test3.png"; break;
        case 3: FILE = "test4.png"; break;
        }
        GL_Texture = get_render()->get_texture(FILE);

}

void level_block::synchro() {
    if (!visible) return;
        get_render()->add_module( this , ParallaxMultipler*100 );

       // myQuad.swap_buffers();


}
void level_block::work() {

    m.lock();
        float *Temp;//,*TempT; // SWAP BUFFERS FOR MULTITHREADING
        Temp = edited_bufferV;
        edited_bufferV = drawed_bufferV;
        drawed_bufferV = Temp;

        Temp = edited_bufferT;
        edited_bufferT = drawed_bufferT;
        drawed_bufferT = Temp;
    m.unlock();


    ParallaxMultipler = sin( (Vertex_POS[0].x*Vertex_POS[0].y)*0.02f +get_time()*0.0001f )*0.45 - 1.0f;
    float TmpT[8]={
        TexCoords[0].x,TexCoords[0].y,
        TexCoords[1].x,TexCoords[1].y,
        TexCoords[2].x,TexCoords[2].y,
        TexCoords[3].x,TexCoords[3].y
        };

    Vec2 CP = Vec2( 0.0f , 0.0f );

   //float P = 500;
   //ParallaxMultipler-= ((P-min(P,Point_distance(Vertex_POS[0],get_mouse()->Position())  ) )/P)*0.5;

    for (int i = 0; i<4 ; i++){

        edited_bufferV[i*2] = Vertex_POS[i].x - (Vertex_POS[i].x-get_render()->get_position_x() )*ParallaxMultipler;
        edited_bufferV[1+i*2] = Vertex_POS[i].y - (Vertex_POS[i].y-get_render()->get_position_y() )*ParallaxMultipler;
        CP.x+=edited_bufferV[i*2];
        CP.y+=edited_bufferV[1+i*2];

        edited_bufferT[i*2] = TmpT[i*2];
        edited_bufferT[1+ i*2] = TmpT[1+ i*2];

    }
    CP.x = CP.x/4;
    CP.y = CP.y/4;

    //sin( (Vertex_POS[0].x*Vertex_POS[0].y)*0.02f +get_time()*0.001f )*0.4;
    //cout<<CP.x<<"/"<<CP.y<<endl;
    visible = false;
    render* R = get_render();
        if (R){
        float R_res = sqrt( R->get_resolution_h()*R->get_resolution_h() + R->get_resolution_w()*R->get_resolution_w())/1.5f;
        float R_zoom = sqrt( R->get_scale_h()*R->get_scale_h() + R->get_scale_w()*R->get_scale_w());
        float R_fac = R_res/R_zoom;
        float X = CP.x -R->get_position_x();
        float Y = CP.y -R->get_position_y();
        if ( sqrt( X*X + Y*Y)<R_fac ) visible = true;
    }
    if (GL_Texture<=0) visible = false;
    if (visible){
        for (int i = 0; i<4 ; i++){ // EFFECT!
        float Ang = Point_direction(Vec2(edited_bufferV[i*2],edited_bufferV[1+i*2]),CP);
        float Dist = sin( get_time()*0.005f + Ang )*20;
        edited_bufferV[i*2] += lengthdir_x( Dist , Ang-45 );
        edited_bufferV[1+i*2] += lengthdir_y( Dist , Ang-45 );
        }
   }

}


void level_block::SetVertexPos(float a,float b,float c,float d, float e, float f,float g,float h){
    if(
       (DirDiv( Point_direction( Vec2(a,b),Vec2(c,d)) , Point_direction(Vec2(c,d),Vec2(e,f)) )<0 ) &&
       (DirDiv( Point_direction( Vec2(c,d),Vec2(e,f)) , Point_direction(Vec2(e,f),Vec2(g,h)) )<0 ) &&
       (DirDiv( Point_direction( Vec2(e,f),Vec2(g,h)) , Point_direction(Vec2(g,h),Vec2(a,b)) )<0 ) &&
       (DirDiv( Point_direction( Vec2(g,h),Vec2(a,b)) , Point_direction(Vec2(a,b),Vec2(c,d)) )<0 )
    )
    {
        if ( (Point_distance( Vec2(a,b),Vec2(c,d) )<4089) && (Point_distance( Vec2(c,d),Vec2(e,f) )<4089) &&
            (Point_distance( Vec2(e,f),Vec2(g,h) )<4089) && (Point_distance( Vec2(g,h),Vec2(a,b) )<4089) )
        {
        Vertex_POS[0].Set(a,b);
        Vertex_POS[1].Set(c,d);
        Vertex_POS[2].Set(e,f);
        Vertex_POS[3].Set(g,h);
        }
        else cout<<"ERRO!";
    }

    else cout<<"ERRO!";
}
void level_block::SetTexCoords(float a,float b,float c,float d, float e, float f,float g,float h){

    TexCoords[0].Set(a,b);
    TexCoords[1].Set(c,d);
    TexCoords[2].Set(e,f);
    TexCoords[3].Set(g,h);
}

void level_block::draw(){

/*    m.lock();
    glBindTexture( GL_TEXTURE_2D, GL_Texture );
    glVertexPointer(2,GL_FLOAT,0,drawed_bufferV);
    glTexCoordPointer(2,GL_FLOAT,0,drawed_bufferT);
    glColorPointer(4,GL_FLOAT,0,color);
    glDrawArrays(GL_QUADS, 0 ,4);
    m.unlock();*/

    //myQuad.render(GL_Texture);

}

