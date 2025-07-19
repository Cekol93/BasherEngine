#pragma once
#ifndef ENGINE_RENDER_H
#define ENGINE_RENDER_H
#include "engine.h"
#include <string>
#include <map>

#define OPENGL_CHECK_ERROR (str)	\
 { \ GLenum error; \ while ((error = glGetError ()) != GL_NO_ERROR) \
 printf ("Error: %s (%s)\n",gluErrorString (error),str); \
 }



struct drawQueue {
    module* M;
    float DEPTH;
};

class renderSortclass {
public: // const data &a, const data &b
    bool operator () (const drawQueue& a, const  drawQueue& b)
    {
        return (a.DEPTH <= b.DEPTH) ? false : true;
    }
};

class engineRender
{
public:
    engineRender();
    ~engineRender();
    void swapData();
    void draw();
    void set_position(float, float);
    void set_scale(float, float);
    void set_angle(float);
    void set_resolution(float, float);
    void set_windowsize(int, int);
    void add_module(module*, float);

    float get_position_x();
    float get_position_y();
    float get_scale_w();
    float get_scale_h();
    float get_angle();
    float get_resolution_w();
    float get_resolution_h();
    int getWindowsizeW();
    int getWindowsizeH();

    long double deltaTime();

    float get_radius();
    GLuint LTex;
    GLuint get_texture(string);

protected:
    float attributes[16];

    int Window_W;
    int Window_H;

    long double computeMS_in;
    long double computeMS_out;

    vector < drawQueue > Modules; // modules to draw modules
    map<string, GLuint > Textures; // mapped textures by string
private:
};
engineRender* M_getRender();
std::string PL(const char* text);
#endif // ENGINE_RENDER_H

/*

    float CameraW = 800;
    float CameraH = 600;
    float CameraPosX = 0;
    float CameraPosY = 0;
    float CameraAngle = 0;
    float CameraScaleX = 1;
    float CameraScaleY = 1;
*/
