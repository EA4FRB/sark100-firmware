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
// 	FILE NAME: 	MSG_ENG.C
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//	Display texts: English
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

#include "glb_data.h"

//-----------------------------------------------------------------------------
//  Display texts:
//-----------------------------------------------------------------------------
BYTE const gBlankStr	[] = "                ";

BYTE const gWelcome1Str	[] = PRODUCT_NAME_STR " ANALYZER";
BYTE const gWelcome2Str	[] = "(c)EA4FRB 10 " VERSION_STR;

BYTE const gConfigPcLinkStr	[] = "PC Link";
BYTE const gConfigStepStr	[] = "Step Size";
BYTE const gConfigIddleStr	[] = "Suspend Timeout";
BYTE const gConfigCalibStr	[] = "Calibrate";
BYTE const gConfigSwLoadStr	[] = "Software Load";
BYTE const *gConfigStr[] = {
							gConfigPcLinkStr,
							gConfigStepStr,
							gConfigIddleStr,
							gConfigCalibStr,
							gConfigSwLoadStr
							};

BYTE const gModeSwrStr	[] = "SWR";
BYTE const gModeImpStr	[] = "IMP";
BYTE const gModeCapStr	[] = "CAP";
BYTE const gModeIndStr	[] = "IND";
BYTE const gModeOffStr	[] = "OFF";
BYTE const gModeVfoStr	[] = "VFO";
BYTE const *gModeStr[] = {
							gModeSwrStr,
							gModeImpStr,
							gModeCapStr,
							gModeIndStr,
							gModeVfoStr,
							gModeOffStr
							};

BYTE const gStep10HzStr   [] = "10Hz";
BYTE const gStep100HzStr  [] = "100Hz";
BYTE const gStep1KhzStr   [] = "1Khz";
BYTE const gStep10KhzStr  [] = "10Khz";
BYTE const gStep100KhzStr [] = "100Khz";
BYTE const *gStepStr[] = {
							gStep10HzStr,
							gStep100HzStr,
							gStep1KhzStr,
							gStep10KhzStr,
							gStep100KhzStr
							};

BYTE const gIddleOffStr    [] = "Off";
BYTE const gIddle30SStr    [] = "30 S";
BYTE const gIddle60SStr    [] = "60 S";
BYTE const gIddle90SStr    [] = "90 S";
BYTE const *gIddleStr[] = {
							gIddleOffStr,
							gIddle30SStr,
							gIddle60SStr,
							gIddle90SStr
							};

BYTE const gBand160MStr   [] = "160M";
BYTE const gBand80MStr    [] = "80M";
BYTE const gBand40MStr    [] = "40M";
BYTE const gBand30MStr    [] = "30M";
BYTE const gBand25MStr    [] = "25M";
BYTE const gBand20MStr    [] = "20M";
BYTE const gBand17MStr    [] = "17M";
BYTE const gBand15MStr    [] = "15M";
BYTE const gBand12MStr    [] = "12M";
BYTE const gBand11MStr    [] = "11M";
BYTE const gBand10MStr    [] = "10M";
BYTE const gBand8MStr     [] = "8M";
BYTE const gBand6MStr	  [] = "6M";
BYTE const *gBandStr[] = {
							gBand160MStr,
							gBand80MStr,
							gBand40MStr,
							gBand30MStr,
							gBand25MStr,
							gBand20MStr,
							gBand17MStr,
							gBand15MStr,
							gBand12MStr,
							gBand11MStr,
							gBand10MStr,
							gBand8MStr,
							gBand6MStr
							};

BYTE const gModeScanStr			[] = "SCAN";

BYTE const gErrorAdjustVfStr	[] = "Error Vf Level ";

BYTE const gInProgressStr		[] = "In progress...";
BYTE const gDoneStr				[] = "Done          ";

BYTE const gOpenLoadStr			[] = "Disconnect Load ";
BYTE const g50OhmLoadStr		[] = "Conn 50 \xf4 load  ";
BYTE const g150OhmLoadStr		[] = "Conn 150 \xf4 load ";

BYTE const gErrorUncalibratedStr[] = "Uncalibrated !! ";
BYTE const gPressAnyKeyStr		[] = "Press any key";

BYTE const gNotImplementedStr	[] = "Not Implemented";

BYTE const gConfirmStr			[] = "Confirm ?";

BYTE const gErrNoMatchingStr	[] = "Err No Matching";

BYTE const gBandWidthStr		[] = "BW:";

BYTE const gBandLitStr			[] = "BAND:";

BYTE const gSwLoadingStr		[] = "SW Loading";

