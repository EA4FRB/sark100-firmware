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

BYTE g_bIsCalibrated;					// TRUE if calibrated
BYTE g_bGainDDS[BAND_MAX];				// DDS PGA's settings for each band
										// Correction values for each band
BRIDGE_CORRECT g_xBandCorrFactor[BAND_MAX];
BRIDGE_CORRECT g_xBridgeCorrect;		// Current correction values
BRIDGE_VOLTAGES g_xBridgeOffset;		// Measurement offset
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
	{310,400,490},	//8m
	{490,510,530}	//6M
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
};

										// Default band settings
const BRIDGE_CORRECT_DEFAULTS g_xDefBandSettings[BAND_MAX] =
{
	// Gain, VR.Slope, VR.Offset, VZ.Slope, VZ.Offset, VA.Slope, VA.Offset,
	{ 2,  {{-3362, 1137}, {-2055, 1060}, {-9256, 1184}} },	//BAND_160M
	{ 2,  {{-3841, 1149}, {-1980, 1060}, {-9436, 1187}} },	//BAND_80M
	{ 2,  {{-3889, 1150}, {-2087, 1063}, {-8615, 1176}} },	//BAND_40M
	{ 3,  {{-3304, 1139}, {-1943, 1061}, {-7067, 1156}} },	//BAND_30M
	{ 3,  {{-3478, 1143}, {-1790, 1058}, {-6769, 1151}} },	//BAND_25M
	{ 3,  {{-3535, 1144}, {-1649, 1056}, {-5799, 1138}} },	//BAND_20M
	{ 4,  {{-3558, 1147}, {-1256, 1048}, {-2941, 1099}} },	//BAND_17M
	{ 4,  {{-3517, 1146}, {-1562, 1056}, {-483,  1068}} },	//BAND_15M
	{ 5,  {{-3303, 1142}, {-1202, 1048}, {2471,  1027}} },	//BAND_12M
	{ 6,  {{-3052, 1139}, {-802,  1040}, {3503,  1009}} },	//BAND_11M
	{ 6,  {{-3094, 1137}, {-906,  1042}, {6085,  973 }} },	//BAND_10M
	{ 8,  {{-2736, 1126}, {277,   1019}, {16909, 821 }} },	//BAND_8M
	{ 10, {{-1783, 1092}, {1171,  998 }, {30861, 607 }} }	//BAND_6M
};

