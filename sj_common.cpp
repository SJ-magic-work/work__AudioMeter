/************************************************************
************************************************************/
#include "sj_common.h"

/************************************************************
************************************************************/
/********************
********************/
int GPIO_0 = 0;
int GPIO_1 = 0;

/********************
********************/
GUI_GLOBAL* Gui_Global = NULL;

FILE* fp_Log = NULL;


/************************************************************
function
************************************************************/
/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt)
{
	double Alpha;
	if((Alpha_dt <= 0) || (Alpha_dt < dt))	Alpha = 1;
	else									Alpha = 1/Alpha_dt * dt;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha)
{
	if(Alpha < 0)		Alpha = 0;
	else if(1 < Alpha)	Alpha = 1;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double sj_max(double a, double b)
{
	if(a < b)	return b;
	else		return a;
}

/************************************************************
class
************************************************************/

/******************************
******************************/
void GUI_GLOBAL::setup(string GuiName, string FileName, float x, float y)
{
	/********************
	********************/
	gui.setup(GuiName.c_str(), FileName.c_str(), x, y);
	
	/********************
	********************/
	/* */

	gui.add(DigitalFilter__k_ave.setup("DigitalFilter__k_ave", 0.0001, 0.00001, 0.001));
	gui.add(Limit_volDown_Speed.setup("Limit_volDown_Speed", 0.7, 0, 0.9)); // 0 is fastest.

	gui.add(Disp_VolMax.setup("Disp_VolMax", 0.5, 0, 1.0));
	
	gui.add(R_of_AudioMeter.setup("R_of_AudioMeter", 330, 100, 500));
	gui.add(ofs_of_AudioMeter.setup("ofs_of_AudioMeter", 150, 100, 500));
	
	/********************
	********************/
	gui.minimizeAll();
}

