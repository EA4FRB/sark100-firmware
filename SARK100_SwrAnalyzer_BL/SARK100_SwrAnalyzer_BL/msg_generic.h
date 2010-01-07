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
// 	FILE NAME: 	MSG_GENERIC.H
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
#ifndef __MSG_GENERIC_H__
#define __MSG_GENERIC_H__

#include <stdlib.h>
#include <m8c.h>

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Display texts:
//-----------------------------------------------------------------------------
extern BYTE const gBlankStr[];

extern BYTE const gWelcome1Str[];
extern BYTE const gWelcome2Str[];

extern BYTE const *gModeStr[];

extern BYTE const *gStepStr[];

extern BYTE const *gBandStr[];

extern BYTE const *gConfigStr[];

extern BYTE const gModeScanStr[];

extern BYTE const gErrorAdjustVfStr[];

extern BYTE const gConfigCalibStr[];

extern BYTE const gInProgressStr[];
extern BYTE const gDoneStr[];

extern BYTE const gOpenLoadStr[];
extern BYTE const g50OhmLoadStr[];
extern BYTE const g150OhmLoadStr[];

extern BYTE const gErrorUncalibratedStr[];
extern BYTE const gPressAnyKeyStr[];

extern BYTE const gErrorOverflowStr[];

extern BYTE const gNotImplementedStr[];

extern BYTE const gConfirmStr[];

extern BYTE const gErrNoMatchingStr[];

extern BYTE const gBandWidthStr[];

extern BYTE const gBandLitStr[];

extern BYTE const gSwLoadingStr[];

//-----------------------------------------------------------------------------
#endif
