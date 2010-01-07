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
// 	FILE NAME: 	MAIN.C
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
#include "psocgpioint.h"
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "Lcd.h"
#include "Util.h"
#include "Display.h"
#include "glb_data.h"
#include "msg_generic.h"
#include "dds.h"
#include "storage.h"
#include "keypad.h"
#include "buzzer.h"
#include "calcs.h"
#include "calibrate_reflectometer.h"
#include "pclink.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define DIDDLING_FREQ		1000000		//1Mhz
#define TIME_DELAY_TEXT		4			//Units of 1/8

#define COL_MODE			0
#define ROW_MODE			0

#define COL_SWR				0
#define ROW_SWR				1

#define COL_L				0
#define ROW_L				1

#define COL_C				0
#define ROW_C				1

#define COL_IMP				5
#define ROW_IMP				1

#define COL_FREQ			6
#define ROW_FREQ			0

//-----------------------------------------------------------------------------
//  Typedefs
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Externals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private data
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static DWORD Mode_Scan (BYTE bBand, BYTE bStep);
static DWORD GetStep (BYTE bStep);
static void Mode_Config (void);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	main
//
//  DESCRIPTION:
//
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void main()
{
	DWORD dwCurrentFreq;
	DWORD dwScanFreq;
	BYTE bMode = MODE_SWR;
	BYTE bBand = BAND_30M;
	BYTE bKey;
	BYTE ii;

	//M8C_EnableWatchDog;				// Enable watchdog
	M8C_ClearWDTAndSleep;			// Put sleep and watchdog timers into a known state
									// before enabling interrupts.
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_SLEEP);
	M8C_EnableGInt;					// Enable global interrupt

	DISP_Setup();

	// Display welcome screen
	LCD_Position(0, 0);
	LCD_PrCString(gWelcome1Str);
	LCD_Position(1, 0);
	LCD_PrCString(gWelcome2Str);
	KEYPAD_WaitKey(TIME_DELAY_TEXT);

	// Get stored correction factors
	STR_Restore();

	if (g_bIsCalibrated == FALSE)
	{
		BUZZ_BeepError();

		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gErrorUncalibratedStr);
		LCD_Position(1, 0);
		LCD_PrCString(gPressAnyKeyStr);
		// Wait key press
		KEYPAD_WaitKey(TIME_WAIT_KEY_S);
		DISP_Clear();
	}

	// Enables DDS oscillator and backlight (shared port)
	Port_2_Data_SHADE |= XO_EN_MASK;
	XO_EN_Data_ADDR |= XO_EN_MASK;
	Delay_Ms(10);
	DDS_Init();

	//Setup default frequency
	dwCurrentFreq = g_xBandLimits[bBand].middle * BAND_FREQ_MULT;

	//Get band-specific correction factors based on current freq dial setting
	g_xBridgeCorrect = g_xBandCorrFactor[bBand];

	//Set save band frequency to the center of each band
	for (ii=0; ii<BAND_MAX; ii++)
	{
		g_dwSaveFreqBand[ii] = g_xBandLimits[ii].middle * BAND_FREQ_MULT;
	}
	//Resets iddle counter
	g_bIddleCounter = TIME_TO_IDDLE_S;

	// Sets DDS gain
	PGA_DDS_1_Start(PGA_DDS_1_HIGHPOWER);
	PGA_DDS_2_Start(PGA_DDS_2_HIGHPOWER);
	Adjust_Dds_Gain(bBand);

	//Check Vf level
	DDS_Set(dwCurrentFreq);
	Delay_Ms(10);
	Do_Measure();
	DDS_Set(0);
	if (g_xBridgeMeasure.Vf<500)
	{
		BUZZ_BeepError();

		LCD_Position(0, 0);
		LCD_PrCString(gErrorAdjustVfStr);
		LCD_Position(1, 0);
		LCD_PrCString(gPressAnyKeyStr);
		// Wait key press
		KEYPAD_WaitKey(TIME_WAIT_KEY_S);
		DISP_Clear();
	}
	
	do
	{
		// Display mode and frequency
		DISP_Clear();
		LCD_Position(ROW_MODE, COL_MODE);
		LCD_PrCString(gModeStr[bMode]);
		LCD_Position(ROW_FREQ, COL_FREQ);
		DISP_Frequency(dwCurrentFreq);
		do
		{
			if (bMode != MODE_OFF)
			{
				DDS_Set(dwCurrentFreq);
				Delay_Ms(10);
				Do_Measure();
				DDS_Set(0);

				gwSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
				gwZ = Calculate_Z(gwSwr, g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
				gwR = Calculate_R(gwZ, gwSwr);
				gwX = Calculate_X(gwZ, gwR);

				switch (bMode)
				{
					case MODE_SWR:
						LCD_Position(ROW_SWR, 0);
						LCD_PrCString(gBlankStr);
						LCD_Position(ROW_SWR, COL_SWR);
						DISP_Swr(gwSwr);

						LCD_Position(ROW_IMP, COL_IMP);
						DISP_Impedance(gwZ);
						break;

					case MODE_IMP:
						// Diddling
						{
							WORD wSwr;
							WORD wZ;
							WORD wX;
							WORD wR;
							BYTE bSign;

							DDS_Set(dwCurrentFreq+DIDDLING_FREQ);
							Delay_Ms(10);
							Do_Measure();
							DDS_Set(0);
							wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
							wZ = Calculate_Z(wSwr, g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
							wR = Calculate_R(wZ, wSwr);
							wX = Calculate_X(wZ, wR);
         					//Increasing X with increasing F ==> inductive reactance = +j
         					//Decreasing X with increasing F ==> capacitive reactance = -j
							if (wX >= gwX)
								bSign = TRUE;
							else
								bSign = FALSE;

							LCD_Position(ROW_SWR, 0);
							LCD_PrCString(gBlankStr);
							LCD_Position(ROW_SWR, COL_SWR);
							DISP_Swr(gwSwr);
							LCD_Position(ROW_IMP, COL_IMP);
							DISP_ImpedanceComplex(gwR, gwX, bSign);
						}
						break;

					case MODE_CAP:
						LCD_Position(ROW_SWR, 0);
						LCD_PrCString(gBlankStr);
						LCD_Position(ROW_C, COL_C);
						gwC = Calculate_C(gwX, dwCurrentFreq);
						DISP_Capacitance(gwC);
						break;

					case MODE_IND:
						LCD_Position(ROW_SWR, 0);
						LCD_PrCString(gBlankStr);
						LCD_Position(ROW_L, COL_L);
						gwL = Calculate_L(gwX, dwCurrentFreq);
						DISP_Inductance(gwL);
						break;

					case MODE_VFO:
					{
						char szText[17];
						LCD_Position(ROW_SWR, 0);
						LCD_PrCString(gBlankStr);
						LCD_Position(0, 0);
						LCD_PrCString("Vf");
						ltoa(szText,g_xBridgeMeasure.Vf/100,10);
						LCD_PrString(szText);
						LCD_PrCString(" Vr");
						ltoa(szText,g_xBridgeMeasure.Vr/100,10);
						LCD_PrString(szText);

						LCD_Position(1, 0);
						LCD_PrCString("Vz");
						ltoa(szText,g_xBridgeMeasure.Vz/100,10);
						LCD_PrString(szText);
						LCD_PrCString(" Va");
						ltoa(szText,g_xBridgeMeasure.Va/100,10);
						LCD_PrString(szText);
					}
						break;

					default:
						break;
				}
			}

			g_bMeasureCounter = MEASURE_PERIOD;
			do
			{
				M8C_Sleep;
				bKey = KEYPAD_Get();
				switch ( bKey )
				{
					case KBD_CONFIG:
						Mode_Config();
						//Get band-specific correction factors based on current freq dial setting
						g_xBridgeCorrect = g_xBandCorrFactor[bBand];
						Adjust_Dds_Gain(bBand);
						break;

					case KBD_MODE:
						if (bMode==MODE_OFF)
						{
							PGA_DDS_1_Start(PGA_DDS_1_HIGHPOWER);
							PGA_DDS_2_Start(PGA_DDS_2_HIGHPOWER);
							Adjust_Dds_Gain(bBand);

							// Enables DDS oscillator and backlight (shared port)
							//XO_EN_Data_ADDR |= XO_EN_MASK;
						}
						if (++bMode >= MODE_MAX)
							bMode = MODE_SWR;
						if (bMode==MODE_OFF)
						{
							PGA_DDS_1_Stop();
							PGA_DDS_2_Stop();

							// Disables DDS oscillator and backlight (shared port)
							//XO_EN_Data_ADDR &= ~XO_EN_MASK;
						}
						break;

					case KBD_BAND:
						g_dwSaveFreqBand[bBand] = dwCurrentFreq;			//Saves current freq
						if (++bBand >= BAND_MAX)
							bBand = BAND_160M;
						dwCurrentFreq = g_dwSaveFreqBand[bBand];			//Restores frequency

						//Get band-specific correction factors based on current freq dial setting
						g_xBridgeCorrect = g_xBandCorrFactor[bBand];
						Adjust_Dds_Gain(bBand);

						DISP_Clear();
						LCD_Position(0, 0);
						LCD_PrCString(gBandLitStr);
						LCD_Position(0, 12);
						LCD_PrCString(gBandStr[bBand]);
						KEYPAD_WaitKey(TIME_DELAY_TEXT);
						break;

					case KBD_SCAN:
						dwScanFreq = Mode_Scan(bBand, g_xConf.bStep);
						if (dwScanFreq!=-1)
						{
							dwCurrentFreq = dwScanFreq;
						}
						break;

					case KBD_UP:
						dwCurrentFreq += GetStep(g_xConf.bStep);
						if (dwCurrentFreq >= (g_xBandLimits[bBand].high * BAND_FREQ_MULT))
						{
							if (++bBand >= BAND_MAX)
								bBand = 0;
							g_xBridgeCorrect = g_xBandCorrFactor[bBand];
							Adjust_Dds_Gain(bBand);
							dwCurrentFreq = g_xBandLimits[bBand].low * BAND_FREQ_MULT;

							DISP_Clear();
							LCD_Position(0, 0);
							LCD_PrCString(gBandLitStr);
							LCD_Position(0, 12);
							LCD_PrCString(gBandStr[bBand]);
							KEYPAD_WaitKey(TIME_DELAY_TEXT);
						}
						break;

					case KBD_DWN:
						dwCurrentFreq -= GetStep(g_xConf.bStep);
						if (dwCurrentFreq <= (g_xBandLimits[bBand].low * BAND_FREQ_MULT))
						{
							if (bBand-- == 0)
								bBand = BAND_MAX-1;

							g_xBridgeCorrect = g_xBandCorrFactor[bBand];
							Adjust_Dds_Gain(bBand);
							dwCurrentFreq = g_xBandLimits[bBand].high * BAND_FREQ_MULT;

							DISP_Clear();
							LCD_Position(0, 0);
							LCD_PrCString(gBandLitStr);
							LCD_Position(0, 12);
							LCD_PrCString(gBandStr[bBand]);
							KEYPAD_WaitKey(TIME_DELAY_TEXT);
						}
						break;

					default:
						break;
				}
			} while ((bKey==0) && (g_bMeasureCounter!=0));

			if (bKey!=0)
				g_bIddleCounter = TIME_TO_IDDLE_S;

			// Iddle mode (not for VFO mode)
			if ((g_bIddleCounter==0) && (bMode != MODE_VFO))
			{
				// Suspend
				// Disables DDS oscillator and backlight (shared port)
				DDS_Set(0);		//DDS power down
				XO_EN_Data_ADDR &= ~XO_EN_MASK;
				Port_2_Data_SHADE &= ~XO_EN_MASK;

				PGA_DDS_1_Stop();
				PGA_DDS_2_Stop();

				LCD_Init();
				LCD_Control(LCD_OFF);						//Display off

				//Waits for a key press
				KEYPAD_WaitKey(0);

				// Resumes
				LCD_Control(LCD_ON);						//Display on

				// Enables DDS oscillator and backlight (shared port)
				XO_EN_Data_ADDR |= XO_EN_MASK;
				Port_2_Data_SHADE |= XO_EN_MASK;

				PGA_DDS_1_Start(PGA_DDS_1_HIGHPOWER);
				PGA_DDS_2_Start(PGA_DDS_2_HIGHPOWER);
				Adjust_Dds_Gain(bBand);

				DDS_Init();

				g_bIddleCounter = TIME_TO_IDDLE_S;
				bKey = 1;		//Forces exit loop
			}
		} while (bKey==0);
	} while (TRUE);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Mode_Scan
//
//  DESCRIPTION:
//
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static DWORD Mode_Scan (BYTE bBand, BYTE bStep)
{
	DWORD dwCurrentFreq;
	DWORD dwLimitFreq;
	DWORD dwResonanceFreq = -1;
	DWORD dwBwMinFreq = -1;
	DWORD dwBwMaxFreq = -1;
	WORD wSwrMin = 200;
	BYTE bKey;

	DISP_Clear();
	LCD_Position(ROW_MODE, COL_MODE);
	LCD_PrCString(gModeScanStr);

	dwCurrentFreq = g_xBandLimits[bBand].low * BAND_FREQ_MULT;
	dwLimitFreq = g_xBandLimits[bBand].high * BAND_FREQ_MULT;
	do
	{
		DDS_Set(dwCurrentFreq);
		LCD_Position(ROW_FREQ, COL_FREQ);
		DISP_Frequency(dwCurrentFreq);

		Do_Measure();
		gwSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
		gwZ = Calculate_Z(gwSwr, g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);

		LCD_Position(ROW_SWR, 0);
		LCD_PrCString(gBlankStr);

		LCD_Position(ROW_SWR, COL_SWR);
		DISP_Swr(gwSwr);
		LCD_Position(ROW_IMP, COL_IMP);
		DISP_Impedance(gwZ);
		if (gwSwr <= wSwrMin)
		{
			if (dwBwMinFreq==-1)
			{
				BUZZ_Beep();
				Delay_Ms(50);
				BUZZ_Beep();
				dwBwMinFreq = dwCurrentFreq;
			}
			wSwrMin = gwSwr;
			dwResonanceFreq = dwCurrentFreq;
		}
		else
		{
			if ((dwBwMinFreq!=-1)&&(dwBwMaxFreq==-1))
			{
				BUZZ_Beep();
				Delay_Ms(50);
				BUZZ_Beep();
				dwBwMaxFreq = dwCurrentFreq;
			}
		}
		dwCurrentFreq += GetStep(bStep);
	} while (dwCurrentFreq < dwLimitFreq);

	BUZZ_Beep();
	DISP_Clear();
	LCD_Position(0, 0);
	if (dwResonanceFreq==-1)
	{
		LCD_PrCString(gErrNoMatchingStr);
	}
	else
	{
		if (dwBwMaxFreq==-1)
			dwBwMaxFreq = dwCurrentFreq;

		LCD_Position(ROW_MODE, COL_MODE);
		LCD_PrCString(gBandWidthStr);
		LCD_Position(ROW_FREQ, COL_FREQ);
		DISP_Frequency(dwBwMaxFreq-dwBwMinFreq);
	}
	LCD_Position(1, 0);
	LCD_PrCString(gPressAnyKeyStr);
	KEYPAD_WaitKey(TIME_WAIT_KEY_S);

	return dwResonanceFreq;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GetStep
//
//  DESCRIPTION:
//
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static DWORD GetStep (BYTE bStep)
{
	switch (bStep)
	{
		case STEP_10HZ:
			return 10;
		default:
		case STEP_100HZ:
			return 100;
		case STEP_1KHZ:
			return 1000;
		case STEP_10KHZ:
			return 10000;
		case STEP_100KHZ:
			return 100000;
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Mode_Config
//
//  DESCRIPTION:
//
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Mode_Config (void)
{
	BYTE bMenu = 0;
	BYTE bKey;
	BYTE bStep;
	
	do
	{
		DISP_Clear();
		LCD_Position(0, 0);
		LCD_PrCString(gConfigStr[bMenu]);
		bKey = KEYPAD_WaitKey(TIME_WAIT_KEY_S);
		if (bKey==0)
			break;
		if (bKey==KBD_CONFIG)
		{
			if (++bMenu>=CONFIG_MAX)
				bMenu = 0;
		}
		else if (bKey==KBD_UP)
		{
			break;
		}
		else if (bKey==KBD_DWN)
		{
			switch (bMenu)
			{
				case CONFIG_PCLINK:
					PcLink();
					break;
					
				case CONFIG_STEP:
					bStep = g_xConf.bStep;
					do
					{
						DISP_Clear();
						LCD_Position(0, 0);
						LCD_PrCString(gConfigStr[bMenu]);
						LCD_Position(1, 0);
						LCD_PrCString(gStepStr[bStep]);
						bKey = KEYPAD_WaitKey(TIME_WAIT_KEY_S);
						if (bKey==KBD_CONFIG)
						{
							if (++bStep>=STEP_MAX)
								bStep = 0;
						}
						else if (bKey==KBD_UP)
						{
							break;
						}
						else if (bKey==KBD_DWN)
						{
							g_xConf.bStep = bStep;
							STR_SaveConfig();
						}						
					} while(TRUE);
					break;
					
				case CONFIG_CALIB:
					LCD_Position(1, 0);
					LCD_PrCString(gConfirmStr);
					// Wait key press
					if (KEYPAD_WaitKey(TIME_WAIT_KEY_S) == KBD_DWN)
						Calibrate_Reflectometer();
					break;
					
				case CONFIG_SW_LOAD:
					LCD_Position(1, 0);
					LCD_PrCString(gConfirmStr);
					// Wait key press
					if (KEYPAD_WaitKey(TIME_WAIT_KEY_S) == KBD_DWN)
					{
						DISP_Clear();
						LCD_Position(0, 0);
						LCD_PrCString(gSwLoadingStr);
						M8C_Reset;		// Perform Software Reset by Supervisory Call
					}	
					break;
			}
		}
	} while (TRUE);
}


//-----------------------------------------------------------------------------
//  Trash
//-----------------------------------------------------------------------------
#if 0
DAC8_1_Start(DAC8_1_HIGHPOWER);
PGA_DDS_1_Start(PGA_DDS_1_HIGHPOWER);
DAC8_1_WriteStall(g_bGainDDS[bBand]);
while (1)
{
		char szText[17];
							Do_Measure();
		DISP_Clear();
						LCD_Position(ROW_SWR, 0);
						LCD_PrCString(gBlankStr);
						LCD_Position(0, 0);
						LCD_PrCString("Vf");
						ltoa(szText,g_xBridgeMeasure.Vf/100,10);
						LCD_PrString(szText);
						LCD_PrCString(" Vr");
						ltoa(szText,g_xBridgeMeasure.Vr/100,10);
						LCD_PrString(szText);

						LCD_Position(1, 0);
						LCD_PrCString("Vz");
						ltoa(szText,g_xBridgeMeasure.Vz/100,10);
						LCD_PrString(szText);
						LCD_PrCString(" Va");
						ltoa(szText,g_xBridgeMeasure.Va/100,10);
						LCD_PrString(szText);
}
#endif