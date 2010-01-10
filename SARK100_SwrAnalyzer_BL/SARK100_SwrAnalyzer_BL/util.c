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
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC 2009	Creation
//
//*****************************************************************************/

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "psocgpioint.h"

#include "util.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Private data:
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Delay_Ms
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



