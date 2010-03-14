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
// 	FILE NAME: 	CALIBRATE_REFLECTOMETER.H
// 	AUTHOR:		Melchor Varela
//
// 	DESCRIPTION
//
//	Calibration routines
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	6OCT2008	Creation
//
//*****************************************************************************/
#ifndef __CALIBRATE_REFLECTOMETER_H__
#define __CALIBRATE_REFLECTOMETER_H__

#include <m8c.h>        // part specific constants and macros

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
extern void Calibrate_Reflectometer (void);
extern void Do_Correct (void);
extern void Adjust_Dds_Gain (BYTE bBand);

//-----------------------------------------------------------------------------
#endif
