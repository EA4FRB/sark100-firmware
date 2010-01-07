//*****************************************************************************/
//  This file is a part of the "EA4FRB SWR Analyzer firmware"
//
//  Copyright © 2010 Melchor Varela - EA4FRB.  All rights reserved.
//  Melchor Varela, Madrid, Spain.
//  melchor.varela@gmail.com
//
//  "EA4FRB SWR Analyzer firmware" is free software: you can redistribute it 
//  and/or modify it under the terms of the GNU General Public License as 
//  published by the Free Software Foundation, either version 3 of the License, 
//  or (at your option) any later version.
//
//  "EA4FRB SWR Analyzer firmware" is distributed in the hope that it will be 
//  useful,  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with "EA4FRB SWR Analyzer firmware".  If not, 
//  see <http://www.gnu.org/licenses/>.
//*****************************************************************************/
//*****************************************************************************/
//
//	PROJECT:	SWR Analyzer
// 	FILE NAME: 	GLB_DATA.C
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
#include <stdlib.h>
#include <m8c.h>

#include "glb_data.h"
#include "PSoCAPI.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------
volatile BYTE g_bIddleCounter = 0;
volatile BYTE g_bMeasureCounter = 0;
volatile BYTE g_bDebounceCounter = 0;

BYTE g_bIsCalibrated;
BYTE g_bGainDDS[BAND_MAX];
BRIDGE_VOLTAGES g_xBandCorrFactor[BAND_MAX];
BRIDGE_VOLTAGES g_xBridgeCorrect;
BRIDGE_VOLTAGES g_xBridgeOffset;
BRIDGE_VOLTAGES g_xBridgeMeasure;
DWORD g_dwSaveFreqBand[BAND_MAX];
CONFIG_DATA g_xConf;
WORD gwSwr;
WORD gwZ;
WORD gwX;
WORD gwR;
WORD gwL;
WORD gwC;


BAND_LIMITS const g_xBandLimits[BAND_MAX] =
{
	{10,18,20},		//160m
	{20,37,50},		//80m
	{50,70,80},		//40m
	{80,100,110},	//30m
	{110,120,130},	//25m
	{130,150,170},	//20m
	{170,180,190},	//18m
	{190,210,230},	//15m
	{230,240,250},	//12m
	{250,280,310},	//10m
	{500,520,540}	//6M
};

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

const g_bDefGainDdsIdx[BAND_MAX] =
{
	3,	//BAND_160M
	3,	//BAND_80M
	3,	//BAND_40M
	3,	//BAND_30M
	3, 	//BAND_25M
	3,	//BAND_20M
	4,	//BAND_18M
	4,	//BAND_15M
	5,	//BAND_12M
	6,	//BAND_10M
	10	//BAND_6M
};
