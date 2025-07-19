#include "engineSerialController.h"
#include "engine.h"
#include <math.h>


engineSerialController* localSerialController = NULL;
engineSerialController* M_getSerialController() { return localSerialController; }

engineSerialController::engineSerialController()
{
	localSerialController = this;
	connected = false;

	lastErrorStr.clear();

	ioWriteNow = false;
	ioReadNow = false;

	bytesToSendFrontBuffer.clear();
	bytesToSendBackBuffer.clear();

	cursor = 0;
	bytesPerSecond = 500000;

	sendTimer = 0;
	reconnectTimer = 0;

	M_getEngine()->registerModule(this, "Engine Serial Controller");

}

engineSerialController::~engineSerialController() {
	if (this->isConnected())
		disconnect();
}

void engineSerialController::insertBytesToSend(vector<unsigned char> in) {
	bytesToSendFrontBuffer = in;
}

void engineSerialController::load() {

}

void engineSerialController::synchro() {

	bytesToSendBackBuffer.clear();
	bytesToSendBackBuffer = bytesToSendFrontBuffer;
	//bytesToSendFrontBuffer.clear();

}

void engineSerialController::work() {



		if (this->isConnected()) {

			/*if (ioReadNow && this->readByte()) {
				ioReadNow = false;
			}*/
			//if (ioWriteNow)
			//for (unsigned int j = 0 ; j<8 ; j++)
				if (this->writeBytes()) {
			//		ioWriteNow = false;
				}


		}
		else {
			this->connect("\\\\.\\COM4");
		}


	//}
	debugMessage += " #0 Is Connected? ";
	debugMessage += (connected) ? "#6YES#0" : "#3NO#0";
	debugMessage += "#n #1Last status : " + lastErrorStr;
}

void engineSerialController::draw() {

}

bool engineSerialController::readByte() {
	/*DWORD readedBytes;
	char readedByte[1];
	ClearCommError(ioHandler, &handlerErrors, &handlerStatus);
	if (handlerStatus.cbInQue > 0) {
		if (ReadFile(ioHandler, readedByte, 1, &readedBytes, NULL)) {
			inputDataFrontBuffer.append(readedByte, 1);
			return true;
		}
		else return false;
	}
	return false;
	*/
}

bool engineSerialController::writeBytes() {


	sendTimer -= this->deltaTime;


	if (bytesToSendBackBuffer.size() > 0) {

		if (sendTimer <= 0) {
			
			if (sendTimer < -35) sendTimer = 0;

			unsigned int packetSize = min(2048, (int)bytesToSendBackBuffer.size() );
			unsigned char bytes[2048];
			for (unsigned int i = 0 ; i< packetSize; i++ )
				bytes[i] = (unsigned char)bytesToSendBackBuffer[i];

			DWORD dwBytesWritten = 0;
			WriteFile(ioHandler, bytes, packetSize, &dwBytesWritten, NULL);
			sendTimer += ( 33.3333333333333 );

			if (dwBytesWritten == 0){ // When packets not send / connection closed!
				connected = false; 
				lastErrorStr = "#3Port unexpected shut down! (" + portName + ") not available";
			}

		}
	}
	return true;
}

bool engineSerialController::isConnected() { return connected; }

void engineSerialController::connect(string portname) {
    portName = portname;
	DWORD COM_BAUD_RATE = bytesPerSecond;

	ioHandler = CreateFileA(static_cast<LPCSTR>(portName.c_str()),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (ioHandler == INVALID_HANDLE_VALUE) {

		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			lastErrorStr = "#3Handle was not attached. Reason:("+ portName +") not available";
	}
	else {

		DCB dcbSerialParams = { 0 };

		if (!GetCommState(ioHandler, &dcbSerialParams)) {

			lastErrorStr ="#3Failed to get current serial params";
		}

		else {
			dcbSerialParams.BaudRate = COM_BAUD_RATE;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(ioHandler, &dcbSerialParams))
				lastErrorStr="#3Could not set serial port params";
			else {
				connected = true;
				PurgeComm(ioHandler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				lastErrorStr="#6Connected at (" + portName + ") at " + to_string(COM_BAUD_RATE) ;
			}
		}
	}
}

void engineSerialController::disconnect() {
	connected = false;
	CloseHandle(ioHandler);
}

string engineSerialController::getBufferedData() {
	return inputDataBackBuffer;
}
