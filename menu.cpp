#include "menu.h"
#include "engine.h"

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"
#include "sound.h"
#include "maploader.h"

#include "render.h"
#include "draw_buffer.h"

void menu::find_maps()
{
    Maps.clear();
    string search_path = "maps/*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                    Maps.push_back( fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    if (Maps.size()>1) Maps.erase (Maps.begin(),Maps.begin()+2); //removing "." and ".."
}

void menu::load()
{
    GL_Texture = get_render()->get_texture("menu_font.png");
    find_maps();
    //GL_Bac_Texture = get_render()->get_texture("menu_font.png");
}

menu::menu()
{

    Chars = new draw_buffer(256);
    DrawFrames = 0;
    CountedFrames = 0;
    OneSecTimer = 0;

    //Chars_OUT = Chars_IN = 0;
    GL_Texture = -1;
    /*front_bufferV = new float[8*1024]; // 4(quad)x2(point)x2(texture&verts) FOR MULTITHREADING
    front_bufferT = new float[8*1024];
    back_bufferV = new float[8*1024];
    back_bufferT = new float[8*1024];

    edited_bufferV = front_bufferV;
    edited_bufferT = front_bufferT;
    drawed_bufferV = back_bufferV;
    drawed_bufferT = back_bufferT;
*/
    visible = false;
    active = true;
    alpha = 0.0f; // smooth menu fade...
    selector = 0; // cursor position
    selector_max = 0; // maximum cursor position
    state = 0; // menu action state
    selected = 0;
    play_sound = 0;
    MousePos.Clear();
    ShowMouse = 1.0f;


  /*  TexCoords = new float[8*256];  //TexCoords -- For Animation frames
    float Tx = 1.00000000f/(float)16;
    float Ty = 1.00000000f/(float)16;

    for (int yy=0;yy<16;yy++)
    {
            for (int xx=0;xx<16;xx++){
            int s=(xx+(yy*16))*8;
            TexCoords[0+s] = 0+(Tx*xx);
            TexCoords[1+s] = 0+(Ty*yy);
            TexCoords[2+s] = Tx+(Tx*xx);
            TexCoords[3+s] = 0+(Ty*yy);
            TexCoords[4+s] = Tx+(Tx*xx);
            TexCoords[5+s] = Ty+(Ty*yy);
            TexCoords[6+s] = 0+(Tx*xx);
            TexCoords[7+s] = Ty+(Ty*yy);
            }
    }*/
    get_engine()->register_module( this , "Menu" );
}

menu::~menu()
{

}

void menu::synchro() {

        if (!visible) return;
        if ( play_sound>0 ) { get_sound()->Play_Queued("menu.wav",Vec2(0,0),10,play_sound); play_sound = 0; }
        Chars->swap_buffers();

/*
        float *Temp;//,*TempT; // SWAP BUFFERS FOR MULTITHREADING
        Temp = edited_bufferV;
        edited_bufferV = drawed_bufferV;
        drawed_bufferV = Temp;

        Temp = edited_bufferT;
        edited_bufferT = drawed_bufferT;
        drawed_bufferT = Temp;

        Chars_OUT = Chars_IN;
*/
        if (get_render()) get_render()->add_module(this , -9000 );
}

void menu::draw_option(Vec2 pos , Vec2 scale , string str , int option_id , int option_action ){

    if (get_mouse()) if (MousePos.y>pos.y -20 && MousePos.y<pos.y +50 && option_action>=0 && get_mouse()->Motion_Any() && selector != option_id) { selector = option_id; play_sound = 1;} // mouse detection;

    if (selector == option_id) {
        if (selected == 0 and  get_keyboard()->Key_Pressed(13) ) { selected = 1; state = option_action; selector = 0; play_sound = 0.7f;}

        if (get_mouse())
            if (selected == 0 and  get_mouse()->Button_Pressed(GLUT_LEFT_BUTTON) )
                if (MousePos.y>pos.y -20 && MousePos.y<pos.y +50) { selected = 1; state = option_action; selector = 0; play_sound = 0.7f;}
            scale.x *=1.5; scale.y *=1.5;
    }

    Vec2 Pos =Vec2(pos.x - (+64 + str.length()*64)*scale.x*0.5f ,pos.y);



    for(std::string::iterator c = str.begin(); c != str.end(); ++c) {
       /* CharsBuffer[Chars_IN].Set( max(0,min((int)*c,255)) ,Pos,scale,0);
        Pos.x+=64*scale.x;
        Chars_IN++;
*/      Pos.x+=64*scale.x;
        Chars->draw_tile(max(0,min((int)*c,255)),Pos,Vec2(64*scale.x,64*scale.y),0,Vec2(0.5,0.5),16,16);
    }


}

void menu::work() {

    //Chars_IN = 0;//reset char buffer
    if (get_mouse()) {
        ShowMouse-=1.0f*this->deltaT;
        if (get_mouse()->Motion_Any()) ShowMouse = 1.0f;
        MousePos.Set
        ( (get_mouse()->Raw_Position().x-get_render()->get_windowsize_w()/2 )*(1/get_render()->get_scale_w() )
        , (get_mouse()->Raw_Position().y-get_render()->get_windowsize_h()/2 )*(1/get_render()->get_scale_h() )
        );
    }

    if (get_keyboard()) // TOGGLE MENU
        if ( get_keyboard()->Key_Pressed(27) ) {// esc
        if (active == 0) { state = 0; selector = 0;}// reseting menu state
        if (state != 0) state = 0;
            else active = !active;
        //cout<<"TOGLE MENU!"<<endl;;
        play_sound = 0.5f;
        }
    if (active) alpha = min((long double)alpha+deltaT*5.0,(long double)1.0);
    else        alpha = max((long double)alpha-deltaT*5.0,(long double)0.0);
    if (alpha>0) visible = true; else visible = false;
    if (!visible) return;
    // MOVING CURSOR...

    if ( get_keyboard()->Key_Pressed(101) ) {selector-=1;  if (selector<0) selector = selector_max;  play_sound = 1; }
    if ( get_keyboard()->Key_Pressed(103) ) {selector+=1;  if (selector>selector_max) selector = 0;  play_sound = 1; }
    selected = 0; // reseting enter lock...
   // cout<<state<<endl;

    if (state == 666){ get_engine()->stop(); } // quit...
    if (state == 0){ // title;
    draw_option(Vec2(0,-100),Vec2(1,1),"Play",0,6);
    draw_option(Vec2(0,000),Vec2(1,1),"Options",1,3);
    draw_option(Vec2(0,100),Vec2(1,1),"Help",2,5);
    draw_option(Vec2(0,200),Vec2(1,1),"Quit",3,666);
    selector_max = 3;
    }
    if (state == 1){ // player selection;
    draw_option(Vec2(0,-100),Vec2(1,1),"Single player",0,2);
    draw_option(Vec2(0,0),Vec2(1,1),"2 Players",1,2);
    draw_option(Vec2(0,100),Vec2(1,1),"3 Players",2,2);
    draw_option(Vec2(0,200),Vec2(1,1),"4 Players",3,2);
    draw_option(Vec2(0,300),Vec2(1,1),"Back",4,0);
    selector_max = 4;
    }

    if (state == 2){ // stage selection;
    draw_option(Vec2(0,-100),Vec2(1,1),"Continue",0,0);
    draw_option(Vec2(0,0),Vec2(1,1),"New game",1,0);
    draw_option(Vec2(0,100),Vec2(1,1),"Chapter selection",2,0);
    draw_option(Vec2(0,200),Vec2(1,1),"Back",3,1);
    selector_max = 3;
    }

    if (state == 3){ // Options;
    draw_option(Vec2(0,-100),Vec2(1,1),"Framerate lock:",0,0);
    draw_option(Vec2(0,0),Vec2(1,1),"Graphic detail:",1,0);
    draw_option(Vec2(0,100),Vec2(1,1),"Sound detail:",2,0);
    draw_option(Vec2(0,200),Vec2(1,1),"Back",3,0);
    selector_max = 3;
    }
    if (state == 4){ // Credits...;
    draw_option(Vec2(0,-100),Vec2(1,1),"Create game",0,0);
    draw_option(Vec2(0,0),Vec2(1,1),"Join game",1,0);
    draw_option(Vec2(0,100),Vec2(1,1),"Player setup",2,0);
    draw_option(Vec2(0,200),Vec2(1,1),"Back",3,0);
    selector_max = 3;
    }
    if (state == 5){ // help...;
    draw_option(Vec2(0,0),Vec2(1,1),"No halp 4 you faggot!",0,0);
    draw_option(Vec2(0,100),Vec2(1,1),"Back",1,0);
    selector_max = 2;
    }

    if (state == 6){ // Maps selection...
        if (Maps.size()>0){
            draw_option(Vec2(0,-300),Vec2(1.25,1.25),"Select map:",-1,-1);
                for (int a = 0; a<Maps.size();a++)
                draw_option(Vec2(0,-100+100*a ),Vec2(1,1),Maps[a],a,100+a);
                selector_max = max(0, (int)(Maps.size()-1) );
        } else{
            draw_option(Vec2(0,0),Vec2(1.5,1.5),"No maps find :(",-1,-1);
            selector_max = 0;
        }
    }

    if ( state>=100 && state<200 ){
        get_maploader()->Load_map(Maps[state-100]);
        state = 0;
        selector = 0;
        active = false;
    }


    CountedFrames += 1;
    OneSecTimer += this->deltaT;
    if (OneSecTimer>1){ OneSecTimer-=1.0f; DrawFrames = CountedFrames; CountedFrames = 0;}

    /*string fstr;
    char buff[32];
    sprintf( buff , "FPS: %i", DrawFrames  ) ;
    fstr = buff;
    draw_option(Vec2(0,800),Vec2(1.25f,1.25f),fstr,-1,-1);
*/
    draw_option(Vec2(0,-600),Vec2(2,2),"MyGame 001",-1,-1);

    if (ShowMouse>0)
    Chars->draw_tile(132,MousePos,Vec2(100,100),40,Vec2(0.5,0.5),16,16);

    float XX=get_mouse()->Raw_Position().x-get_render()->get_windowsize_w()/2;
    float YY=get_mouse()->Raw_Position().y-get_render()->get_windowsize_h()/2;
    get_render()->set_position(XX,YY);


/*
    for( int j=0;j<Chars_IN;j++ )
    {

        float X = CharsBuffer[j].Position.x;
        float Y = CharsBuffer[j].Position.y;

        int frame =      (int) CharsBuffer[j].Frame ;
        int Tfr = frame*8;
        int Tptd = j*8;
        edited_bufferT[Tptd + 0] = TexCoords[Tfr ] ;   //Texturing
        edited_bufferT[Tptd + 1] = TexCoords[Tfr +1 ] ;
        edited_bufferT[Tptd + 2] = TexCoords[Tfr +2 ] ;
        edited_bufferT[Tptd + 3] = TexCoords[Tfr +3 ] ;
        edited_bufferT[Tptd + 4] = TexCoords[Tfr +4 ] ;
        edited_bufferT[Tptd + 5] = TexCoords[Tfr +5 ] ;
        edited_bufferT[Tptd + 6] = TexCoords[Tfr +6 ] ;
        edited_bufferT[Tptd + 7] = TexCoords[Tfr +7 ] ; // End of texturing

        float xs = CharsBuffer[j].Scale.x*alpha;
        float ys = CharsBuffer[j].Scale.y;

        edited_bufferV[Tptd + 0] = (-32.0f)*xs +X;
        edited_bufferV[Tptd + 1] = (-32.0f)*ys +Y ;
        edited_bufferV[Tptd + 2] = (32.0f)*xs +X ;
        edited_bufferV[Tptd + 3] = (-32.0f)*ys +Y ;
        edited_bufferV[Tptd + 4] = (32.0f)*xs +X ;
        edited_bufferV[Tptd + 5] = (32.0f)*ys +Y ;
        edited_bufferV[Tptd + 6] = (-32.0f)*xs +X ;
        edited_bufferV[Tptd + 7] = (32.0f)*ys +Y ;

    }*/
}

void menu::draw() {

 if (GL_Texture<0) return;

 glClear(GL_COLOR_BUFFER_BIT);
 glPushMatrix();
 glTranslated(get_render()->get_position_x(),get_render()->get_position_y(),0);
 glColor4f(alpha,alpha,alpha,alpha);
 Chars->render(GL_Texture);
   /* glBindTexture( GL_TEXTURE_2D, GL_Texture ); // DRAWING MENU ON SCREEN...
    glVertexPointer(2,GL_FLOAT,0,drawed_bufferV);
    glTexCoordPointer(2,GL_FLOAT,0,drawed_bufferT);
    glDrawArrays(GL_QUADS, 0 ,4*Chars_OUT);
*/
 glColor4f(1.0f,1.0f,1.0f,1.0f);
 glPopMatrix(); // END DRAWING SCENE

}
