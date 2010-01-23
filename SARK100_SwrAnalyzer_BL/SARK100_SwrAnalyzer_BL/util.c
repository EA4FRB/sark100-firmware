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
// 	FILE NAME: 	UTIL.C
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//	Utility functions
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC 2009	Creation
//
//*****************************************************************************/

#include <m8c.h>        		// Part specific constants and macros
#include "PSoCAPI.h"    		// PSoC API definitions for all User Modules
#include "psocgpioint.h"
#include "glb_data.h"
#include "util.h"

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Delay_Ms
//
//  DESCRIPTION:
//
//	Wait for a number of ms
//
//  ARGUMENTS:
//     ms	Milliseconds
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void Delay_Ms(WORD ms)
{
	Timer16_Delay_WritePeriod((ms * 33)+1);
	Timer16_Delay_Start();

	while (Timer16_Delay_wReadTimer()!=0)
	{;}
	Timer16_Delay_Stop();
}


//-----------------------------------------------------------------------------
//  FUNCTION NAME: BcdToHex
//
//  DESCRIPTION:
//
//	Converts bcd coded byte to hexadecimal.
//
//  ARGUMENTS:
//    bcd = bcd value.
//
//  RETURNS:
//    Hexadecimal representation of bcd value.
//
//-----------------------------------------------------------------------------
BYTE BcdToHex ( BYTE bcd )
{
	return ((BYTE)((bcd&0xf0)>>4)*10)+(BYTE)(bcd&0x0f);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: HexToBcd
//
//  DESCRIPTION:
//
//	Converts hexadecimal number to bcd.
//
//  ARGUMENTS:
//    hex = hexadecimal representation of bcd value.
//
//  RETURNS:
//    bcd value.
//
//-----------------------------------------------------------------------------
BYTE HexToBcd ( BYTE hex )
{
	return ((BYTE)(hex/10)<<4) | (BYTE)(hex%10);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GetStep
//
//  DESCRIPTION:
//
//	Gets the step value in Hz from the step index value
//
//  ARGUMENTS:
//		bStep	Integer describing frequency step value
//
//  RETURNS:
//     Step value in Hz
//
//-----------------------------------------------------------------------------
DWORD GetStep (BYTE bStep)
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

