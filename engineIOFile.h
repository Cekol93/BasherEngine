#pragma once
#ifndef ENGINE_IO_FILE_H
#define ENGINE_IO_FILE_H

#include "engine.h"

class engineIOFIle : public module
{
public:
    engineIOFIle();
    ~engineIOFIle();
    void load();
    void synchro();
    void work();
    void draw();

    string savePath;
    string loadPath;

    bool pathEntered;
    bool isSaving;
    bool isLoading;

    void createSaveWindow();
    void createLoadWindow();
    void saveFile();
    void loadFile();


};


#endif 