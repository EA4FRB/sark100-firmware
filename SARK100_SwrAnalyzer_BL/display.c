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
// 	FILE NAME: 	DISPLAY.C
// 	AUTHOR:		Melchor Varela
//
// 	DESCRIPTION
//
//	Display utility functions
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
#include "m8c.h"
#include "Lcd.h"
#include "Util.h"
#include "Display.h"
#include "Msg_generic.h"
#include "glb_data.h"

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Setup
//
//  DESCRIPTION:
//
//	Setup display controller and create icons from bitmap data.
//
//  ARGUMENTS:
//	  none.
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Setup ( void )
{
	LCD_Start();
}
//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Frequency
//
//  DESCRIPTION:
//
//	Display VFO frequency in the current cursor position
//
//  ARGUMENTS:
//    dwFreq 	Frequency in hertz
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Frequency ( DWORD dwFreq )
{
	char szFreq[9];
	char szText[11];
	int ii, jj;

	ultoa(szFreq, dwFreq, 10);
	szText[10] = 0;
	for (ii=strlen(szFreq)-1, jj=9;ii>=0;ii--,jj--)
	{
		szText[jj] = szFreq[ii];
		if ((jj==7) && (ii!=0))
			szText[--jj] = '.';
		else if ((jj==3) && (ii!=0))
			szText[--jj] = ',';
	}
	for (;jj!=-1; jj--)
	{
		szText[jj] = ' ';
	}
	LCD_PrString(szText);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Swr
//
//  DESCRIPTION:
//
//	Display SWR value
//
//  ARGUMENTS:
//		wSwr	SWR value
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Swr ( WORD wSwr )
{
	char szSwr[4];

	if (wSwr>=SWR_MAX)
	{
		LCD_PrCString(">10");
		return;
	}
	itoa(szSwr, wSwr, 10);
	LCD_Write_Data( szSwr[0] );
	LCD_Write_Data( '.' );
	LCD_Write_Data( szSwr[1] );
	LCD_Write_Data( szSwr[2] );
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_ImpedanceComplez
//
//  DESCRIPTION:
//
//	Display impedance
//
//  ARGUMENTS:
//		wR			Resistance
//		wX			Reactance
//		bSign		Sign of impedance
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_ImpedanceComplex ( WORD wR, WORD wX, BYTE bSign )
{
	char szText[16];

	itoa(szText, wR, 10);
	LCD_PrString(szText);
	LCD_Write_Data( bSign );
	LCD_Write_Data( 'j' );
	itoa(szText, wX, 10);
	LCD_PrString(szText);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Capacitance
//
//  DESCRIPTION:
//
//	Display capacitance
//
//  ARGUMENTS:
//		wC		Capacitance in 10xpF
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Capacitance ( WORD wC )
{
	char szC[16];

	LCD_PrCString("C = ");
	itoa(szC, wC/10, 10);
	LCD_PrString(szC);
	LCD_Write_Data( '.' );
	itoa(szC, wC%10, 10);
	LCD_PrString(szC);
	LCD_PrCString("pF");
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Inductance
//
//  DESCRIPTION:
//
//	Display inductance
//
//  ARGUMENTS:
//		wL		Inductance in 10xuH
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Inductance ( WORD wL )
{
	char szL[16];

	LCD_PrCString("L = ");
	itoa(szL, wL/10, 10);
	LCD_PrString(szL);
	LCD_Write_Data( '.' );
	itoa(szL, wL%10, 10);
	LCD_PrString(szL);
	LCD_PrCString("uH");
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Impedance
//
//  DESCRIPTION:
//
//	Displays impedance modulus
//
//  ARGUMENTS:
//		wZ		Impedance modulus
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Impedance ( WORD wZ )
{
	char szZ[16];


	LCD_PrCString("Z = ");
	if (wZ>2000)
	{
		LCD_PrCString(">2000");
	}
	else
	{
		itoa(szZ, wZ, 10);
		LCD_PrString(szZ);
		LCD_PrCString(" \xf4");
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Clear
//
//  DESCRIPTION:
//
//	Clears the screen
//
//  ARGUMENTS:
//
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Clear (void)
{
	LCD_Control(LCD_CLEAR);
}
