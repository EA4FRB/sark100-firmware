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
// 	FILE NAME: 	PCLINK.C
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//	PC link routines for update measurements
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC 2009	Creation
//
//*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <m8c.h>
#include "PSoCAPI.h"
#include "keypad.h"
#include "msg_generic.h"
#include "dds.h"
#include "glb_data.h"
#include "calibrate_reflectometer.h"
#include "calcs.h"
#include "util.h"

//-----------------------------------------------------------------------------
//  Private data:
//-----------------------------------------------------------------------------
static BYTE const gszCmdOn		[] = "on";
static BYTE const gszCmdOff		[] = "off";
static BYTE const gszCmdFreq	[] = "freq";
static BYTE const gszCmdMeasImp	[] = "imp";
static BYTE const gszCmdMeasRaw	[] = "raw";
static BYTE const gszCmdScan	[] = "scan";
static BYTE const gszCmdScanRaw	[] = "scanr";

static BYTE const gszWelcome	[] = "\r\n" PRODUCT_NAME_STR " SWR Analyzer " VERSION_STR "\r\n";
static BYTE const gszCmdPrompt	[] = "\r\n>>";
static BYTE const gszWaitLink	[] = "Waiting Link";
static BYTE const gszOk			[] = "\r\nOK\r\n";
static BYTE const gszSwrOvf		[] = "9.99";
static BYTE const gszOvf		[] = "9999";
static BYTE const gszStart		[] = "\r\nStart\r\n";
static BYTE const gszEnd		[] = "End\r\n";

static BYTE const gszErrFreqNotSet	[] = "\r\nError: freq not set\r\n";
static BYTE const gszErrExpectFreq	[] = "\r\nError: expected freq val\r\n";
static BYTE const gszErrExpectStep	[] = "\r\nError: expected step val\r\n";
static BYTE const gszErrInvalidFreq	[] = "\r\nError: invalid freq\r\n";
static BYTE const gszErrCmdNotFound1[] = "\r\nCommand <";
static BYTE const gszErrCmdNotFound2[] = "> not found\r\n";

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static BYTE GetBand (DWORD dwFreq);
static void Cmd_Off (void);
static void Cmd_On (void);
static void Cmd_Freq (void);
static void Cmd_Imp (void);
static void Cmd_Raw (void);
static void Cmd_Scan (BYTE bRaw);

//-----------------------------------------------------------------------------
//  Private data
//-----------------------------------------------------------------------------
static DWORD gdwCurrentFreq = -1;

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	PcLink
//
//  DESCRIPTION:
//
//	PC link routines for update measurements
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void PcLink (void)
{
	char * strPtr; 						// Parameter pointer

	UART_CmdReset(); 					// Initialize receiver/cmd buffer
	UART_IntCntl(UART_ENABLE_RX_INT); 	// Enable RX interrupts
	UART_Start(UART_PARITY_NONE); 		// Enable UART

	M8C_EnableGInt ;

    UART_PutChar(12); 					// Clear the screen
	UART_CPutString(gszWelcome);
	UART_CPutString(gszCmdPrompt);

	LCD_Position(1, 0);
	LCD_PrCString(gszWaitLink);
	while(TRUE)
	{
		if (KEYPAD_Get() == KBD_UP)
			break;

		if(UART_bCmdCheck()) 			// Wait for command
		{								// More than delimiter?
			if(strPtr = UART_szGetParam())
			{
				LCD_Position(1, 0);
				LCD_PrCString(gBlankStr);
				LCD_Position(1, 0);
				LCD_PrString(strPtr);
    			if(!cstrcmp((const char*)gszCmdOn,(char*)strPtr))
				{
					Cmd_On();
				}
    			else if(!cstrcmp((const char*)gszCmdOff,(char*)strPtr))
				{
					Cmd_Off();
				}
    			else if(!cstrcmp((const char*)gszCmdScan,(char*)strPtr))
    			{
					Cmd_Scan(FALSE);
				}
    			else if(!cstrcmp((const char*)gszCmdScanRaw,(char*)strPtr))
    			{
					Cmd_Scan(TRUE);
				}
    			else if(!cstrcmp((const char*)gszCmdFreq,(char*)strPtr))
				{
					Cmd_Freq();
				}
    			else if(!cstrcmp((const char*)gszCmdMeasImp,(char*)strPtr))
				{
					UART_PutCRLF();
					Cmd_Imp();
				}
    			else if(!cstrcmp((const char*)gszCmdMeasRaw,(char*)strPtr))
				{
					UART_PutCRLF();
					Cmd_Raw();
				}
				else
				{
					UART_CPutString(gszErrCmdNotFound1);
					UART_PutString(strPtr);
					UART_CPutString(gszErrCmdNotFound2);
				}
			}
			UART_CmdReset(); // Reset command buffer
			UART_CPutString(gszCmdPrompt);
		}
	}
	DDS_Set(0);
	UART_Stop();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GetBand
//
//  DESCRIPTION:
//
//	Get band index for a given frequency
//
//  ARGUMENTS:
//     dwFreq		Frequency in Hz
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static BYTE GetBand (DWORD dwFreq)
{
	BYTE bBand;

	for (bBand=0; bBand<BAND_MAX; bBand++)
	{
		if (dwFreq >= (g_xBandLimits[bBand].low*BAND_FREQ_MULT) &&
			dwFreq < (g_xBandLimits[bBand].high*BAND_FREQ_MULT))
			break;
	}
	if (bBand>=BAND_MAX)
		return -1;

	return bBand;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Cmd_On
//
//  DESCRIPTION:
//
//	Process on (enable DDS) command
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Cmd_On (void)
{
	if (gdwCurrentFreq==-1)
	{
		UART_CPutString(gszErrFreqNotSet);
	}
	else
	{
		DDS_Set(gdwCurrentFreq);
		UART_CPutString(gszOk);
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Cmd_Off
//
//  DESCRIPTION:
//
//	Process on command
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Cmd_Off (void)
{
	DDS_Set(0);
	UART_CPutString(gszOk);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Cmd_Freq
//
//  DESCRIPTION:
//
//	Process freq (set frequency) command
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Cmd_Freq (void)
{
	char * strPtr; 						// Parameter pointer
	BYTE bBand;

	do
	{
		strPtr = UART_szGetParam();
		if (strPtr==NULL)
		{
			UART_CPutString(gszErrExpectFreq);
			break;
		}
		gdwCurrentFreq = atol(strPtr);
		bBand = GetBand(gdwCurrentFreq);
		if (bBand==-1)
		{
			UART_CPutString(gszErrInvalidFreq);
			break;
		}
		g_xBridgeCorrect = g_xBandCorrFactor[bBand];
		Adjust_Dds_Gain(bBand);
		UART_CPutString(gszOk);
	} while (FALSE);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Cmd_Imp
//
//  DESCRIPTION:
//
//	Process imp (impedance) command
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Cmd_Imp (void)
{
	BYTE szMsg[20];

	Do_Measure();
	gwSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	gwZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	gwR = Calculate_R(gwZ, gwSwr);
	gwX = Calculate_X(gwZ, gwR);

	itoa(szMsg, gwSwr, 10);
	UART_PutChar( szMsg[0] );
	UART_PutChar( '.' );
	UART_PutChar( szMsg[1] );
	UART_PutChar( szMsg[2] );

	UART_PutChar(',');
	itoa(szMsg, gwR, 10);
	UART_PutString(szMsg);

	UART_PutChar(',');
	itoa(szMsg, gwX, 10);
	UART_PutString(szMsg);

	UART_PutChar(',');
	itoa(szMsg, gwZ, 10);
	UART_PutString(szMsg);

	UART_PutCRLF();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Cmd_Raw
//
//  DESCRIPTION:
//
//	Process raw (raw measurement data) command
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Cmd_Raw (void)
{
	BYTE szMsg[20];

	Do_Measure();

	ltoa(szMsg, g_xBridgeMeasure.Vf, 10);
	UART_PutString(szMsg);

	UART_PutChar(',');
	ltoa(szMsg, g_xBridgeMeasure.Vr, 10);
	UART_PutString(szMsg);

	UART_PutChar(',');
	ltoa(szMsg, g_xBridgeMeasure.Vz, 10);
	UART_PutString(szMsg);

	UART_PutChar(',');
	ltoa(szMsg, g_xBridgeMeasure.Va, 10);
	UART_PutString(szMsg);

	UART_PutCRLF();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Cmd_Scan
//
//  DESCRIPTION:
//
//	Process scan command
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void Cmd_Scan (BYTE bRaw)
{
	DWORD dwFreq;
	DWORD dwEndFreq;
	DWORD dwLimitFreq;
	DWORD dwStepFreq;
	BYTE bBand;
	BYTE bBandEnd;
	char * strPtr; 						// Parameter pointer

	do
	{
										// Get start frequency
		strPtr = UART_szGetParam();
		if (strPtr==NULL)
		{
			UART_CPutString(gszErrExpectFreq);
			break;
		}
		dwFreq = atol(strPtr);
		bBand = GetBand(dwFreq);
		if (bBand==-1)
		{
			UART_CPutString(gszErrInvalidFreq);
			break;
		}
										// Get end frequency
		strPtr = UART_szGetParam();
		if (strPtr==NULL)
		{
			UART_CPutString(gszErrExpectFreq);
			break;
		}
		dwEndFreq = atol(strPtr);
		bBandEnd = GetBand(dwEndFreq);
		if (bBandEnd==-1)
		{
			UART_CPutString(gszErrInvalidFreq);
			break;
		}

										// Get step value
		strPtr = UART_szGetParam();
		if (strPtr==NULL)
		{
			UART_CPutString(gszErrExpectStep);
			break;
		}
		dwStepFreq = atoi(strPtr);

		UART_CPutString(gszStart);
		for (; dwFreq < dwEndFreq; bBand++)
		{
			g_xBridgeCorrect = g_xBandCorrFactor[bBand];
			Adjust_Dds_Gain(bBand);
			dwLimitFreq = g_xBandLimits[bBand].high * BAND_FREQ_MULT;

			do
			{
				DDS_Set(dwFreq);
				if (bRaw)
					Cmd_Raw();
				else
					Cmd_Imp();
				dwFreq += dwStepFreq;
			} while ((dwFreq < dwLimitFreq) && (dwFreq < dwEndFreq));
		}
		UART_CPutString(gszEnd);
		if (gdwCurrentFreq==-1)
		{
			DDS_Set(0);
		}
	} while (FALSE);
}

