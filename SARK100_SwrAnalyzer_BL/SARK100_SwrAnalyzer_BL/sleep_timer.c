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
// 	FILE NAME: 	SLEEP_TIMER.C
// 	AUTHOR:		Melchor Varela
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
#include "m8c.h"
#include "glb_data.h"

//-----------------------------------------------------------------------------
//  Public data
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private static data
//-----------------------------------------------------------------------------
static volatile BYTE gTickCount = 1;			// Prescaler to generate one second

//-----------------------------------------------------------------------------
//  FUNCTION NAME: SleepTimerINT
//
//  DESCRIPTION:
//
//	Sleep timer interrupt sevice routine. Period 8Hz.
//
//  ARGUMENTS:
//    none.
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
#pragma interrupt_handler 	SleepTimerINT

void SleepTimerINT ( void )
{
	M8C_ClearWDTAndSleep;
	
	if (g_bMeasureCounter)
		g_bMeasureCounter--;
	if (g_bDebounceCounter)
		g_bDebounceCounter--;
	
	if( --gTickCount )
    	return;
	gTickCount = 8;
	
	//One second
	g_bIddleCounter--;
}


