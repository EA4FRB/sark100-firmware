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
// 	FILE NAME: 	PCLINK.C
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
#include <string.h>
#include <m8c.h>
#include "PSoCAPI.h"
#include "keypad.h"
#include "msg_generic.h"
#include "dds.h"
#include "glb_data.h"
#include "calibrate_reflectometer.h"
#include "calcs.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Typedefs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Externals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private data:
//-----------------------------------------------------------------------------
static BYTE const gszCmdOn		[] = "on";
static BYTE const gszCmdOff		[] = "off";
static BYTE const gszCmdFreq	[] = "freq";
static BYTE const gszCmdMeasImp	[] = "imp";
static BYTE const gszCmdMeasRaw	[] = "raw";

static BYTE const gszWelcome	[] = "\r\nEA4FRB SWR Analyzer V0.0 \r\n";
static BYTE const gszCmdPrompt	[] = "\r\n>>";
static BYTE const gszWaitLink	[] = "Waiting Link";
static BYTE const gszOk			[] = "\r\nOK\r\n";
static BYTE const gszSwr		[] = "\r\nSWR=";
static BYTE const gszSwrOvf		[] = ">10";
static BYTE const gszZ			[] = "\tZ=";
static BYTE const gszR			[] = "\tR=";
static BYTE const gszX			[] = "\tX=";
static BYTE const gszOvf		[] = "Ovf";
static BYTE const gszVf			[] = "\r\nVf=";
static BYTE const gszVr			[] = "\tVr=";
static BYTE const gszVz			[] = "\tVz=";
static BYTE const gszVa			[] = "\tVa=";

static BYTE const gszErrFreqNotSet	[] = "\r\nError: freq not set\r\n";
static BYTE const gszErrExpectFreq	[] = "\r\nError: expected freq val\r\n";
static BYTE const gszErrInvalidFreq	[] = "\r\nError: invalid freq\r\n";
static BYTE const gszErrCmdNotFound1[] = "\r\nCommand <";
static BYTE const gszErrCmdNotFound2[] = "> not found\r\n";

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static BYTE GetBand (DWORD dwFreq);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	PcLink
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
void PcLink (void)
{
	char * strPtr; 					// Parameter pointer
	DWORD dwCurrentFreq = -1;
	BYTE szMsg[20];
	BYTE bBand;
	
	UART_CmdReset(); 						// Initialize receiver/cmd
											// buffer
	UART_IntCntl(UART_ENABLE_RX_INT); 		// Enable RX interrupts
	UART_Start(UART_PARITY_NONE); 			// Enable UART
	
	M8C_EnableGInt ;
	
    UART_PutChar(12); 												//Clear the screen
	UART_CPutString(gszWelcome);
	UART_CPutString(gszCmdPrompt);

	LCD_Position(1, 0);
	LCD_PrCString(gszWaitLink);
	while(TRUE) 
	{
		if (KEYPAD_Get() == KBD_UP)
			break;

		if(UART_bCmdCheck()) 				// Wait for command
		{ 
			if(strPtr = UART_szGetParam()) // More than delimiter?
			{ 
				LCD_Position(1, 0);
				LCD_PrCString(gBlankStr);
				LCD_Position(1, 0);
				LCD_PrString(strPtr);
    			if(!cstrcmp((const char*)gszCmdOn,(char*)strPtr)) 
				{
					if (dwCurrentFreq==-1)
					{
						UART_CPutString(gszErrFreqNotSet);
						break;
					}
					DDS_Set(dwCurrentFreq);
					UART_CPutString(gszOk);
				}
    			else if(!cstrcmp((const char*)gszCmdOff,(char*)strPtr)) 
				{
					DDS_Set(0);
					UART_CPutString(gszOk);
				}
    			else if(!cstrcmp((const char*)gszCmdFreq,(char*)strPtr)) 
				{
					do
					{
						strPtr = UART_szGetParam();
						if (strPtr==NULL)
						{
							UART_CPutString(gszErrExpectFreq);
							break;
						}
						dwCurrentFreq = atol(strPtr);
						bBand = GetBand(dwCurrentFreq);
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
    			else if(!cstrcmp((const char*)gszCmdMeasImp,(char*)strPtr)) 
				{
					Do_Measure();
					gwSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
					gwZ = Calculate_Z(gwSwr, g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
					gwR = Calculate_R(gwZ, gwSwr);
					gwX = Calculate_X(gwZ, gwR);
				
					itoa(szMsg, gwSwr, 10);
					UART_CPutString(gszSwr);
					if (gwSwr>SWR_MAX)
					{
						UART_CPutString(gszSwrOvf);
					}
					else
					{
						UART_PutString(szMsg);
					}
					
					itoa(szMsg, gwZ, 10);
					UART_CPutString(gszZ);
					if (gwZ==-1)
					{
						UART_CPutString(gszOvf);
					}
					else
					{
						UART_PutString(szMsg);
					}
					
					itoa(szMsg, gwR, 10);
					UART_CPutString(gszR);
					if (gwR==-1)
					{
						UART_CPutString(gszOvf);
					}
					else
					{
						UART_PutString(szMsg);
					}

					itoa(szMsg, gwX, 10);
					UART_CPutString(gszX);
					if (gwX==-1)
					{
						UART_CPutString(gszOvf);
					}
					else
					{
						UART_PutString(szMsg);
					}
					UART_PutCRLF();
				}
    			else if(!cstrcmp((const char*)gszCmdMeasRaw,(char*)strPtr)) 
				{
					Do_Measure();

					ltoa(szMsg, g_xBridgeMeasure.Vf/100, 10);
					UART_CPutString(gszVf);
					UART_PutString(szMsg);
					
					ltoa(szMsg, g_xBridgeMeasure.Vr/100, 10);
					UART_CPutString(gszVr);
					UART_PutString(szMsg);
					
					ltoa(szMsg, g_xBridgeMeasure.Vz/100, 10);
					UART_CPutString(gszVz);
					UART_PutString(szMsg);
					
					ltoa(szMsg, g_xBridgeMeasure.Va/100, 10);
					UART_CPutString(gszVa);
					UART_PutString(szMsg);
					UART_PutCRLF();
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
//
//  ARGUMENTS:
//     none.
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
