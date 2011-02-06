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
// 	FILE NAME: 	GLB_DATA.C
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
#include <stdlib.h>
#include <m8c.h>

#include "glb_data.h"
#include "PSoCAPI.h"

//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------
										// Timer variables managed by Sleep_Timer
volatile BYTE g_bIddleCounter = 0;
volatile BYTE g_bMeasureCounter = 0;
volatile BYTE g_bDebounceCounter = 0;
volatile BYTE g_bSpeedKeyCounter = 0;

BYTE g_bScanning = FALSE;				// TRUE while scanning
BYTE g_bIsCalibrated;					// TRUE if calibrated
BYTE g_bGainDDS[BAND_MAX];				// DDS PGA's settings for each band
										// Correction values for each band
BRIDGE_CORRECT g_xBandCorrFactor[BAND_MAX];
BRIDGE_CORRECT g_xBridgeCorrect;		// Current correction values
BRIDGE_VOLTAGES g_xBridgeMeasure;		// Measured values
DWORD g_dwSaveFreqBand[BAND_MAX];		// Stores frequency val when switching bands
CONFIG_DATA g_xConf;					// Configuration data

										// Calculated impedance values
WORD gwSwr;
WORD gwZ;
WORD gwX;
WORD gwR;
WORD gwL;
WORD gwC;

										// Band limits definition table
BAND_LIMITS const g_xBandLimits[BAND_MAX] =
{
	{10,18,20},		//160m
	{20,37,50},		//80m
	{50,71,80},		//40m
	{80,101,110},	//30m
	{110,120,130},	//25m
	{130,141,170},	//20m
	{170,181,190},	//17m
	{190,210,230},	//15m
	{230,249,260},	//12m
	{260,270,280},	//11m
	{280,290,310},	//10m
	{310,355,400},	//8m
	{400,445,490},	//7m
	{490,510,530}	//6m
};

										// PGA DDS gain settings table
GAIN_DDS const g_xGainDds[GAIN_SETTINGS_MAX] =
{
	{PGA_DDS_1_G0_56,PGA_DDS_2_G0_50},	//0.56x0.50=0.28
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_50},	//0.5x0.50=0.25
	{PGA_DDS_1_G0_56,PGA_DDS_2_G0_43},	//0.56x0.43=0.2408
	{PGA_DDS_1_G0_62,PGA_DDS_2_G0_37},	//0.62x0.37=0.2294
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_43},	//0.5x0.43=0.215
	{PGA_DDS_1_G0_56,PGA_DDS_2_G0_37},	//0.56x0.37=0.2072
	{PGA_DDS_1_G0_62,PGA_DDS_2_G0_31},	//0.62x0.31=0.1922
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_37},	//0.5x0.37=0.185
	{PGA_DDS_1_G0_56,PGA_DDS_2_G0_31},	//0.56x0.31=0.1736
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_31},	//0.5x0.31=0.155
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_25},	//0.5x0.25=0.125
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_18},	//0.5x0.18=0.09
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_12},	//0.5x0.12=0.06
	{PGA_DDS_1_G0_50,PGA_DDS_2_G0_06},	//0.5x0.06=0.03
	{PGA_DDS_1_G0_43,PGA_DDS_2_G0_06},	//0.43x0.06=0.0258
	{PGA_DDS_1_G0_37,PGA_DDS_2_G0_06},	//0.37x0.06=0.0222
	{PGA_DDS_1_G0_25,PGA_DDS_2_G0_06},	//0.25x0.06=0.015
};

										// Default band settings
const BRIDGE_CORRECT_DEFAULTS g_xDefBandSettings[BAND_MAX] =
{
	// Gain, VR.Slope, VR.Offset, VZ.Slope, VZ.Offset, VA.Slope, VA.Offset,
	{2  ,  {{-161 ,1060  }, {-266 ,1075  }, {-31  , 1024}} },		//BAND_160M
	{2  ,  {{-173 ,1065  }, {-242 ,1070  }, {-31  , 1026}} },		//BAND_80M
	{2  ,  {{-174 ,1066  }, {-244 ,1071  }, {15   , 1021}} },		//BAND_40M
	{3  ,  {{-158 ,1064  }, {-229 ,1070  }, {125  , 1005}} },		//BAND_30M
	{3  ,  {{-198 ,1073  }, {-241 ,1073  }, {223  , 993 }} },		//BAND_25M
	{3  ,  {{-175 ,1069  }, {-209 ,1066  }, {244  , 991 }} },		//BAND_20M
	{4  ,  {{-147 ,1064  }, {-169 ,1059  }, {466  , 956 }} },		//BAND_17M
	{4  ,  {{-163 ,1069  }, {-174 ,1060  }, {666  , 931 }} },		//BAND_15M
	{5  ,  {{-176 ,1072  }, {-156 ,1056  }, {1010 , 885 }} },		//BAND_12M
	{6  ,  {{-133 ,1062  }, {-151 ,1056  }, {1129 , 858 }} },		//BAND_11M
	{6  ,  {{-160 ,1069  }, {-117 ,1048  }, {1320 , 835 }} },		//BAND_10M
	{7  ,  {{-125 ,1061  }, {-74  ,1041  }, {1937 , 749 }} },		//BAND_8M
	{9  ,  {{-60  ,1041  }, {18   ,1022  }, {2898 , 595 }} },		//BAND_7M
	{10 ,  {{-11  ,1023  }, {71   ,1010  }, {3721 , 468 }} }		//BAND_6M
};

