#pragma once
#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#include "engine.h"

class engineSerialController : public module
{

private:
    // -- COM PORT HANDLERS...
    HANDLE ioHandler;
    COMSTAT handlerStatus;
    DWORD handlerErrors;
    string portName;
    // --
    bool connected;
    string lastErrorStr;

    string inputDataFrontBuffer;
    string inputDataBackBuffer;

    bool readByte();
    bool writeBytes();



    bool ioWriteNow;
    bool ioReadNow;

    
    unsigned int bytesPerSecond;
    unsigned int cursor;

    long double reconnectTimer;
    long double sendTimer;

    vector<unsigned char>bytesToSendFrontBuffer;
    vector<unsigned char>bytesToSendBackBuffer;



public:
    engineSerialController();
    ~engineSerialController();
    void load();
    void synchro();
    void work();
    void draw();

    bool isConnected();
    string getBufferedData();

    void connect(string portName);
    void disconnect();

    void insertBytesToSend( vector<unsigned char> in );

};

engineSerialController* M_getSerialController();

#endif 