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
// 	FILE NAME: 	BUZZER.C
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
#include "PSoCAPI.h"

#include "util.h"
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
//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	
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
void BUZZ_Beep ( void )
{
	/* set period: clock 32Khz --> 4Khz  */
	PWM8_BUZZ_WritePeriod(8);
	/* set pulse width to generate a 50% duty cycle */
	PWM8_BUZZ_WritePulseWidth(4);
	/* ensure interrupt is disabled */
	PWM8_BUZZ_DisableInt();
	PWM8_BUZZ_Start();
	Delay_Ms(200);
	PWM8_BUZZ_Stop();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	
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
void BUZZ_BeepError ( void )
{
	/* set period: clock 32Khz  --> 2Khz  */
	PWM8_BUZZ_WritePeriod(16);
	/* set pulse width to generate a 50% duty cycle */
	PWM8_BUZZ_WritePulseWidth(8);
	/* ensure interrupt is disabled */
	PWM8_BUZZ_DisableInt();
	PWM8_BUZZ_Start();
	Delay_Ms(200);
	PWM8_BUZZ_Stop();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	
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
void BUZZ_KeyClick ( void )
{
	/* set period: clock 32Khz  --> 4Khz  */
	PWM8_BUZZ_WritePeriod(8);
	/* set pulse width to generate a 50% duty cycle */
	PWM8_BUZZ_WritePulseWidth(4);
	/* ensure interrupt is disabled */
	PWM8_BUZZ_DisableInt();
	PWM8_BUZZ_Start();
	Delay_Ms(5);
	PWM8_BUZZ_Stop();
}

