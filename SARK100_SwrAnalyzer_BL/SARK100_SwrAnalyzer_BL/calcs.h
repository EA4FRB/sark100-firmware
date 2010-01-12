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
// 	FILE NAME: 	CALCS.H
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//	SWR and impedance calculation routines
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC 2009	Creation
//
//*****************************************************************************/
#ifndef __CALCS_H__
#define __CALCS_H__

#include <m8c.h>        // part specific constants and macros
#include <math.h>

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
WORD Calculate_Swr (DWORD dwVf, DWORD dwVr);
WORD Calculate_Z (WORD wSwr, DWORD dwVz, DWORD dwVa);
WORD Calculate_R (WORD wZ, WORD wSwr);
WORD Calculate_X (WORD wZ, WORD wR);
WORD Calculate_L (WORD wX, DWORD dwFreq);
WORD Calculate_C (WORD wX, DWORD dwFreq);

//-----------------------------------------------------------------------------
#endif