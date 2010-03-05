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
// 	FILE NAME: 	CALIBRATE_REFRECTOMETER.C
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//	Calibration routines
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC 2009	Creation
//
//*****************************************************************************/

#include <m8c.h>        				// Part specific constants and macros
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
#include "calcs.h"

//#define DEBUG	1						// Uncomment to debug calibration results on USB

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define VF_REFERENCE_LEVEL	4000		// Full scale value
#define SWR_274_LOAD		548			// Expected SWR at 274-ohm load
#define MAX_ITER_SWR		50			// Maximum number of iterations looking for best SWR

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calibrate_Reflectometer()
//
//  DESCRIPTION:
//
//	Calibration routine
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
										// Set default correction factors
	g_xBridgeCorrect.Vf = 100;
	g_xBridgeCorrect.Vz = 1;
	g_xBridgeCorrect.Vr = 1;
	g_xBridgeCorrect.Va = 1;

	do
	{
		DISP_Clear();

		DDS_Set(0);
		Delay_Ms(200);
										// No signal, no load
										// Get offset readings
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

		DDS_Set(10000000);
		Delay_Ms(500);
										//
										// Adjust Vf
		for (bBand=BAND_MAX-1; bBand>=0;bBand--)
		{
			BYTE bGainIdx;

			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			for (bGainIdx=0;bGainIdx<GAIN_SETTINGS_MAX;bGainIdx++)
			{
				PGA_DDS_1_SetGain(g_xGainDds[bGainIdx].bGain1);
				PGA_DDS_2_SetGain(g_xGainDds[bGainIdx].bGain2);
				Delay_Ms(200);
				Do_Measure();
				if (g_xBridgeMeasure.Vf >= (VF_REFERENCE_LEVEL-g_xBridgeOffset.Vf))
				{
					g_bGainDDS[bBand] = bGainIdx;
					break;
				}
			}
			if (bGainIdx==GAIN_SETTINGS_MAX)
			{
				g_bGainDDS[bBand] = GAIN_SETTINGS_MAX-1;
				break;
			}
		}
#if 1		
										//
										// 50ohm load
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

										// At each band frequency, ...
        								// Determine correction factor for Vz and Vr to be 1/2 Vf (using 50-ohm load)
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(200);

			Do_Measure();

			g_xBandCorrFactor[bBand].Vz = (g_xBridgeMeasure.Vf*100)/(2*g_xBridgeMeasure.Vz);
			g_xBandCorrFactor[bBand].Va = (g_xBridgeMeasure.Vf*100)/(2*g_xBridgeMeasure.Va);
        }
#endif
#if 0
										//
										// 150ohm load
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
										// At each band frequency, ...
        								// Determine correction factor for Vz to be 3/4 Vf and Va to be 1/4 Vf (using 150-ohm load)
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(200);

			Do_Measure();

			g_xBandCorrFactor[bBand].Vz = (g_xBridgeMeasure.Vf*100*3)/(4*g_xBridgeMeasure.Vz);
			g_xBandCorrFactor[bBand].Va = (g_xBridgeMeasure.Vf*100)/(4*g_xBridgeMeasure.Va);
        }
#endif
										// 274ohm load
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(g274OhmLoadStr);
		BUZZ_Beep();
		if ( KEYPAD_WaitKey(TIME_WAIT_KEY_S) == KBD_UP )
			break;
		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(gInProgressStr);

										// At each band frequency, ...
        								// Determine correction factor for SWR to be 548 (using 274-ohm load)
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			BYTE ii;
			
			g_xBandCorrFactor[bBand].Vr = 100;
			g_xBridgeCorrect = g_xBandCorrFactor[bBand];
			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(200);
			for (ii=0;ii<MAX_ITER_SWR;ii++)
			{
				Do_Measure();
				gwSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
				if (gwSwr == SWR_274_LOAD)
					break;
				else if (gwSwr>SWR_274_LOAD)
					g_xBridgeCorrect.Vr--;
				else
					g_xBridgeCorrect.Vr++;
			}						
			g_xBandCorrFactor[bBand] = g_xBridgeCorrect;
        }

										// Store data
		g_bIsCalibrated = TRUE;
		STR_SaveCalibration();

										// Finish, alert user
		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gConfigCalibStr);
		LCD_Position(1, 0);
		LCD_PrCString(gDoneStr);
		BUZZ_Beep();
		KEYPAD_WaitKey(TIME_WAIT_KEY_S);
	}
	while (FALSE);

#ifdef DEBUG
{
	BYTE szMsg[20];

	UART_CmdReset(); 					// Initialize receiver/cmd buffer
	UART_IntCntl(UART_ENABLE_RX_INT); 	// Enable RX interrupts
	UART_Start(UART_PARITY_NONE); 		// Enable UART
	M8C_EnableGInt ;
	UART_PutChar(12); 					// Clear the screen

										// Print offset
	UART_CPutString("Offset, VF=");
	ltoa(szMsg, g_xBridgeOffset.Vf, 10);
	UART_PutString(szMsg);
	UART_CPutString(", VR=");
	ltoa(szMsg, g_xBridgeOffset.Vr, 10);
	UART_PutString(szMsg);
	UART_CPutString(", VA=");
	ltoa(szMsg, g_xBridgeOffset.Va, 10);
	UART_PutString(szMsg);
	UART_CPutString(", VZ=");
	ltoa(szMsg, g_xBridgeOffset.Vz, 10);
	UART_PutString(szMsg);
	UART_PutCRLF();

										// Print correction factors
	for (bBand=0; bBand<BAND_MAX;bBand++)
	{
		UART_CPutString("Band:");
		itoa(szMsg, bBand, 10);
		UART_PutString(szMsg);

		UART_CPutString(", Gain=");
		itoa(szMsg, g_bGainDDS[bBand], 10);
		UART_PutString(szMsg);
		
		UART_CPutString(", CorrVR=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].Vr, 10);
		UART_PutString(szMsg);
		
		UART_CPutString(", CorrVZ=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].Vz, 10);
		UART_PutString(szMsg);

		UART_CPutString(", CorrVA=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].Va, 10);
		UART_PutString(szMsg);

		UART_PutCRLF();
	}	
	UART_Stop();
}	
#endif
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Adjust_Dds_Gain()
//
//  DESCRIPTION:
//
//	Adjust PGA's gain for setting right DDS level
//
//  ARGUMENTS:
//  	bBand	Integer describing band number
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


