/************************************************************
************************************************************/
#include <stdio.h>

#include "DigitalFilter.h"

/************************************************************
***********************************************************/
// static const double k_ave = 0.001;
static const double k_ave = 0.0001;

/************************************************************
************************************************************/

/******************************
explanation
	_xを入力し、4次のIIR filterを通った後のy[0]まで算出.
******************************/
double IIR_4::x_in(double _x)
{
	return iir2_1.x_in(iir2_0.x_in(_x));
}

/******************************
explanation
	移動平均後の y を出力.
******************************/
double IIR_4::get_y()
{
	/********************
	Is_move_aveの設定はコンストラクタで設定済み
	********************/
	return iir2_1.get_y();
}

/******************************
explanation
	_xを入力し、2次のIIR filterを通った後のy[0]まで算出.
******************************/
double IIR_2::x_in(double _x)
{
	/********************
	********************/
	for(int i = NUM_HISTORY - 1; 0 < i; i--){
		x[i] = x[i - 1];
		y[i] = y[i - 1];
	}
	x[0] = _x;
	y[0] = -Cb * y[1] - Cc * y[2] + Cd * x[0] + Ce * x[1] + Cf * x[2];
	
	/********************
	********************/
	/********************
	********************/
	if(Is_move_ave)	last_y = (1 - Gui_Global->DigitalFilter__k_ave) * last_y + Gui_Global->DigitalFilter__k_ave * abs(y[0]);
	else			last_y = y[0];
	
	return last_y;
}

/******************************
explanation
	移動平均後の y を出力.
******************************/
double IIR_2::get_y()
{
	return last_y;
}

/******************************
description
	_xを入力し、1次のIIR filterを通った後のy[0]まで算出.
******************************/
double IIR_1::x_in(double _x)
{
	/********************
	********************/
	for(int i = NUM_HISTORY - 1; 0 < i; i--){
		x[i] = x[i - 1];
		y[i] = y[i - 1];
	}
	x[0] = _x;
	y[0] = -Cb * y[1] + Cc * x[0] + Cd * x[1];
	
	/********************
	********************/
	if(Is_move_ave)	last_y = (1 - Gui_Global->DigitalFilter__k_ave) * last_y + Gui_Global->DigitalFilter__k_ave * abs(y[0]);
	else			last_y = y[0];
	
	return last_y;
}

/******************************
explanation
	移動平均後の y を出力.
******************************/
double IIR_1::get_y()
{
	return last_y;
}

