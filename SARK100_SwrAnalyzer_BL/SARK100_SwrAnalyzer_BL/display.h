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
// 	FILE NAME: 	DISPLAY.H
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
void DISP_UpdateLevelBargraph ( BYTE bitmapId, BYTE level );
void DISP_Frequency ( DWORD dwFreq );
void DISP_Swr ( WORD wSwr );
void DISP_ImpedanceComplex ( WORD wR, WORD wX, BYTE bIsPositive );
void DISP_Impedance ( WORD wZ );
void DISP_Capacitance ( WORD wC );
void DISP_Inductance ( WORD wL );
void DISP_Clear (void);

//-----------------------------------------------------------------------------
//  Icons definitions
//-----------------------------------------------------------------------------
#define ID_BITMAP_UP_ARROW		1
#define ID_BITMAP_DOWN_ARROW	2
#define ID_BITMAP_LEFT_ARROW	3
#define ID_BITMAP_RIGHT_ARROW	4
#define ID_BITMAP_REC_LEVEL		5
#define ID_BITMAP_SQUELCH_LEVEL	6

#define STR_BACK_ARROW			"\x7f"
#define STR_UP_ARROW			"\x01"
#define STR_DOWN_ARROW			"\x02"
#define STR_LEFT_ARROW			"\x03"
#define STR_RIGHT_ARROW			"\x04"
#define STR_REC_LEVEL			"\x05"
#define STR_SQUELCH_LEVEL		"\x06"

//*****************************************************************************
#endif











