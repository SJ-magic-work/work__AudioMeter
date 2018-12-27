/************************************************************
IIR Filter design with the tool below
	http://hs-soft.exout.net/iir.php

Digital Filterの形
	http://www.picfun.com/dsPIC/dspsw/dspprog02.html
************************************************************/
#pragma once


/************************************************************
************************************************************/
#include <math.h>

#include "ofMain.h"

#include "sj_common.h"


/************************************************************
class
************************************************************/

/**************************************************
**************************************************/
class IIR_1{
private:
	/**************************************************
	**************************************************/
	enum{
		NUM_HISTORY = 2,
	};
	const double Ca, Cb, Cc, Cd; // 1次 IIR Filter
	
	/**************************************************
	for z^-1
	**************************************************/
	double x[NUM_HISTORY];
	double y[NUM_HISTORY];
	
	/**************************************************
	for moving average
	**************************************************/
	const bool Is_LastBlock;
	double last_y;

	
public:
	IIR_1(double _Ca, double _Cb, double _Cc, double _Cd, bool _Is_LastBlock = true)
	: Ca(_Ca), Cb(_Cb), Cc(_Cc), Cd(_Cd)
	, Is_LastBlock(_Is_LastBlock), last_y(0)
	{
		for(int i = 0; i < NUM_HISTORY; i++){
			x[i] = 0; y[i] = 0;
		}
	}
	
	double x_in(double _x);
	
	double get_y();
};

/**************************************************
**************************************************/
class IIR_2{
private:
	/****************************************
	****************************************/
	enum{
		NUM_HISTORY = 3,
	};
	const double Ca, Cb, Cc, Cd, Ce, Cf; // 2次 IIR Filter
	
	/****************************************
	for z^-1
	****************************************/
	double x[NUM_HISTORY];
	double y[NUM_HISTORY];
	
	/****************************************
	for moving average
	****************************************/
	const bool Is_LastBlock;
	double last_y;

	
public:
	IIR_2(double _Ca, double _Cb, double _Cc, double _Cd, double _Ce, double _Cf, bool _Is_LastBlock = true)
	: Ca(_Ca), Cb(_Cb), Cc(_Cc), Cd(_Cd), Ce(_Ce), Cf(_Cf)
	, Is_LastBlock(_Is_LastBlock), last_y(0)
	{
		for(int i = 0; i < NUM_HISTORY; i++){
			x[i] = 0; y[i] = 0;
		}
	}
	
	double x_in(double _x);
	
	double get_y();
};

/**************************************************
**************************************************/
class IIR_4{
private:
	/****************************************
	****************************************/
	IIR_2 iir2_0;
	IIR_2 iir2_1;
	

	
public:
	IIR_4(	double _Ca0, double _Cb0, double _Cc0, double _Cd0, double _Ce0, double _Cf0,
			double _Ca1, double _Cb1, double _Cc1, double _Cd1, double _Ce1, double _Cf1,
			bool _Is_LastBlock = true)
	: iir2_0(_Ca0, _Cb0, _Cc0, _Cd0, _Ce0, _Cf0, false)
	, iir2_1(_Ca1, _Cb1, _Cc1, _Cd1, _Ce1, _Cf1, _Is_LastBlock)
	{
	}
	
	double x_in(double _x);
	double get_y();
};


