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
// 	FILE NAME: 	DDS.C
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

#include "dds.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
//#define REF_FREQUENCY	(4294967296/180000000)	//30Mhz ref clock times 6
#define REF_FREQUENCY_NUM	(42949673)			//30Mhz ref clock times 6
#define REF_FREQUENCY_DEN	(1800000)			//30Mhz ref clock times 6

//-----------------------------------------------------------------------------
//  Public data:
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Private data:
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static DWORD divluh1(DWORD x, DWORD y, DWORD z) ;
static void muldwu1(DWORD w[], DWORD u, DWORD v);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	DDS_Init
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
void DDS_Init(void)
{
	DDS_CLK_Data_ADDR &= ~DDS_CLK_MASK;
	DDS_DATA_Data_ADDR &= ~DDS_DATA_MASK;
	DDS_LOAD_Data_ADDR &= ~DDS_LOAD_MASK;

	DDS_CLK_Data_ADDR |= DDS_CLK_MASK;		//Strobe Clock to get hardware bits D0, D1 and D2 into the input reg
	DDS_CLK_Data_ADDR &= ~DDS_CLK_MASK;
	DDS_LOAD_Data_ADDR |= DDS_LOAD_MASK;	//and raise Load to get them into the bControl reg
	DDS_LOAD_Data_ADDR &= ~DDS_LOAD_MASK;

	DDS_Set(0);								//Write 32 zeros to AD9851 with 6x PLL enabled
}

void DDS_Set(DWORD dwFreq)
{
	BYTE bBitCnt;
	BYTE bControl;
	DWORD dwOp[2];

	DDS_LOAD_Data_ADDR &= ~DDS_LOAD_MASK;		//Drop DDS_Load signal to start the programming sequence

	// Transfer frequency bits
	if (dwFreq!=0)
	{
		muldwu1(dwOp, dwFreq, REF_FREQUENCY_NUM);
		dwFreq = divluh1(dwOp[0], dwOp[1], REF_FREQUENCY_DEN);
	}	

	for (bBitCnt=0; bBitCnt<(sizeof(dwFreq)*8); bBitCnt++, dwFreq>>=1)
	{
		if ((dwFreq&1)!=0)
			DDS_DATA_Data_ADDR |= DDS_DATA_MASK;
		else
			DDS_DATA_Data_ADDR &= ~DDS_DATA_MASK;
		DDS_CLK_Data_ADDR |= DDS_CLK_MASK;		//Toggle write clock
		DDS_CLK_Data_ADDR &= ~DDS_CLK_MASK;
	}

	// Transfer bControl word
//	if (dwFreq!=0)
		bControl=0x1;									//6x PLL enabled, no phase shift
//	else	
//		bControl=0x5;									//6x PLL enabled, no phase shift, power-down
	for (bBitCnt=0; bBitCnt<(sizeof(bControl)*8); bBitCnt++, bControl>>=1)
	{
		if ((bControl&1)!=0)
			DDS_DATA_Data_ADDR |= DDS_DATA_MASK;
		else
			DDS_DATA_Data_ADDR &= ~DDS_DATA_MASK;
		DDS_CLK_Data_ADDR |= DDS_CLK_MASK;		//Toggle write clock
		DDS_CLK_Data_ADDR &= ~DDS_CLK_MASK;
	}
	DDS_LOAD_Data_ADDR |= DDS_LOAD_MASK;		//Raise Load signal to load the DDS registers
	DDS_LOAD_Data_ADDR &= ~DDS_LOAD_MASK;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	64bit math routines
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
static DWORD divluh1(DWORD x, DWORD y, DWORD z) 
{
   // Divides (x || y) by z.
   int i;
   DWORD c;

   c = 0;
   for (i = 1; i <= 32; i++) {
      if (c == 0) {
         c = x >> 31;
         x = (x << 1) | (y >> 31); // Shift x || y left
         y = y << 1;               // one bit.
         c = c ^ (x < z);
         x = x - z;
      }
      else {
         c = x >> 31;
         x = (x << 1) | (y >> 31); // Shift x || y left
         y = y << 1;               // one bit.
         x = x + z;
         c = c ^ (x < z);
      }
      y = y + (1 - c);
   }
   return y;                    // Remainder is x.
}

static void muldwu1(DWORD w[], DWORD u, DWORD v) 
{
   DWORD u0, u1, v0, v1, k, t;
   DWORD w1, w2, w3;

   u0 = u >> 16; u1 = u & 0xFFFF;
   v0 = v >> 16; v1 = v & 0xFFFF;

   t = u1*v1;
   w3 = t & 0xFFFF;             // (*)
   k = t >> 16;

   t = u0*v1 + k;
   w2 = t & 0xFFFF;
   w1 = t >> 16;

   t = u1*v0 + w2;
   k = t >> 16;

   w[0] = u0*v0 + w1 + k;
   w[1] = (t << 16) + w3;       // (*)
/* w[1] = u*v;                  // Alternative. */

   return;
}

