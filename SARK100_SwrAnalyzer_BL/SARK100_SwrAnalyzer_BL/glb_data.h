//*****************************************************************************/
//  This file is a part of the "SARK100 SWR Analyzer firmware"
//
//  Copyright © 2010 Melchor Varela - EA4FRB.  All rights reserved.
//  Melchor Varela, Madrid, Spain.
//  melchor.varela@gmail.com
//
//  "SARK100 SWR Analyzer firmware" is free software: you can redistribute it 
//  and/or modify it under the terms of the GNU General Public License as 
//  published by the Free Software Foundation, either version 3 of the License, 
//  or (at your option) any later version.
//
//  "SARK100 SWR Analyzer firmware" is distributed in the hope that it will be 
//  useful,  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with "SARK100 SWR Analyzer firmware".  If not, 
//  see <http://www.gnu.org/licenses/>.
//*****************************************************************************/
//*****************************************************************************/
//
//	PROJECT:	SARK100 SWR Analyzer
// 	FILE NAME: 	GLB_DATA.H
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC 2009	Creation
//
//*****************************************************************************/
#ifndef __GLB_DATA_H__
#define __GLB_DATA_H__

#include <stdlib.h>
#include <m8c.h>

#include "measure.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define TIME_TO_IDDLE_S		30
#define TIME_WAIT_KEY_S		30

#define	MEASURE_PERIOD		16	//Units 1/8 sec

#define BAND_FREQ_MULT		(100000)

#define GAIN_SETTINGS_MAX	14

#define SWR_MAX				999

typedef enum {
	CONFIG_PCLINK = 0,
	CONFIG_STEP,
	CONFIG_CALIB,
	CONFIG_SW_LOAD,
	
	CONFIG_MAX		
} CONFIG_DEFS;	

typedef enum {
	MODE_SWR = 0,
	MODE_IMP,
	MODE_CAP,
	MODE_IND,
	MODE_OFF,
	
	MODE_MAX,
	
	MODE_VFO	// Mode VFO not available yet
} MODE_DEFS;	

typedef enum {
	STEP_10HZ,
	STEP_100HZ,
	STEP_1KHZ,
	STEP_10KHZ,
	STEP_100KHZ,
	
	STEP_MAX		
} STEP_DEFS;	

typedef enum {
	BAND_160M,
	BAND_80M,
	BAND_40M,
	BAND_30M,
	BAND_25M,
	BAND_20M,
	BAND_18M,
	BAND_15M,
	BAND_12M,
	BAND_10M,
	BAND_6M,
	
	BAND_MAX		
} BAND_DEFS;	

typedef struct
{
	WORD low;
	WORD middle;
	WORD high;
} BAND_LIMITS;

typedef struct
{
	BYTE bGain1;
	BYTE bGain2;
} GAIN_DDS;

typedef struct 
{
	BYTE bStep;
} CONFIG_DATA;

//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------
extern volatile BYTE g_bIddleCounter;
extern volatile BYTE g_bMeasureCounter;
extern volatile BYTE g_bDebounceCounter;

extern BYTE g_bIsCalibrated;
extern BYTE g_bGainDDS[BAND_MAX];
extern BRIDGE_VOLTAGES g_xBandCorrFactor[BAND_MAX];
extern BRIDGE_VOLTAGES g_xBridgeCorrect;
extern BRIDGE_VOLTAGES g_xBridgeOffset;
extern BRIDGE_VOLTAGES g_xBridgeMeasure;
extern CONFIG_DATA g_xConf;
extern BAND_LIMITS const g_xBandLimits[BAND_MAX];
extern DWORD g_dwSaveFreqBand[BAND_MAX];
extern GAIN_DDS const g_xGainDds[GAIN_SETTINGS_MAX];
extern const g_bDefGainDdsIdx[BAND_MAX];

extern WORD gwSwr;
extern WORD gwZ;
extern WORD gwX;
extern WORD gwR;
extern WORD gwL;
extern WORD gwC;

//-----------------------------------------------------------------------------
#endif