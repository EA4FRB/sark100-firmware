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
// 	FILE NAME: 	CALIBRATE_REFRECTOMETER.C
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

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"
#include "Lcd.h"

#include "dds.h"
#include "util.h"
#include "glb_data.h"
#include "keypad.h"
#include "msg_generic.h"
#include "buzzer.h"
#include "storage.h"
#include "display.h"
#include "calibrate_reflectometer.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define VF_REFERENCE_LEVEL	4000

//-----------------------------------------------------------------------------
//  Private data:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calibrate_Reflectometer()
//
//  DESCRIPTION:
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void Calibrate_Reflectometer (void)
{
	BYTE bBand;
	BYTE bErrorVf = FALSE;
DWORD dwTemp;
	
//@TODO: Save values in case of cancelation

	// Set default correction factors
	g_xBridgeCorrect.Vf = 100;
	g_xBridgeCorrect.Vz = 1;
	g_xBridgeCorrect.Vr = 1;
	g_xBridgeCorrect.Va = 1;

	do
	{
		DISP_Clear();

		DDS_Set(0);
		Delay_Ms(100);

		//No signal, no load
		//Get offset readings
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(gOpenLoadStr);
		BUZZ_Beep();
		if ( KEYPAD_WaitKey(TIME_WAIT_KEY_S) == KBD_UP )
			break;
		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(gInProgressStr);

		g_xBridgeCorrect.Vz = 1;	
		g_xBridgeCorrect.Vr = 1;
		g_xBridgeCorrect.Va = 1;
		g_xBridgeCorrect.Vf = 1;

		g_xBridgeOffset.Vz = 0;			// Do_Measure does offset adjustment so set default to 0
		g_xBridgeOffset.Vr = 0;
		g_xBridgeOffset.Va = 0;
		g_xBridgeOffset.Vf = 0;
	
		Do_Measure();
		g_xBridgeOffset = g_xBridgeMeasure;
		
		//
		//Adjust Vf
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			BYTE bGainIdx;
			
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			for (bGainIdx=0;bGainIdx<GAIN_SETTINGS_MAX;bGainIdx++)
			{
				PGA_DDS_1_SetGain(g_xGainDds[bGainIdx].bGain1);
				PGA_DDS_2_SetGain(g_xGainDds[bGainIdx].bGain2);
				Delay_Ms(100);
				Do_Measure();

				if (g_xBridgeMeasure.Vf > (VF_REFERENCE_LEVEL-g_xBridgeOffset.Vf))
				{
					g_bGainDDS[bBand] = bGainIdx;
					break;
				}					
			}
			if (bGainIdx==GAIN_SETTINGS_MAX)
			{
				bErrorVf = TRUE;
				break;
			}	
		}			
		if (bErrorVf)
		{
			BUZZ_BeepError();
	
			LCD_Position(0, 0);
			LCD_PrCString(gErrorAdjustVfStr);
			LCD_Position(1, 0);
			LCD_PrCString(gPressAnyKeyStr);
			// Wait key press
			KEYPAD_WaitKey(TIME_WAIT_KEY_S);
			DISP_Clear();
			break;
		}

		//50ohm load
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(g50OhmLoadStr);
		BUZZ_Beep();
		if ( KEYPAD_WaitKey(TIME_WAIT_KEY_S) == KBD_UP )
			break;
		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(gInProgressStr);

		//At each band frequency, ...
        //Determine correction factor for Vz and Va to be 1/2 Vf (using 50-ohm load)
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(100);

			Do_Measure();
			
			dwTemp = g_xBandCorrFactor[bBand].Vz = (g_xBridgeMeasure.Vf*100)/(2*g_xBridgeMeasure.Vz);
			dwTemp = g_xBandCorrFactor[bBand].Va = (g_xBridgeMeasure.Vf*100)/(2*g_xBridgeMeasure.Va);
        }

		//150ohm load
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(g150OhmLoadStr);
		BUZZ_Beep();
		if ( KEYPAD_WaitKey(TIME_WAIT_KEY_S) == KBD_UP )
			break;
		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(gInProgressStr);

		//At each band frequency, ...
        //Determine correction factor for Vr to be 1/2 Vf (using 150-ohm load)
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(100);
			Do_Measure();
			
			dwTemp = g_xBandCorrFactor[bBand].Vr = (g_xBridgeMeasure.Vf*100)/(2*g_xBridgeMeasure.Vr);
        }

		//Store data	
		g_bIsCalibrated = TRUE;
		STR_SaveCalibration();			

		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(gDoneStr);
		BUZZ_Beep();
		KEYPAD_WaitKey(TIME_WAIT_KEY_S);
	}
	while (FALSE);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Adjust_Dds_Gain()
//
//  DESCRIPTION:
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void Adjust_Dds_Gain (BYTE bBand)
{
	PGA_DDS_1_SetGain(g_xGainDds[g_bGainDDS[bBand]].bGain1);
	PGA_DDS_2_SetGain(g_xGainDds[g_bGainDDS[bBand]].bGain2);
}