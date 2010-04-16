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
//	Global constants and variables
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
//  Version string
//-----------------------------------------------------------------------------
#define VERSION_STR			"V09"
#define PRODUCT_NAME_STR	"SARK100"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------

#define	CORRECTION_FACTOR	1000		// Correction factor used for calibration

#define TIME_WAIT_KEY_S		30			// Time to wait for key when dialog, units of seconds

#define	MEASURE_PERIOD		8			// Measurements sample period, units 1/8 sec

#define BAND_FREQ_MULT		(100000)	// Frequency multiplier for frequency tables

#define GAIN_SETTINGS_MAX	17			// Number of gain settings for DDS PGAs

#define SWR_MAX				999			// Maximum supported SWR value

typedef enum {							// Configuration menu
	CONFIG_PCLINK = 0,
	CONFIG_STEP,
	CONFIG_IDDLE,
	CONFIG_CALIB,
	CONFIG_SW_LOAD,

	CONFIG_MAX
} CONFIG_DEFS;

typedef enum {							// Mode menu
	MODE_SWR = 0,
	MODE_IMP,
	MODE_CAP,
	MODE_IND,
//	MODE_VFO,
	MODE_OFF,

	MODE_MAX
} MODE_DEFS;

typedef enum {							// Step values
	STEP_10HZ,
	STEP_100HZ,
	STEP_1KHZ,
	STEP_10KHZ,
	STEP_100KHZ,

	STEP_MAX
} STEP_DEFS;

typedef enum {							// User iddle timeout values
	USER_IDDLE_NONE,
	USER_IDDLE_30S,
	USER_IDDLE_60S,
	USER_IDDLE_90S,

	USER_IDDLE_MAX
} USER_IDDLE_DEFS;

typedef enum {							// Band values
	BAND_160M,
	BAND_80M,
	BAND_40M,
	BAND_30M,
	BAND_25M,
	BAND_20M,
	BAND_17M,
	BAND_15M,
	BAND_12M,
	BAND_11M,
	BAND_10M,
	BAND_8M,
	BAND_6M,

	BAND_MAX
} BAND_DEFS;

typedef struct							// Band limits definition
{
	WORD low;
	WORD middle;
	WORD high;
} BAND_LIMITS;

typedef struct							// PGA's DDS gain settings
{
	BYTE bGain1;
	BYTE bGain2;
} GAIN_DDS;

typedef struct							// Configuration data stored in EEPROM
{
	BYTE bStep;
	BYTE bUserIddle;
} CONFIG_DATA;

typedef struct							// Two point calibration data
{
	INT iSlope;
	INT iOffset;
} CORRECT_DATA;

typedef struct							// Complete correction data
{
	CORRECT_DATA xVr;
	CORRECT_DATA xVz;
	CORRECT_DATA xVa;
} BRIDGE_CORRECT;

typedef struct
{
	BYTE bGain;
	BRIDGE_CORRECT xBridgeCorrect;
} BRIDGE_CORRECT_DEFAULTS;

//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------
extern volatile BYTE g_bIddleCounter;
extern volatile BYTE g_bMeasureCounter;
extern volatile BYTE g_bDebounceCounter;
extern volatile BYTE g_bSpeedKeyCounter;

extern BYTE g_bScanning;
extern BYTE g_bIsCalibrated;
extern BYTE g_bGainDDS[BAND_MAX];
extern BRIDGE_CORRECT g_xBandCorrFactor[BAND_MAX];
extern BRIDGE_CORRECT g_xBridgeCorrect;
extern BRIDGE_VOLTAGES g_xBridgeOffset;
extern BRIDGE_VOLTAGES g_xBridgeMeasure;
extern CONFIG_DATA g_xConf;
extern BAND_LIMITS const g_xBandLimits[BAND_MAX];
extern DWORD g_dwSaveFreqBand[BAND_MAX];
extern GAIN_DDS const g_xGainDds[GAIN_SETTINGS_MAX];
extern const BRIDGE_CORRECT_DEFAULTS g_xDefBandSettings[BAND_MAX];

extern WORD gwSwr;
extern WORD gwZ;
extern WORD gwX;
extern WORD gwR;
extern WORD gwL;
extern WORD gwC;

//-----------------------------------------------------------------------------
#endif