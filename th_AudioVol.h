/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"

#include "sj_common.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
class THREAD_AUDIO_VOL : public ofThread, private Noncopyable{
private:
	/****************************************
	****************************************/
	/********************
	********************/
	double Vol[NUM_CHs];
	
	/****************************************
	function
	****************************************/
	/********************
	singleton
	********************/
	THREAD_AUDIO_VOL();
	~THREAD_AUDIO_VOL();
	THREAD_AUDIO_VOL(const THREAD_AUDIO_VOL&); // Copy constructor. no define.
	THREAD_AUDIO_VOL& operator=(const THREAD_AUDIO_VOL&); // コピー代入演算子. no define.
	
	/********************
	********************/
	void threadedFunction();
	
public:
	/****************************************
	****************************************/
	/********************
	********************/
	static THREAD_AUDIO_VOL* getInstance(){
		static THREAD_AUDIO_VOL inst;
		return &inst;
	}
	
	void exit();
	void setup();
	void get_vol(double& vol_L, double& vol_R);
	
	void update__vol(const float _vol_L, const float _vol_R);
};



