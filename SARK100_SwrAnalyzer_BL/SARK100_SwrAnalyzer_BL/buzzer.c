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
// 	FILE NAME: 	BUZZER.C
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//	Buzzer management
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
#include "PSoCAPI.h"

#include "util.h"

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	BUZZ_Beep
//
//  DESCRIPTION:
//
//	Sounds buzzer, standard beep
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void BUZZ_Beep ( void )
{
										// Set period: clock 32Khz --> 4Khz
	PWM8_BUZZ_WritePeriod(8);
										// Set pulse width to generate a 50% duty cycle
	PWM8_BUZZ_WritePulseWidth(4);
										// Enensure interrupt is disabled
	PWM8_BUZZ_DisableInt();
	PWM8_BUZZ_Start();
	Delay_Ms(200);
	PWM8_BUZZ_Stop();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	BUZZ_BeepError
//
//  DESCRIPTION:
//
//	Sounds buzzer, error beep
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void BUZZ_BeepError ( void )
{
										// Set period: clock 32Khz  --> 2Khz
	PWM8_BUZZ_WritePeriod(16);
										// Set pulse width to generate a 50% duty cycle
	PWM8_BUZZ_WritePulseWidth(8);
										// Ensure interrupt is disabled
	PWM8_BUZZ_DisableInt();
	PWM8_BUZZ_Start();
	Delay_Ms(200);
	PWM8_BUZZ_Stop();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	BUZZ_KeyClick
//
//  DESCRIPTION:
//
//	Simulates the key click
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void BUZZ_KeyClick ( void )
{
										// Set period: clock 32Khz  --> 4Khz
	PWM8_BUZZ_WritePeriod(8);
										// Set pulse width to generate a 50% duty cycle
	PWM8_BUZZ_WritePulseWidth(4);
										// Ensure interrupt is disabled
	PWM8_BUZZ_DisableInt();
	PWM8_BUZZ_Start();
	Delay_Ms(5);
	PWM8_BUZZ_Stop();
}

