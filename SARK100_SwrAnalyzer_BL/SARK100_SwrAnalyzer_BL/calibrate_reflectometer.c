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
//  Typedefs
//-----------------------------------------------------------------------------
typedef struct							// Two-point calibration vector
{
	WORD wX0;
	WORD wX1;
	WORD wY0;
	WORD wY1;
} CALIB_VECTOR;

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define SLOPE_CORRECT		10000
#define VF_REFERENCE_LEVEL	4000		// Full scale value
#define SWR_274_LOAD		548			// Expected SWR at 274-ohm load
#define SWR_150_LOAD		300			// Expected SWR at 150-ohm load
#define WAIT_TIME_MEASURE	1000		// Estabilizing time for measurement (ms)

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static void Calc_Correct (CORRECT_DATA *pxCorr, CALIB_VECTOR *pxVect);
static WORD Correct_Measure(CORRECT_DATA *pxCorr, WORD wX);
static BOOL WaitLoadUser (const char *pszText);

//-----------------------------------------------------------------------------
//  Private data
//-----------------------------------------------------------------------------
static CALIB_VECTOR xCalVz[BAND_MAX];
static CALIB_VECTOR xCalVr[BAND_MAX];
static CALIB_VECTOR xCalVa[BAND_MAX];

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

	do
	{
		DISP_Clear();

		DDS_Set(0);
		Delay_Ms(200);

										// No signal, no load
										// Get offset readings
		if (!WaitLoadUser(gOpenLoadStr))
			break;

		g_xBridgeOffset.Vz = 0;			// Do_Measure does offset adjustment so set default to 0
		g_xBridgeOffset.Vr = 0;
		g_xBridgeOffset.Va = 0;
		g_xBridgeOffset.Vf = 0;

		Do_Measure();
		g_xBridgeOffset = g_xBridgeMeasure;

		DDS_Set(10000000);
		Delay_Ms(WAIT_TIME_MEASURE);
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
				if (g_xBridgeMeasure.Vf > (VF_REFERENCE_LEVEL-g_xBridgeOffset.Vf))
				{
					if (bGainIdx < (GAIN_SETTINGS_MAX-1))
						bGainIdx++;
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

										//
										// 50ohm load
		if (!WaitLoadUser(g50OhmLoadStr))
			break;
										// At each band frequency, ...
        								// Determine correction factor for Vz and Va to be 1/2 Vf (using 50-ohm load)
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(WAIT_TIME_MEASURE);
			Do_Measure();

			xCalVz[bBand].wX0 = g_xBridgeMeasure.Vz;
			xCalVz[bBand].wY0 = (DWORD)((DWORD)g_xBridgeMeasure.Vf*CORRECTION_FACTOR)/(DWORD)(2*g_xBridgeMeasure.Vz);
			xCalVa[bBand].wX0 = g_xBridgeMeasure.Va;
			xCalVa[bBand].wY0 = (DWORD)((DWORD)g_xBridgeMeasure.Vf*CORRECTION_FACTOR)/(DWORD)(2*g_xBridgeMeasure.Va);
        }
										//
										// 150ohm load
		if (!WaitLoadUser(g150OhmLoadStr))
			break;
										// At each band frequency, ...
        								// Determine correction factor for Vz to be 3/4 Vf and Va to be 1/4 Vf (using 150-ohm load)
										// Determine correction factor for Vr --> SWR:300
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(WAIT_TIME_MEASURE);
			Do_Measure();

			xCalVz[bBand].wX1 = g_xBridgeMeasure.Vz;
			xCalVz[bBand].wY1 = (DWORD)((DWORD)g_xBridgeMeasure.Vf*CORRECTION_FACTOR*3)/(DWORD)(4*g_xBridgeMeasure.Vz);
			xCalVa[bBand].wX1 = g_xBridgeMeasure.Va;
			xCalVa[bBand].wY1 = (DWORD)((DWORD)g_xBridgeMeasure.Vf*CORRECTION_FACTOR)/(DWORD)(4*g_xBridgeMeasure.Va);
			
			xCalVr[bBand].wX0 = g_xBridgeMeasure.Vr;
			xCalVr[bBand].wY0 = ((DWORD)(((SWR_150_LOAD-100)*g_xBridgeMeasure.Vf)/(SWR_150_LOAD+100))*CORRECTION_FACTOR)/(DWORD)g_xBridgeMeasure.Vr;
        }

										// 274ohm load
		if (!WaitLoadUser(g274OhmLoadStr))
			break;
										// At each band frequency, ...
        								// Determine correction factor for Vr to get a SWR of 548 (using 274-ohm load)
		for (bBand=0; bBand<BAND_MAX;bBand++)
		{
			BYTE ii;

			Adjust_Dds_Gain(bBand);
			DDS_Set(g_xBandLimits[bBand].middle * BAND_FREQ_MULT);
			Delay_Ms(WAIT_TIME_MEASURE);
			Do_Measure();

			xCalVr[bBand].wX1 = g_xBridgeMeasure.Vr;
			xCalVr[bBand].wY1 = ((DWORD)(((SWR_274_LOAD-100)*g_xBridgeMeasure.Vf)/(SWR_274_LOAD+100))*CORRECTION_FACTOR)/(DWORD)g_xBridgeMeasure.Vr;

										// Convert calibration factor to slope and offset		
			Calc_Correct(&g_xBandCorrFactor[bBand].xVz, &xCalVz[bBand]);
			Calc_Correct(&g_xBandCorrFactor[bBand].xVa, &xCalVa[bBand]);
			Calc_Correct(&g_xBandCorrFactor[bBand].xVr, &xCalVr[bBand]);
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
/*
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
*/
										// Print correction factors
	for (bBand=0; bBand<BAND_MAX;bBand++)
	{
		UART_CPutString("Band:");
		itoa(szMsg, bBand, 10);
		UART_PutString(szMsg);

		UART_CPutString("\t G=");
		itoa(szMsg, g_bGainDDS[bBand], 10);
		UART_PutString(szMsg);

		UART_CPutString("\t VR.S=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].xVr.iSlope, 10);
		UART_PutString(szMsg);

		UART_CPutString("\t VR.O=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].xVr.iOffset, 10);
		UART_PutString(szMsg);

		UART_CPutString("\t VZ.S=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].xVz.iSlope, 10);
		UART_PutString(szMsg);

		UART_CPutString("\t VZ.O=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].xVz.iOffset, 10);
		UART_PutString(szMsg);

		UART_CPutString("\t VA.S=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].xVa.iSlope, 10);
		UART_PutString(szMsg);

		UART_CPutString("\t VA.O=");
		ltoa(szMsg, g_xBandCorrFactor[bBand].xVa.iOffset, 10);
		UART_PutString(szMsg);

		UART_PutCRLF();
	}
	UART_Stop();
}
#endif
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Do_Correct()
//
//  DESCRIPTION:
//
//	Do the complete adjustment of the measurement based on two-point calibration data
//
//  ARGUMENTS:
//  	none
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void Do_Correct (void)
{
	g_xBridgeMeasure.Vf *= CORRECTION_FACTOR;
	g_xBridgeMeasure.Vz *= Correct_Measure(&g_xBridgeCorrect.xVz, g_xBridgeMeasure.Vz);
	g_xBridgeMeasure.Va *= Correct_Measure(&g_xBridgeCorrect.xVa, g_xBridgeMeasure.Va);
	g_xBridgeMeasure.Vr *= Correct_Measure(&g_xBridgeCorrect.xVr, g_xBridgeMeasure.Vr);
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

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Correct_Measure()
//
//  DESCRIPTION:
//
//	Corrects measurement: y=mx+b
//
//  ARGUMENTS:
//  	pxCorr	Correction data (slope and offset)
//		wX		Data to correct
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static WORD Correct_Measure(CORRECT_DATA *pxCorr, WORD wX)
{
	return (((LONG)pxCorr->iSlope*(LONG)wX)/SLOPE_CORRECT)+pxCorr->iOffset;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calc_Correct()
//
//  DESCRIPTION:
//
//	Converts two-point calibration vector to slope and offset
//
//  ARGUMENTS:
//  	pxCorr	Correction data (slope and offset)
//		pxVect	Calibration vector
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Calc_Correct (CORRECT_DATA *pxCorr, CALIB_VECTOR *pxVect)
{
	LONG lTmp;
	lTmp = (LONG)pxVect->wY1-(LONG)pxVect->wY0;
	lTmp *= (LONG)SLOPE_CORRECT;
	pxCorr->iSlope = lTmp/((LONG)pxVect->wX1-(LONG)pxVect->wX0);
	pxCorr->iOffset = (LONG)pxVect->wY1-((LONG)((LONG)pxCorr->iSlope*pxVect->wX1)/(LONG)SLOPE_CORRECT);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	WaitLoadUser()
//
//  DESCRIPTION:
//
//	Wait for load and user action
//
//  ARGUMENTS:
//  	pszText	Text in display
//
//  RETURNS:
//     FALSE user abort
//
//-----------------------------------------------------------------------------
static BOOL WaitLoadUser (const char *pszText)
{
	LCD_Position(0, 0);
	LCD_PrCString(gConfigCalibStr);
	LCD_Position(1, 0);
	LCD_PrCString(pszText);
	BUZZ_Beep();
	if ( KEYPAD_WaitKey(TIME_WAIT_KEY_S) == KBD_UP )
		return FALSE;
	DISP_Clear();
	LCD_Position(0, 0);
	LCD_PrCString(gConfigCalibStr);
	LCD_Position(1, 0);
	LCD_PrCString(gInProgressStr);
	
	return TRUE;
}