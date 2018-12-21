/************************************************************
************************************************************/
#include "th_AudioVol.h"
#include "stdlib.h"

/************************************************************
************************************************************/

/******************************
******************************/
THREAD_AUDIO_VOL::THREAD_AUDIO_VOL()
{
}

/******************************
******************************/
THREAD_AUDIO_VOL::~THREAD_AUDIO_VOL()
{
}

/******************************
******************************/
void THREAD_AUDIO_VOL::threadedFunction()
{
	while(isThreadRunning()) {
		lock();
		
		unlock();
		
		
		sleep(1);
	}
}

/******************************
******************************/
void THREAD_AUDIO_VOL::exit()
{
	this->lock();
	
	this->unlock();
}

/******************************
******************************/
void THREAD_AUDIO_VOL::setup()
{
	this->lock();
	for(int i = 0; i < NUM_CHs; i++){
		Vol[i] = 0;
	}
	this->unlock();
}



/******************************
******************************/
void THREAD_AUDIO_VOL::get_vol(double& vol_L, double& vol_R)
{
	this->lock();
	
	vol_L = Vol[CH_L];
	vol_R = Vol[CH_R];
	
	this->unlock();
}

/******************************
******************************/
void THREAD_AUDIO_VOL::update__vol(const float _vol_L, const float _vol_R)
{
	this->lock();
		
		Vol[CH_L] = sj_max(_vol_L, Vol[CH_L] * Gui_Global->Limit_volDown_Speed);
		Vol[CH_R] = sj_max(_vol_R, Vol[CH_R] * Gui_Global->Limit_volDown_Speed);
		
	this->unlock();
}


