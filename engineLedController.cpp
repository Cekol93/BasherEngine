#include "engineLedController.h"
#include "engineLedBlock.h"
#include "engineSerialController.h"

#include "engineMouse.h"
#include "engineKeyboard.h"
#include "engineColorPicker.h"

#include "engine.h"
#include "engineRenderer.h"

#include <math.h>
#define M_PI 3.14159265358979323846

engineLedController* localLedController = NULL;
engineLedController* M_getLedController() { return localLedController; }

engineLedController::engineLedController()
{
	localLedController = this;
	M_getEngine()->registerModule(this, "Engine LED Controller");

	for (unsigned int i = 0; i < 256; i++) {
		bytesBuffer.push_back((unsigned char)0); //r
		bytesBuffer.push_back((unsigned char)0); //g
		bytesBuffer.push_back((unsigned char)0); //b
	}
	bytesBuffer.push_back(0);

	drawBuffer = new engineRenderDrawBuffer(2048);

	W = 8;
	H = 2;

	individualLeds.clear();
	for (unsigned int i = 0; i < 256; i++) {
		engineLedBlock* myLed = new engineLedBlock(i);
		individualLeds.push_back(myLed);
	}
	//-- Player settings...

	playback = false;
	playbackLooped = true;
	playbackSpeed = 4.0;
	currentFrame = 0.0;

	totalFrames = 3;
	selectedFrame = 0;
	deleteFrame = false;


	//-- Editor settings...
	editedParameter = 1;
	R = G = B = 255.0;


	//-- drawing stuff...
	
	GlFontTexture = -1;
	//GlRenderToTextureBig = -1;
	//GlRenderToTextureSmall = -1;
}

engineLedController::~engineLedController() {

}


void engineLedController::load() {
	GlFontTexture = M_getRender()->get_texture("menu_font.png");
}

void engineLedController::synchro() {
	if (M_getRender())
		M_getRender()->add_module(this, 0);
	drawBuffer->swapBuffers();
}

void engineLedController::work() {

	bool openedColorPicker = false;

	if (M_getColorPicker()) {
		openedColorPicker = M_getColorPicker()->isOpened();
		if (openedColorPicker) {
			R = M_getColorPicker()->getR();
			G = M_getColorPicker()->getG();
			B = M_getColorPicker()->getB();
		}
	}



	if (M_getKeyboard()) {

		if (M_getKeyboard()->keyPressed(49)) editedParameter = 1; // Speed
		if (M_getKeyboard()->keyPressed(50)) editedParameter = 2; // R
		if (M_getKeyboard()->keyPressed(51)) editedParameter = 3; // G
		if (M_getKeyboard()->keyPressed(52)) editedParameter = 4; // B
		if (M_getKeyboard()->keyPressed(95)) deleteFrame = true; // B

		if (M_getKeyboard()->keyPressed(43)) {
			if (editedParameter == 1 ) playbackSpeed = min(120.0, playbackSpeed + 0.25);
			if (editedParameter == 2) R = min(255.0, R + 0.25);
			if (editedParameter == 3) G = min(255.0, G + 0.25);
			if (editedParameter == 4) B = min(255.0, B + 0.25);
		}
		if (M_getKeyboard()->keyPressed(45)) {
			if (editedParameter == 1) playbackSpeed = max(0.25, playbackSpeed - 0.25);
			if (editedParameter == 2) R = max(0.0, R - 0.25);
			if (editedParameter == 3) G = max(0.0, G - 0.25);
			if (editedParameter == 4) B = max(0.0, B - 0.25);
		}

		if (M_getKeyboard()->keyPressed(32)) playback = !playback;

		if (M_getKeyboard()->keyPressed(102)) {
			if (selectedFrame == totalFrames - 1) {
				totalFrames++;
				selectedFrame++;
			}
			else
			selectedFrame = min((int)totalFrames - 1, (int)selectedFrame + 1);
		}
		if (M_getKeyboard()->keyPressed(100))
			selectedFrame = max((int)0, (int)selectedFrame - 1);

	}

	if (playback) {

		currentFrame += playbackSpeed * 0.001 * deltaTime;
		while (currentFrame > (double)totalFrames) currentFrame -= (double)totalFrames;

	}
	else {

		currentFrame = selectedFrame;

	}

	if (deleteFrame) {
		deleteFrame = false;
		for (unsigned int i = 0; i < individualLeds.size(); i++) {
			engineLedBlock* myLed = (engineLedBlock*)individualLeds[i];
			myLed->deleteFrame(selectedFrame);
		}
		totalFrames -= 1;
	}

	for (unsigned int i = 0; i < individualLeds.size() ; i++) {
		engineLedBlock * myLed = (engineLedBlock*)individualLeds[i];
		if (myLed != nullptr) {
			myLed->setFramesCount(totalFrames);
			myLed->prepareFrameForReading(currentFrame);

			
				if (openedColorPicker) {
					bytesBuffer[(i * 3) + 0] = (unsigned char)max(1.0, min(254.0, M_getColorPicker()->getR()));
					bytesBuffer[(i * 3) + 1] = (unsigned char)max(1.0, min(254.0, M_getColorPicker()->getG()));
					bytesBuffer[(i * 3) + 2] = (unsigned char)max(1.0, min(254.0, M_getColorPicker()->getB()));
				}
				else {
					bytesBuffer[(i * 3) + 0] = myLed->getR();//max(1.0, bytesBuffer[(i * 3) + 0] * 0.99995); //r
					bytesBuffer[(i * 3) + 1] = myLed->getG();//max(1.0, bytesBuffer[(i * 3) + 1] * 0.99995); //g
					bytesBuffer[(i * 3) + 2] = myLed->getB();//max(1.0, bytesBuffer[(i * 3) + 2] * 0.99995); //b
				}
		}
	}


	
	if (M_getMouse()) {

		double onScreenSizeW = max(1.0,(W-1.0)) * 150.0;
		double onScreenSizeH = max(1.0,(H-1.0)) * 150.0;

		double mouseProcX = 0.5;// + sin( (t*0.5 +j*10) * 0.1) * 0.35;
		double mouseProcY = 0.5;// - cos( (t*0.5 +j*10) * 0.1) * 0.35;
		if (M_getMouse()) {
			mouseProcX = 0.5 + M_getMouse()->getPosition().x / onScreenSizeW;
			mouseProcY = 0.5 + M_getMouse()->getPosition().y / onScreenSizeH;
			//cout << mouseProcX << "/" << mouseProcY << endl;
		}

		double WW = (double)W - 1.0;
		double HH = (double)H - 1.0;
		unsigned int mouseX = (unsigned int)round(min(WW, max(0.0, WW * mouseProcX)));
		unsigned int mouseY = (unsigned int)round(min(HH, max(0.0, HH * mouseProcY)));

		if (mouseX % 2)
			mouseY = W - mouseY - 1;

		unsigned int placeInArr = mouseY + mouseX * W;

		double cR = R;
		double cG = G;
		double cB = B;


		//bytesBuffer[placeInArr * 3 + 0] = min(255.0, bytesBuffer[placeInArr * 3 + 0] + cR); //r
		//bytesBuffer[placeInArr * 3 + 1] = min(255.0, bytesBuffer[placeInArr * 3 + 1] + cG); //g
		//bytesBuffer[placeInArr * 3 + 2] = min(255.0, bytesBuffer[placeInArr * 3 + 2] + cB); //b



		if (M_getMouse()->getButton(0)) {

			engineLedBlock* myLed = (engineLedBlock*)individualLeds[placeInArr];
			if (myLed != nullptr) {
				unsigned int editedFrame = min(selectedFrame, myLed->getFramesCount());
				
				myLed->setRGBS(R, G, B, 1, selectedFrame);
				//myLed->setR( R, editedFrame);
				//myLed->setG( G, editedFrame);
				//myLed->setB( B, editedFrame);
			}
		}

		if (M_getMouse()->getButton(1)) {

			engineLedBlock* myLed = (engineLedBlock*)individualLeds[placeInArr];
			if (myLed != nullptr) {
				unsigned int editedFrame = min(selectedFrame, myLed->getFramesCount());
				R = myLed->getR() ;
				G = myLed->getG();
				B = myLed->getB();
			}
		}

		if (M_getMouse()->getButton(2)) {

			engineLedBlock* myLed = (engineLedBlock*)individualLeds[placeInArr];
			if (myLed != nullptr) {
				unsigned int editedFrame = min(selectedFrame, myLed->getFramesCount());
				myLed->setRGBS(0.0f, 0.0f, 0.0f, 1, selectedFrame);
			}
		}


	}



	if (M_getSerialController())
		M_getSerialController()->insertBytesToSend(bytesBuffer);


	if (!openedColorPicker)
		prepareDrawContent();

}

void  engineLedController::prepareDrawContent() {

	
	for ( unsigned int xx = 0 ; xx< W ; xx++)
		for (unsigned int yy = 0; yy < H; yy++) {

			//double mouseProcX = (xx + 8.0) / 16.0;
			//double mouseProcY = (yy + 8.0) / 16.0;

			unsigned int mouseX = xx;//W-xx-1;// + 8;//(unsigned int)round(min(15.0, max(0.0, 15.0 * mouseProcX)));
			unsigned int mouseY = yy;// + 8;// (unsigned int)round(min(15.0, max(0.0, 15.0 * mouseProcY)));

			if (mouseX % 2)
				mouseY = W - mouseY - 1;

			unsigned int placeInArr = mouseY + mouseX * W;


			drawBuffer->setColor
			(     (float)bytesBuffer[placeInArr * 3 + 0] / 255.0f
				, (float)bytesBuffer[placeInArr * 3 + 1] / 255.0f
				, (float)bytesBuffer[placeInArr * 3 + 2] / 255.0f
			);
			

			Vec2 Pos;
			Pos.x = (xx - (W * 0.5f))*150.0f + 75.0f;
			Pos.y = (yy - (H * 0.5f))*150.0f + 75.0f;
			//Pos.Set( (xx-(W*0.5f) ) *150 + 75, yy*150 + 75 );

			drawBuffer->drawTile(133, Pos, Vec2(155, 155), 0, Vec2(0.4, 0.4), 16, 16);

			drawBuffer->setColor(
				(255.0f - (float)bytesBuffer[placeInArr * 3 + 0]) / 255.0f
				, (255.0f - (float)bytesBuffer[placeInArr * 3 + 1]) / 255.0f
				, (255.0f - (float)bytesBuffer[placeInArr * 3 + 2]) / 255.0f
			);
			drawText( Pos - Vec2(75.0f,0.0f) , Vec2(0.75f, 0.75f), to_string(placeInArr).c_str());

			//drawBuffer->drawTile((char)to_string(placeInArr).c_str(), Pos, Vec2(155, 155), 0, Vec2(0.5, 0.5), 16, 16);
		}
	
	string playerGuiMsg = "";
	playerGuiMsg += "Animation ";
	if (playback)
		playerGuiMsg += "Playing ";
	else
		playerGuiMsg += "Stopped ";

	playerGuiMsg += "(";
	playerGuiMsg += to_string(selectedFrame+1);
	playerGuiMsg += "/";
	playerGuiMsg += to_string(totalFrames);
	playerGuiMsg += ")";

	if (editedParameter == 1) playerGuiMsg += "Playback Speed:"+to_string(playbackSpeed)+" FPS";
	if (editedParameter == 2) playerGuiMsg += "Red Value Paint:" + to_string(R) + "/255 ";
	if (editedParameter == 3) playerGuiMsg += "Green Value Paint:" + to_string(G) + "/255 ";
	if (editedParameter == 4) playerGuiMsg += "Blue Value Paint:" + to_string(B) + "/255 ";

	
	drawBuffer->setColor(1.0f,1.0f,1.0f);
	drawText(Vec2(-1280.0f, 1380.0f), Vec2(1.0f, 1.0f), playerGuiMsg);
}

void engineLedController::drawText(Vec2 pos, Vec2 scale, string text) {
	//char editChar;
	bool edit = false;
	Vec2 inputPos = pos;
	Vec2 cursorPos = pos; //Vec2(pos.x - (+64 + str.length()*64)*scale.x*0.5f ,pos.y);
	float _eff = sin((float)M_getEngine()->elapsedTime() * 0.01f) * 0.4f;

	for (std::string::iterator c = text.begin(); c != text.end(); ++c) {
		char currChar = max(0, min((int)*c, 255));
		cursorPos.x += 55 * scale.x;
		drawBuffer->drawTile(currChar, cursorPos, Vec2(64 * scale.x, 64 * scale.y), 0, Vec2(0.5, 0.5), 16, 16);
		
	}
}

void engineLedController::drawProgressBar(Vec2 pos, Vec2 scale, double progress) {

}


void engineLedController::draw() {



	if (GlFontTexture > 0) {
		glPushMatrix();
		glTranslated(M_getRender()->get_position_x(), M_getRender()->get_position_y(), 0);
		drawBuffer->render(GlFontTexture);
		glPopMatrix(); // END DRAWING SCENE
	}
}

vector<engineLedBlock*> engineLedController::getLedBlocks() {
	return individualLeds;
}
