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
// 	FILE NAME: 	DISPLAY.H
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
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Lcd.h"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define LCD_CLEAR			1
#define LCD_OFF				0x80
#define LCD_ON				0xc0

//-----------------------------------------------------------------------------
//  Public functions
//-----------------------------------------------------------------------------
void DISP_Setup ( void );
void DISP_Frequency ( DWORD dwFreq );
void DISP_Swr ( WORD wSwr );
void DISP_ImpedanceComplex ( WORD wR, WORD wX, BYTE bIsPositive );
void DISP_Impedance ( WORD wZ );
void DISP_Capacitance ( WORD wC );
void DISP_Inductance ( WORD wL );
void DISP_Clear (void);

//*****************************************************************************
#endif











