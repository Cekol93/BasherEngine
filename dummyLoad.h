#pragma once
#ifndef DUMMY_LOAD_H
#define DUMMY_LOAD_H

#include "engine.h"

class dummyLoad : public module
{
public:
    dummyLoad();
    ~dummyLoad();
    void load();
    void synchro();
    void work();
    void draw();
};


#endif 