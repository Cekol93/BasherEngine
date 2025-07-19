#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include "engineRenderer.h"
#include "engine.h"
engineRender* localRender = NULL;
engineRender* M_getRender() { return localRender; }



void Reshape(int width, int height) {
    float rat = min(((float)width / (float)height) / (1280.f / 720.0f), 2.5f);
    //cout<<rat<<endl;
    M_getRender()->set_resolution(1280 * rat, 720);
    M_getRender()->set_windowsize(width, height);

    glViewport(0, 0, width, height);
    glClearAccum(0.0, 0.0, 0.0, 1.0);
    glClear(GL_ACCUM_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_STENCIL_BUFFER_BIT);

    /*
    if (!fullscreen)
    glutPositionWindow( (glutGet(GLUT_SCREEN_WIDTH)/2)-(glutGet(GLUT_WINDOW_WIDTH)/2)   , (glutGet(GLUT_SCREEN_HEIGHT)/2)-(glutGet(GLUT_WINDOW_HEIGHT)/2) );

    SetWindowSize(width,height);
    glViewport(0, 0, width, height);
    glClearAccum(0.0, 0.0, 0.0, 1.0);
    glClear(GL_ACCUM_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_STENCIL_BUFFER_BIT);*/

    //glClearAccum(0.0, 0.0, 0.0, 1.0);
    //glClear(GL_ACCUM_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_STENCIL_BUFFER_BIT);
}

engineRender::engineRender()
{
    computeMS_out = computeMS_in = 0.0f;
    for (int i = 0; i < 16; i++)  attributes[i] = 0.0f;
    Modules.clear();
    localRender = this;
    set_resolution(1280, 720);
    set_scale(0.25f, 0.25f);


    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutReshapeFunc(Reshape);
}

engineRender::~engineRender()
{

}
void engineRender::set_windowsize(int w, int h) { Window_W = w; Window_H = h; }
void engineRender::set_resolution(float w, float h) { attributes[0] = w; attributes[1] = h; }
void engineRender::set_position(float x, float y) { attributes[2] = x; attributes[3] = y; }
void engineRender::set_scale(float w, float h) { attributes[4] = w; attributes[5] = h; }
void engineRender::set_angle(float a) { attributes[6] = a; }

int engineRender::getWindowsizeW() { return Window_W; }
int engineRender::getWindowsizeH() { return Window_H; }
long double engineRender::deltaTime() { return computeMS_out; }

float engineRender::get_resolution_w() { return attributes[8]; }
float engineRender::get_resolution_h() { return attributes[9]; }
float engineRender::get_position_x() { return attributes[10]; }
float engineRender::get_position_y() { return attributes[11]; }
float engineRender::get_scale_w() { return attributes[12]; }
float engineRender::get_scale_h() { return attributes[13]; }
float engineRender::get_angle() { return attributes[14]; }
float engineRender::get_radius() { return attributes[15]; }
void engineRender::add_module(module* M, float Depth) { drawQueue Que = { M,Depth }; Modules.push_back(Que); }
void engineRender::swapData() {
    for (int i = 0; i < 8; i++) attributes[i + 8] = attributes[i];
}
void engineRender::draw() {
    computeMS_out = computeMS_in;

    LARGE_INTEGER rS, rE, freq;

    float MS1, MS2;
    //MS1 = get_time();
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&rS);
    glutSwapBuffers();

    std::sort(Modules.begin(), Modules.end(), renderSortclass());

    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "Opengl Error!: " << PL((const char*)gluErrorString(error)) << std::endl; return;
    }

    glMatrixMode(GL_PROJECTION);                                                //Matrixmode
    glLoadIdentity();                                                           //Start
    gluOrtho2D(-attributes[8] / 2, attributes[8] / 2, attributes[9] / 2, -attributes[9] / 2);                              //Setcamera
    glMatrixMode(GL_MODELVIEW);                                             //Matrixmode for camera scaling
    glPushMatrix();                                                       //Start
    glRotatef(attributes[14], 0.0f, 0.0f, 0.5f);                                           //Camera rotating
    glTranslated(-attributes[10] * attributes[12], -attributes[11] * attributes[13], 0);                                                    //Camera fliping
    glScalef(attributes[12], attributes[13], 1);

    glClearColor(0.04f, 0.04f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    for (unsigned int i = 0; i < Modules.size(); i++) // Ogarniamy obiekty, ich role, oraz niszczymy je (kiedy jest potrzeba)
    {
        module* M = (module*)Modules[i].M;
        M->draw();
    }
    Modules.clear();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPopMatrix(); // END DRAWING SCENE

   /* float fps = max(1.0f, 1 / M_getEngine()->deltaTime());
    float fac = max(1.0f,fps/150);
    float a = (1.0f/fac);
    float MotionFactor = (1.0f-a);//( 0.001f/max(M_getEngine()->deltaTime(),0.001f) );

    glAccum( GL_MULT,  MotionFactor );
    glAccum( GL_ACCUM, 1.0000f-MotionFactor );
    glAccum( GL_RETURN,  1.0 );
    */
//glutSwapBuffers();
    
    //MS2 = get_time();
    QueryPerformanceCounter(&rE);
    //computeMS_in = (MS2-MS1);
    computeMS_in = (rE.QuadPart - rS.QuadPart) * 1000.0f / freq.QuadPart;
}

GLuint engineRender::get_texture(string filename) {

    string Path = "textures/" + filename;
    GLuint TexID = -1;

    if (Textures.find(filename) == Textures.end()) { // If texture not loaded!

        int w, h, c;
        stbi_uc* pixels = stbi_load(Path.c_str(), &w, &h, &c, 4);

        if (pixels != NULL) {
            GLuint GLtexture;
            glGenTextures(1, &GLtexture);

            glBindTexture(GL_TEXTURE_2D, GLtexture);
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
            Textures[filename] = GLtexture;
        }
        else {
            Textures[filename] = NULL;
            cout << " Cannot load texture! " << stbi_failure_reason() << endl;
        }
    }
    return Textures[filename];
    
}


std::string PL(const char* text)
{
    std::string result;
    for (; *text; text++)
        switch (*text)
        {
        case 'π': result += static_cast <char>(0xa5); break;
        case 'Ê': result += static_cast <char>(0x86); break;
        case 'Í': result += static_cast <char>(0xa9); break;
        case '≥': result += static_cast <char>(0x88); break;
        case 'Ò': result += static_cast <char>(0xe4); break;
        case 'Û': result += static_cast <char>(0xa2); break;
        case 'ú': result += static_cast <char>(0x98); break;
        case 'ø': result += static_cast <char>(0xbe); break;
        case 'ü': result += static_cast <char>(0xab); break;
        case '•': result += static_cast <char>(0xa4); break;
        case '∆': result += static_cast <char>(0x8f); break;
        case ' ': result += static_cast <char>(0xa8); break;
        case '£': result += static_cast <char>(0x9d); break;
        case '—': result += static_cast <char>(0xe3); break;
        case '”': result += static_cast <char>(0xe0); break;
        case 'å': result += static_cast <char>(0x97); break;
        case 'Ø': result += static_cast <char>(0xbd); break;
        case 'è': result += static_cast <char>(0x8d); break;
        default: result += *text; break;
        }
    return result;
}

