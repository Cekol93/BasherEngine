#include "engineIOFile.h"
#include "engine.h"
#include "engineKeyboard.h"

#include "engineLedBlock.h"
#include "engineLedController.h"

#include <windows.h>
#include <shobjidl.h> 
#include <stdlib.h>
#include <fstream>


#define FILEPATH_BUFFER_SIZE 256

engineIOFIle::engineIOFIle()
{
    M_getEngine()->registerModule(this, "Engine Input Output File");

    savePath = "";
    loadPath = "";
    pathEntered = false;
    isSaving = false;
    isLoading = false;


}

engineIOFIle::~engineIOFIle() {

}


void engineIOFIle::load() {

}

void engineIOFIle::synchro() {

}

void engineIOFIle::work() {
    
    debugMessage += "Save Path :" + savePath;
    
    if (M_getKeyboard()) {
        if (M_getKeyboard()->keyReleased( 11 )) isSaving = true;
        if (M_getKeyboard()->keyReleased( 12 )) isLoading = true;
    }
    
    if (!isSaving && !isLoading) return;

    if (pathEntered) {
        if (isSaving)   saveFile();
        if (isLoading)  loadFile();
    }else {
        if (isSaving)   createSaveWindow();
        if (isLoading)  createLoadWindow();
        //cout << "IS path entered: " << pathEntered << endl;
        if (!pathEntered) { isSaving = false; isLoading = false; }
    }

}

void engineIOFIle::createSaveWindow(){
    
    pathEntered = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileSaveDialog* pFileSave;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileSave->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        /* CONVERSATION PWSTR TO STRING*/
                        size_t i;
                        char* pMBBuffer = (char*)malloc(FILEPATH_BUFFER_SIZE);
                        const wchar_t* pWCBuffer = pszFilePath;

                        // Conversion
                        wcstombs_s(&i, pMBBuffer, (size_t)FILEPATH_BUFFER_SIZE,
                            pWCBuffer, (size_t)FILEPATH_BUFFER_SIZE - 1); // -1 so the appended NULL doesn't fall outside the allocated buffer

                        // Free multibyte character buffer
                        if (pMBBuffer)
                        {
                            savePath = pMBBuffer;
                            pathEntered = true;
                            free(pMBBuffer);
                        }
                        ///MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileSave->Release();
        }
        CoUninitialize();
    }
};
void engineIOFIle::createLoadWindow(){

    pathEntered = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {                       
                        /* CONVERSATION PWSTR TO STRING*/
                        size_t i;
                        char* pMBBuffer = (char*)malloc(FILEPATH_BUFFER_SIZE);
                        const wchar_t* pWCBuffer = pszFilePath;

                        // Conversion
                        wcstombs_s(&i, pMBBuffer, (size_t)FILEPATH_BUFFER_SIZE,
                            pWCBuffer, (size_t)FILEPATH_BUFFER_SIZE - 1); // -1 so the appended NULL doesn't fall outside the allocated buffer

                        // Free multibyte character buffer
                        if (pMBBuffer)
                        {
                            loadPath = pMBBuffer;
                            pathEntered = true;
                            free(pMBBuffer);
                        }
                        else
                            isLoading = false;
                        ///MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
};

void engineIOFIle::saveFile(){

    ofstream MyFile(savePath);

    if (M_getLedController()) {


        MyFile << "Width" << endl;
        MyFile << to_string( M_getLedController()->W ) << endl;
        
        MyFile << "Height" << endl;
        MyFile << to_string( M_getLedController()->H ) << endl;

        MyFile << "Frames" << endl;
        MyFile << to_string( M_getLedController()->totalFrames )<< endl;

        unsigned int ledsCount = M_getLedController()->W * M_getLedController()->H;

        MyFile << "LedsCount" << endl;
        MyFile << to_string(ledsCount) << endl;

        vector<engineLedBlock*> ledsToSave = M_getLedController()->getLedBlocks();
        for (unsigned int i = 0; i < ledsCount; i++) {
            engineLedBlock* led = ledsToSave[i];

            MyFile << "LedData" << endl;
            for (unsigned int j = 0; j < M_getLedController()->totalFrames; j++) {

                colorPixelRegistry C = (colorPixelRegistry)led->colorPixelRegistryData[j];
                MyFile << to_string( C.r ) << endl;
                MyFile << to_string( C.g ) << endl;
                MyFile << to_string( C.b ) << endl;
            }
        }
    }
    MyFile.close();
    cout << "file saved" << endl;

    isSaving = false;
    pathEntered = false;
};
void engineIOFIle::loadFile(){


    
    unsigned int dataAction = 0;
    
    unsigned int frames = 0;
    unsigned int currentPixel = 0;


    unsigned int boardWidth = 0;
    unsigned int boardHeight = 0;
    unsigned int boardFrames = 0;
    unsigned int boardLedsCount = 0;
    unsigned int boardLedsCounted = 0;

    vector<string>fileLines;
    string myDataLine;
    ifstream MyReadFile(loadPath);
    while (getline(MyReadFile, myDataLine)) {
        if (myDataLine.compare("") != 0)
            fileLines.push_back(myDataLine);
    }
    MyReadFile.close();

    cout << "1" << endl;

    for (unsigned int i = 0; i < fileLines.size(); i++) {
        cout << i;
        string line = fileLines[i];
        cout << " " << line;
        string nextLine = fileLines[ min( (unsigned int)fileLines.size()-1 , i+1 ) ];
        cout <<"/" << nextLine << endl;


        if (line.compare("Width") == 0)
            boardWidth = stoi( nextLine );

        if (line.compare("Height") == 0)
            boardHeight = stoi( nextLine );

        if (line.compare("Frames") == 0)
            boardFrames = stoi( nextLine );

        if (line.compare("LedsCount") == 0)
            boardLedsCount = stoi(nextLine);

        if (line.compare("LedData") == 0)
            boardLedsCounted++;

    }
    cout << " END " << endl;
    std::cout << " FILE READED W:" << boardWidth << " H:" << boardHeight << " F:" << boardFrames <<" L:"<<boardLedsCount<<"/"<< boardLedsCount<< endl;

    if (boardWidth > 0 && boardHeight > 0 && boardFrames > 0) {



        if (M_getLedController())
            M_getLedController()->totalFrames = boardFrames;

        unsigned int currentLed = 0;
        for (unsigned int i = 0; i < fileLines.size(); i++) {
            string line = fileLines[i];
            if (line.compare("LedData") == 0) {

                cout << "11:" << currentLed << endl;
                engineLedBlock* eb = M_getLedController()->individualLeds[currentLed];
                cout << "22:" << eb << endl;
                eb->setFramesCount(boardFrames);
                cout << "33" << endl;

                for (unsigned int j = 0; j < boardFrames; j++) {

                    float R = stof(fileLines[i + 1 + j * 3]);
                    float G = stof(fileLines[i + 2 + j * 3]);
                    float B = stof(fileLines[i + 3 + j * 3]);

                    cout << "R:" << R << " G:" << G << " B:" << B << endl;

                    eb->setRGBS(R, G, B, 0, j);
                }
                currentLed++;
            }


        }

    }



    /*

    while (getline(MyReadFile, myDataLine)) {
        
        if ( myDataLine.compare("Width") == 0)  dataAction = 1;
        else if ( myDataLine.compare("Height") == 0)  dataAction = 2;
        else if ( myDataLine.compare("Frames") == 0)  dataAction = 3;
        else if ( myDataLine.compare("Pixel") == 0)  dataAction = 4;
        else {

            if ( dataAction == 1 ){
                if (M_getLedController())
                    M_getLedController()->W = stoi(myDataLine);
                cout << "READED WIDTH:" << stoi(myDataLine) << endl;
            }
            if ( dataAction == 2 ){
                if (M_getLedController())
                    M_getLedController()->H = stoi(myDataLine);
                cout << "READED HEIGHT:" << stoi(myDataLine) << endl;
            }
            if ( dataAction == 3 ){
                if (M_getLedController())
                    M_getLedController()->totalFrames = stoi(myDataLine);
                    frames = stoi(myDataLine);
                    cout << "READED FRAMES:" << stoi(myDataLine) << endl;
            }
            if ( dataAction == 4 ){

                engineLedBlock* eb = M_getLedController()->individualLeds[currentPixel];
                eb->setFramesCount(frames);

                for (unsigned int i = 0; i < frames; i++) {
                    
                    getline(MyReadFile, myDataLine);
                    if (myDataLine.compare("") != 0) {
                        engineLedBlock* eb = M_getLedController()->individualLeds[currentPixel];
                        eb->colorPixelRegistryInput[i].r = stof(myDataLine);
                    }
                    //if (MyReadFile.)

                    getline(MyReadFile, myDataLine);
                    if (myDataLine.compare("") != 0) {
                        engineLedBlock* eb = M_getLedController()->individualLeds[currentPixel];
                        eb->colorPixelRegistryInput[i].g = stof(myDataLine);
                    }

                    getline(MyReadFile, myDataLine);
                    if (myDataLine.compare("") != 0) {
                        engineLedBlock* eb = M_getLedController()->individualLeds[currentPixel];
                        eb->colorPixelRegistryInput[i].b = stof(myDataLine);
                    }
                }
                currentPixel++;
            }
        }
        cout << myDataLine << endl ;
    }*/

    // Close the file
    MyReadFile.close();
    cout << "file loaded" << endl;

    isLoading = false;
    pathEntered = false;
};

void engineIOFIle::draw() {

}

