/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxSyphon.h"

#include "sj_common.h"
#include "th_AudioVol.h"
#include "DigitalFilter.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
struct AUDIO_SAMPLE : private Noncopyable{
	vector<float> Left;
	vector<float> Right;
	
	void resize(int size){
		Left.resize(size);
		Right.resize(size);
	}
};

/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		FONT_S,
		FONT_M,
		FONT_L,
		
		NUM_FONT_SIZE,
	};
	
	/****************************************
	****************************************/
	/********************
	********************/
	ofImage image;
	
	ofxSyphonServer mainOutputSyphonServer;
	ofFbo fbo_GUI;
	
	/********************
	********************/
	int soundStream_Input_DeviceId;
	int soundStream_Output_DeviceId;
	ofSoundStream soundStream;
	
	bool b_DispGui;
	bool b_Smooth;
	
	IIR_2 AudioCh_L;
	IIR_2 AudioCh_R;
	
	AUDIO_SAMPLE AudioSample;
	bool b_EnableAudioOut;
	
	/********************
	********************/
	ofTrueTypeFont font[NUM_FONT_SIZE];
	
	/********************
	********************/
	THREAD_AUDIO_VOL* pass_AudioVol;
	double Vol[NUM_CHs];
	
	
	
	/****************************************
	****************************************/
	void setup_Gui();
	void draw_meter(const ofPoint& CoordZero, const float _Rotate, const float val, char* Title);
	void draw_GUI_fbo();
	
public:
	/****************************************
	****************************************/
	ofApp(int _soundStream_Input_DeviceId, int _soundStream_Output_DeviceId);
	~ofApp();
	
	void setup();
	void update();
	void draw();
	
	void audioIn(float *input, int bufferSize, int nChannels);
	void audioOut(float *output, int bufferSize, int nChannels);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
};
