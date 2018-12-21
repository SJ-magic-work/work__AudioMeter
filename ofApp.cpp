/************************************************************
************************************************************/
#include "ofApp.h"


/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _soundStream_Input_DeviceId, int _soundStream_Output_DeviceId)
: soundStream_Input_DeviceId(_soundStream_Input_DeviceId)
, soundStream_Output_DeviceId(_soundStream_Output_DeviceId)
, b_DispGui(true)
, b_Smooth(true)
, b_EnableAudioOut(false)
, pass_AudioVol(THREAD_AUDIO_VOL::getInstance())
, AudioCh_L(1, -1.6006331351528, 0.66805369559014, 0.016855140109326, 0.033710280218652, 0.016855140109326)
, AudioCh_R(1, -1.6006331351528, 0.66805369559014, 0.016855140109326, 0.033710280218652, 0.016855140109326)
{
	fp_Log = fopen("Log.csv", "w");
	
	font[FONT_S].load("font/RictyDiminishedDiscord-Bold.ttf", 18, true, true, true);
	font[FONT_M].load("font/RictyDiminishedDiscord-Bold.ttf", 20, true, true, true);
	font[FONT_L].load("font/RictyDiminishedDiscord-Bold.ttf", 25, true, true, true);
}

/******************************
******************************/
ofApp::~ofApp()
{
	if(fp_Log) fclose(fp_Log);
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetBackgroundAuto(true);
	
	ofSetWindowTitle("AudioMeter");
	// ofSetVerticalSync(false);
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowShape(WINDOW_WIDTH, WINDOW_HEIGHT);
	ofSetEscapeQuitsApp(false);
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	ofSetCircleResolution(60);
	
	/********************
	********************/
	mainOutputSyphonServer.setName("AudioMeter"); // server name
	
	fbo_GUI.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, 4);
	
	/********************
	********************/
	image.loadImage("img.jpg");
	
	/********************
	********************/
	setup_Gui();
	
	/********************
	********************/
	// soundStream.setDeviceID(soundStream_DeviceId);
	/* set in & out respectively. */
	vector<ofSoundDevice> SoundStreamLists = soundStream.listDevices();
	if( soundStream_Input_DeviceId == -1 ){
		ofExit();
		return;
		
	}else{
		soundStream.setInDeviceID(soundStream_Input_DeviceId);
		
		if( soundStream_Output_DeviceId != -1 ){
			if(SoundStreamLists[soundStream_Output_DeviceId].name == "Apple Inc.: Built-in Output"){
				printf("!!!!! prohibited to use [%s] for output ... by SJ for safety !!!!!\n", SoundStreamLists[soundStream_Output_DeviceId].name.c_str());
			}else{
				soundStream.setOutDeviceID(soundStream_Output_DeviceId);
				b_EnableAudioOut = true;
			}
		}
	}
	
	AudioSample.resize(AUDIO_BUF_SIZE);
	
	/********************
	********************/
	pass_AudioVol->setup();
	
	/********************
	soundStream.setup()の位置に注意:最後
		audioIn()/audioOut()がstartする.
		これらのmethodは、fft_threadにaccessするので、start前にReStart()によって、fft_threadが初期化されていないと、不正accessが発生してしまう.
		
	out/in chs
		今回は、audioOut()から出力する音はないので、検討時に誤ってLoopハウリングを起こすなどの危険性に対する安全も考慮し、out ch = 0.とした.
	********************/
	if(b_EnableAudioOut)	soundStream.setup(this, 2/* out */, 2/* in */, AUDIO_SAMPLERATE, AUDIO_BUF_SIZE, AUDIO_BUFFERS);
	else					soundStream.setup(this, 0/* out */, 2/* in */, AUDIO_SAMPLERATE, AUDIO_BUF_SIZE, AUDIO_BUFFERS);
}

/******************************
description
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	Gui_Global = new GUI_GLOBAL;
	Gui_Global->setup("param", "gui.xml", 10, 10);
}

/******************************
******************************/
void ofApp::update(){
	pass_AudioVol->get_vol(Vol[CH_L], Vol[CH_R]);
}

/******************************
******************************/
void ofApp::draw_GUI_fbo(){
	fbo_GUI.begin();
		/********************
		********************/
		ofClear(0, 0, 0, 0);
		
		ofDisableAlphaBlending();
	
		if(b_Smooth)	ofEnableSmoothing();
		else			ofDisableSmoothing();
		
		draw_meter(ofPoint(-Gui_Global->ofs_of_AudioMeter, 360), 45, Vol[CH_L], "POWER : LEFT");
		draw_meter(ofPoint(1280 + Gui_Global->ofs_of_AudioMeter, 360), -(90 + 45), Vol[CH_R], "POWER : RIGHT");
		
	fbo_GUI.end();
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	draw_GUI_fbo();
	
	/********************
	********************/
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
	ofClear(0, 0, 0, 0);
	
	float alpha = ofMap(Vol[CH_L], 0, Gui_Global->Disp_VolMax, 40, 255, true);
	ofSetColor(255, 255, 255, alpha);
	/*
	float Angle_Val = ofMap(Vol[CH_L], 0, Gui_Global->Disp_VolMax, 0, 90, true);
	if(11.25 * 6 < Angle_Val)	ofSetColor(255, 10, 0, alpha);
	else						ofSetColor(255, 255, 255, alpha);
	*/
	image.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	ofSetColor(255);
	fbo_GUI.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	/********************
	********************/
	mainOutputSyphonServer.publishScreen();
	
	/********************
	********************/
	if(b_DispGui) Gui_Global->gui.draw();
}

/******************************
******************************/
void ofApp::draw_meter(const ofPoint& CoordZero, const float _Rotate, const float val, char* Title)
{
	/********************
	********************/
	const bool b_DrawFont = true;
	const float R = Gui_Global->R_of_AudioMeter;
	const float IndexHeight = 30;
	const float Scale_r = 4;
	
	const float Angle_Scale = 11.25;
	
	const float Alpha_Frame = 200;
	const float Alpha_Bar = 150;
	
	/********************
	********************/
	ofPushStyle();
	ofPushMatrix();
	
	ofTranslate(CoordZero);
	ofRotate(_Rotate);
	
		/********************
		目盛り
		********************/
		ofPushMatrix();
			char buf[BUF_SIZE_S];
			ofSetLineWidth(2);
			
			// white
			ofSetColor(255, 255, 255, Alpha_Frame);
			
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "20");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));
			
			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "10");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));

			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "7");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));

			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "5");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));

			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "3");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));

			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "0");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));

			
			// Red
			ofSetColor(255, 0, 0, Alpha_Frame);
			
			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "1");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));

			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "2");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));
			
			ofRotate(Angle_Scale);
			ofDrawCircle(0, -R, Scale_r);
			sprintf(buf, "3");
			if(b_DrawFont) font[FONT_S].drawString(buf, -font[FONT_S].stringWidth(buf)/2, -(R + IndexHeight));
		
		ofPopMatrix();
	
		/********************
		********************/
		ofPushMatrix();
			ofSetColor(255, 255, 255, Alpha_Frame);
			
			ofRotate(45);
			font[FONT_S].drawString(Title, -font[FONT_S].stringWidth(Title)/2, -R + 100);
		ofPopMatrix();
		
		/********************
		value
		********************/
		ofPushMatrix();
			const float Bar_Width = 4.0;
			const float Bar_from = 100;
			const float Bar_Out = 10;
			
			float Angle_Val = ofMap(val, 0, Gui_Global->Disp_VolMax, 0, 90, true);
			ofRotate(Angle_Val);
			
			ofSetLineWidth(1);
			ofFill();
			ofSetColor(255, 255, 255, Alpha_Bar);
			ofDrawRectangle(-Bar_Width/2, -R + Bar_from, Bar_Width, -(Bar_from + Bar_Out));
			
		ofPopMatrix();
		
		
	ofPopMatrix();
	ofPopStyle();
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key){
		case 'd':
			b_DispGui = !b_DispGui;
			break;
			
		case 's':
			b_Smooth = !b_Smooth;
			break;
			
		case ' ':
			ofSaveScreen("image.png");
			printf("> image saved\n");
			fflush(stdout);
			break;
	}

}

/******************************
audioIn/ audioOut
	同じthreadで動いている様子。
	また、audioInとaudioOutは、同時に呼ばれることはない(多分)。
	つまり、ofAppからaccessがない限り、変数にaccessする際にlock/unlock する必要はない。
	ofApp側からaccessする時は、threadを立てて、安全にpassする仕組みが必要
******************************/
void ofApp::audioIn(float *input, int bufferSize, int nChannels)
{
	/********************
	-1 < input < 1
	********************/
    for (int i = 0; i < bufferSize; i++) {
		AudioCh_L.x_in(input[2*i]);
		AudioCh_R.x_in(input[2*i+1]);
		
		// to pass to audioOut.
        AudioSample.Left[i] = input[2*i];
		AudioSample.Right[i] = input[2*i+1];
    }
	
	/********************
	1 process / block.
	********************/
	pass_AudioVol->update__vol(AudioCh_L.get_y(), AudioCh_R.get_y());
}  

/******************************
******************************/
void ofApp::audioOut(float *output, int bufferSize, int nChannels)
{
	/********************
	********************/
    for (int i = 0; i < bufferSize; i++) {
		if(b_EnableAudioOut){
			output[2*i  ] = AudioSample.Left[i];
			output[2*i+1] = AudioSample.Right[i];
		}else{
			output[2*i  ] = 0; // L
			output[2*i+1] = 0; // R
		}
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
