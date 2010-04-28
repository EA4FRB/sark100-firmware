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
//	PROJECT:	PSoC AntennaAnalyzer
// 	FILE NAME: 	KEYPAD.C
// 	AUTHOR:		Melchor Varela
//
// 	DESCRIPTION
//
//	Keypad driver
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	6OCT2008	Creation
//
//*****************************************************************************/

#include <m8c.h>        				// Part specific constants and macros
#include "PSoCAPI.h"    				// PSoC API definitions for all User Modules
#include "psocgpioint.h"

#include "keypad.h"
#include "glb_data.h"
#include "buzzer.h"
#include "util.h"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define KEY_DEBOUNCE_TIME		2		// Units of 1/8 sec
#define SPEED_KEY_DET_TIME_S	5		// Units of seconds

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	KEYPAD_Get
//
//  DESCRIPTION:
//
//	Get key pressed value
//
//  ARGUMENTS:
//    none.
//
//  RETURNS:
//    Key pressed. Zero if no key.
//
//-----------------------------------------------------------------------------
BYTE KEYPAD_Get ( void )
{
	BYTE bKey;
	static BYTE bLastKey = 0;

	if (g_bDebounceCounter!=0)
		return 0;

	bKey = KEYPAD_Scan();
	if (bKey != KEYPAD_Scan())
		bKey = 0;

	if (bKey)
	{
		BUZZ_KeyClick();
		g_bDebounceCounter = KEY_DEBOUNCE_TIME;

		if (bKey==bLastKey)
		{
			if (g_bSpeedKeyCounter == 0)
			{
				if (bKey == KBD_UP)
				{
										// Debouncing not needed for faster scan
					g_bDebounceCounter = 0;
					bKey = KBD_2xUP;
				}
				else if (bKey == KBD_DWN)
				{
										// Debouncing not needed for faster scan
					g_bDebounceCounter = 0;
					bKey = KBD_2xDWN;
				}
			}
		}
		else
		{
			g_bSpeedKeyCounter = SPEED_KEY_DET_TIME_S;
			bLastKey = bKey;
		}
	}
	else
		bLastKey = 0;
	return bKey;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	KEYPAD_WaitKey
//
//  DESCRIPTION:
//
//	Waits for key or delay.
//	Implements power saving features
//
//  ARGUMENTS:
//    bDelayS	Wait timeout in seconds
//
//  RETURNS:
//    Key pressed.
//
//-----------------------------------------------------------------------------
BYTE KEYPAD_WaitKey ( BYTE bDelayS )
{
	BYTE bKey;
										// Actions to minimize power consumption
	OSC_CR0 &= ~0x07; 					// Clear Bits 0 to 2
	OSC_CR0 |= 0x05; 					// Sets CPU clock to 750Khz
	g_bIddleCounter = bDelayS;
	do
	{
		M8C_Sleep;
		bKey = KEYPAD_Get();
		if (bDelayS)
		{
			if (g_bIddleCounter==0)
				break;
		}
	} while (bKey == 0);
	OSC_CR0 &= ~0x07; 					// Clear Bits 0 to 2
	OSC_CR0 |= 0x03;  					// Set CPU Clock to SysClk/1

	return bKey;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	KEYPAD_Scan
//
//  DESCRIPTION:
//
//	Scans keypad
//
//  ARGUMENTS:
//    none.
//
//  RETURNS:
//    Key pressed. Zero if no key.
//
//-----------------------------------------------------------------------------
BYTE KEYPAD_Scan ( void )
{
	BYTE bKey = 0;

	COL0_Data_ADDR |= COL0_MASK;
	COL1_Data_ADDR |= COL1_MASK;
	COL2_Data_ADDR |= COL2_MASK;

	COL0_Data_ADDR &= ~COL0_MASK;
	if ( (ROW0_Data_ADDR & ROW0_MASK) == 0 )
	{
		bKey = KBD_CONFIG;
	}
	if ( (ROW1_Data_ADDR & ROW1_MASK) == 0 )
	{
		bKey = KBD_BAND;
	}
	COL0_Data_ADDR |= COL0_MASK;

	COL1_Data_ADDR &= ~COL1_MASK;
	if ( (ROW0_Data_ADDR & ROW0_MASK) == 0 )
	{
		bKey = KBD_SCAN;
	}
	if ( (ROW1_Data_ADDR & ROW1_MASK) == 0 )
	{
		bKey = KBD_UP;
	}
	COL1_Data_ADDR |= COL1_MASK;

	COL2_Data_ADDR &= ~COL2_MASK;
	if ( (ROW0_Data_ADDR & ROW0_MASK) == 0 )
	{
		bKey = KBD_MODE;
	}
	if ( (ROW1_Data_ADDR & ROW1_MASK) == 0 )
	{
		if (bKey == KBD_UP)
			bKey =  KBD_UP_DWN;
		else
			bKey = KBD_DWN;
	}
	COL2_Data_ADDR |= COL2_MASK;

	return bKey;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	KEYPAD_SysSuspend
//
//  DESCRIPTION:
//
//	System suspend. Wakes up from a key press
//
//  ARGUMENTS:
//    none.
//
//  RETURNS:
//    Key pressed. Zero if no key.
//
//-----------------------------------------------------------------------------
void KEYPAD_SysSuspend ( void )
{
	BYTE bSave_ARF_CR;
	BYTE bSave_ABF_CR0;

	M8C_DisableGInt;
										// Activates all columns
	COL0_Data_ADDR &= ~COL0_MASK;
	COL1_Data_ADDR &= ~COL1_MASK;
	COL2_Data_ADDR &= ~COL2_MASK;
										// Reduces analog power
	bSave_ARF_CR = ARF_CR;
	bSave_ABF_CR0 = ABF_CR0;
	ARF_CR &= 0xf8; 					// analog blocks Off
	ABF_CR0 &= 0xc3;	 				// analog buffer off

										// Set low level active interrupt
	PRT1IC0	|= 0x3;
	PRT1IC1	&= ~0x3;

	PRT1IE	|= 0x3;						// Enables row0&row1 interrupt

										// Disables sleep interrupt
	M8C_DisableIntMask(INT_MSK0, INT_MSK0_SLEEP);
										// Enables GPIO interrupt
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_GPIO);

										// Clear Pending GPIO Interrupt
	INT_CLR0 &= 0x20;

	M8C_EnableGInt;

	M8C_Sleep;							// Goes sleep

	INT_VC = 0;							// Erases vector

	PRT1IE	&= ~(0x3);					// Disables row0&row1 interrupt

										// Disables GPIO interrupt
	M8C_DisableIntMask(INT_MSK0, INT_MSK0_GPIO);
										// Restores sleep interrupt
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_SLEEP);

	ARF_CR = bSave_ARF_CR;
	ABF_CR0 = bSave_ABF_CR0;
}