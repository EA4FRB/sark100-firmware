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
// 	FILE NAME: 	DISPLAY.C
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
#include <stdlib.h>
#include <string.h>
#include "m8c.h"
#include "Lcd.h"
#include "Util.h"
#include "Display.h"
#include "Msg_generic.h"
#include "glb_data.h"

//-----------------------------------------------------------------------------
//  Icons bitmap definition
//-----------------------------------------------------------------------------
static BYTE const UpArrowBitmap[8] = 		{ 0x00, 0x00, 0x04, 0x0e, 0x1f, 0x00, 0x00, 0x00 };
static BYTE const DownArrowBitmap[8] = 		{ 0x00, 0x00, 0x1f, 0x0e, 0x04, 0x00, 0x00, 0x00 };
static BYTE const LeftArrowBitmap[8] = 		{ 0x00, 0x02, 0x06, 0x0e, 0x06, 0x02, 0x00, 0x00 };
static BYTE const RightArrowBitmap[8] = 	{ 0x00, 0x08, 0x0c, 0x0e, 0x0c, 0x08, 0x00, 0x00 };

//-----------------------------------------------------------------------------
//  Private functions prototypes
//-----------------------------------------------------------------------------
static void Configure_CG_Bitmap ( BYTE bitmapId, BYTE const *bitmapPtr  );

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

	Configure_CG_Bitmap( ID_BITMAP_UP_ARROW,    UpArrowBitmap );
	Configure_CG_Bitmap( ID_BITMAP_DOWN_ARROW,  DownArrowBitmap );
	Configure_CG_Bitmap( ID_BITMAP_LEFT_ARROW,  LeftArrowBitmap );
	Configure_CG_Bitmap( ID_BITMAP_RIGHT_ARROW, RightArrowBitmap );
	DISP_UpdateLevelBargraph( ID_BITMAP_REC_LEVEL,     0 );
	DISP_UpdateLevelBargraph( ID_BITMAP_SQUELCH_LEVEL, 0 );
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: UpdateLevelBargraph
//
//  DESCRIPTION:
//
//	Update bargraph bitmap based on provided level
//
//  ARGUMENTS:
//	  bitmapId : bitmap identifier
//    level    : {0..7}
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_UpdateLevelBargraph ( BYTE bitmapId, BYTE level )
{
	BYTE ii;

	level &= 0x7;

	for ( ii = 0; ii < 8; ii++ )
	{
		LCD_Control( 0x40 + bitmapId*8 + 7 - ii);
		if ( ii >= level+1 )
			LCD_Write_Data( 0x00 );
		else
			LCD_Write_Data( 0x1f );
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: Configure_CG_Bitmap
//
//  DESCRIPTION:
//
//	Configurate display character generator RAM to display icons
//
//  ARGUMENTS:
//
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
static void Configure_CG_Bitmap ( BYTE bitmapId, BYTE const *bitmapPtr  )
{
	BYTE ii;

	for ( ii = 0; ii < 8; ii++ )
	{
		LCD_Control( 0x40 + bitmapId*8 + ii);
		LCD_Write_Data( *(bitmapPtr+ii) );
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Frequency
//
//  DESCRIPTION:
//
//	Display VFO frequency in the current cursor position
//
//  ARGUMENTS:
//    freq : frequency in hertz
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
//
//
//  ARGUMENTS:
//
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_Swr ( WORD wSwr )
{
	char szSwr[4];

	if (wSwr>SWR_MAX)
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
//  FUNCTION NAME: DISP_Impedance
//
//  DESCRIPTION:
//
//
//
//  ARGUMENTS:
//
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
void DISP_ImpedanceComplex ( WORD wR, WORD wX, BYTE bIsPositive )
{
	char szText[16];
	
	if (wR==-1 || wX==-1)
	{
		LCD_PrCString(gErrorOverflowStr);
		return;
	}
	if (wR>10000)
	{
		itoa(szText, wR/1000, 10);
		LCD_PrString(szText);	
		LCD_Write_Data( 'K' );
	}	
	else
	{
		itoa(szText, wR, 10);
		LCD_PrString(szText);	
	}		
	if (bIsPositive)
		LCD_PrCString("+j");
	else
		LCD_PrCString("-j");
	if (wX>10000)
	{
		itoa(szText, wX/1000, 10);
		LCD_PrString(szText);	
		LCD_Write_Data( 'K' );
	}	
	else
	{
		itoa(szText, wX, 10);
		LCD_PrString(szText);	
	}		
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Capacitance
//
//  DESCRIPTION:
//
//
//
//  ARGUMENTS:
//
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
//@TODO: Improve presentation: decimals, etc
void DISP_Capacitance ( WORD wC )
{
	char szC[16];

	LCD_PrCString("C = ");
	if (wC==-1)
	{
		LCD_PrCString(gErrorOverflowStr);
	}
	else
	{
		itoa(szC, wC, 10);
		LCD_PrString(szC);
		LCD_PrCString("pF");
	}		
}


//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Inductance
//
//  DESCRIPTION:
//
//
//
//  ARGUMENTS:
//
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
//@TODO: Improve presentation: decimals, etc
void DISP_Inductance ( WORD wL )
{
	char szL[16];

	LCD_PrCString("L = ");
	if (wL==-1)
	{
		LCD_PrCString(gErrorOverflowStr);
	}
	else
	{
		itoa(szL, wL, 10);
		LCD_PrString(szL);
		LCD_PrCString("uH");
	}		
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME: DISP_Impedance
//
//  DESCRIPTION:
//
//
//
//  ARGUMENTS:
//
//
//  RETURNS:
//    none.
//
//-----------------------------------------------------------------------------
//@TODO: Improve presentation: decimals, etc
void DISP_Impedance ( WORD wZ )
{
	char szZ[16];

	LCD_PrCString("Z = ");
	if (wZ==-1)
	{
		LCD_PrCString(gErrorOverflowStr);
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
//
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
