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
// 	FILE NAME: 	CALCS.C
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

#include <m8c.h>        // part specific constants and macros
#include <math.h>

#include "PSoCAPI.h"
#include "psocgpioint.h"

#include "calibrate_reflectometer.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
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
static WORD Calc_Sqrt (DWORD dwN);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calculate_Swr
//
//  DESCRIPTION:
//
//	Calculates SWR based on VF and VR values
//	Notice that wVf value is x2 because it is measured at the bridge input, and
//	wVr is also x2 because the PGA gain is set to double, so both compensates and
//	it is not needed further calculation
//
//	SWR = (Vf+Vr/Vf-Vr)*100
//
//  ARGUMENTS:
//     	dwVf
//		dwVr
//
//  RETURNS:
//     SWR
//
//-----------------------------------------------------------------------------
WORD Calculate_Swr (DWORD dwVf, DWORD dwVr)
{
	DWORD dwDenominator;
	DWORD dwNumerator;

	if (dwVf>dwVr)
		dwDenominator = dwVf-dwVr;
	else if (dwVr>dwVf)
		dwDenominator = dwVr-dwVf;
	else
		return (WORD) -1;				// Error

	dwNumerator = (dwVf+dwVr)*100;

	return (dwNumerator/dwDenominator);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calculate_Z
//
//  DESCRIPTION:
//
//	Calculates impedance
//
//	Z = (50 * Vz)/Va;
//
//  ARGUMENTS:
//  	wSwr
//		dwVz
//		dwVa
//
//  RETURNS:
//     Z
//
//-----------------------------------------------------------------------------
WORD Calculate_Z (WORD wSwr, DWORD dwVz, DWORD dwVa)
{
	if (wSwr>999)
		return -1;

	if (dwVa == 0)						// Avoids divide by zero
		return (WORD)-1;

	return ((DWORD)(dwVz * (DWORD)50))/dwVa;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calculate_R
//
//  DESCRIPTION:
//
//	Calculates resistance
//
//               (2500 + Z^2) * SWR
//           R = ------------------
//                50 * (SWR^2 + 1)
//
//  ARGUMENTS:
//     wZ
//		wSwr
//
//  RETURNS:
//     R
//
//-----------------------------------------------------------------------------
WORD Calculate_R (WORD wZ, WORD wSwr)
{
	DWORD dwNumerator;
	DWORD dwDenominator;

	if (wZ == 0)
		return 0;
	if (wZ == -1 || wSwr == -1)
		return -1;
	if (wSwr>999)
	{
		return -1;
	}
	dwDenominator = (((DWORD)wSwr*wSwr)/2)+5000;
	if (dwDenominator == 0)				// Avoids divide by zero
		return (WORD)-1;
	dwNumerator = (((DWORD)wZ*wZ) + 2500) * (DWORD)wSwr;

	return dwNumerator/dwDenominator;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calculate_X
//
//  DESCRIPTION:
//
//	Calculates impedance
//
//           X = SQRT ( Z_squared - R_squared )
//
//  ARGUMENTS:
//     	wZ
//		wR
//
//  RETURNS:
//     X
//
//-----------------------------------------------------------------------------
WORD Calculate_X (WORD wZ, WORD wR)
{
	DWORD dwTemp;

	if (wZ == -1 || wR == -1)
		return -1;
	dwTemp = ((DWORD)wZ*wZ)-((DWORD)wR*wR);
	if ((signed long)dwTemp<0)
		return 0;

	return Calc_Sqrt(dwTemp);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calculate_L
//
//  DESCRIPTION:
//
//	Calculates inductance
//
//   	L=10^6*X/2*PI*freq
//
//  ARGUMENTS:
//     	wX
//		dwFreq
//
//  RETURNS:
//     Inductance value in *10 uH.
//
//-----------------------------------------------------------------------------
WORD Calculate_L (WORD wX, DWORD dwFreq)
{
	DWORD dwTemp;

	if (wX == -1)
		return -1;

	dwFreq /= 1000;						// Hz to Khz

	dwTemp = ((DWORD)wX*100000)/628;
	return dwTemp / dwFreq;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calculate_C
//
//  DESCRIPTION:
//
//	Calculates capacitance
//
//           C=10^12/(2*PI*freq*X)
//
//  ARGUMENTS:
//     	wX
//		dwFreq
//
//  RETURNS:
//     Capacitance value in x10 pf .
//
//-----------------------------------------------------------------------------
WORD Calculate_C (WORD wX, DWORD dwFreq)
{
	if (wX==0)
		return 0;
	if (wX == -1)
		return -1;

	dwFreq /= 1000;		//Hz to Khz

	return ((DWORD)10000/63) * 	((DWORD)10000/dwFreq) * ((DWORD)10000/(wX*10));
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Calc_Sqrt
//
//  DESCRIPTION:
//
//	Calculates square root fo integer number
//
//  ARGUMENTS:
//     dwN		Value to calculate
//
//  RETURNS:
//     Square root
//
//-----------------------------------------------------------------------------
static WORD Calc_Sqrt (DWORD dwN)
{
	DWORD dwRem = 0;
	DWORD dwRoot = 0;
	BYTE ii;

	for (ii=0;ii<16;ii++)
	{
		dwRoot <<= 1;
		dwRem = ((dwRem<<2)+(dwN>>30));
		dwN <<= 2;
		dwRoot++;
		if (dwRoot<=dwRem)
		{
			dwRem -= dwRoot;
			dwRoot++;
		}
		else
		{
			dwRoot--;
		}
	}
	return (WORD)(dwRoot>>1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0	// Test data sets
void Do_Test_Calcs ( void )
{
	//100 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 1365*100;
	g_xBridgeMeasure.Va = 1365*100;
	g_xBridgeMeasure.Vz = 2731*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//50 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 0*100;
	g_xBridgeMeasure.Va = 2048*100;
	g_xBridgeMeasure.Vz = 2048*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//150 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 2048*100;
	g_xBridgeMeasure.Va = 1024*100;
	g_xBridgeMeasure.Vz = 3072*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//0 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 4096*100;
	g_xBridgeMeasure.Va = 4096*100;
	g_xBridgeMeasure.Vz = 0*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);


	//500 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 3351*100;
	g_xBridgeMeasure.Va = 372*100;
	g_xBridgeMeasure.Vz = 3724*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//1000 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 3706*100;
	g_xBridgeMeasure.Va = 195*100;
	g_xBridgeMeasure.Vz = 3901*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//2000 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 3896*100;
	g_xBridgeMeasure.Va = 100*100;
	g_xBridgeMeasure.Vz = 3996*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//3000 Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 3962*100;
	g_xBridgeMeasure.Va = 67*100;
	g_xBridgeMeasure.Vz = 4029*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//10K Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 4055*100;
	g_xBridgeMeasure.Va = 20*100;
	g_xBridgeMeasure.Vz = 4076*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);

	//open Ohm
	g_xBridgeMeasure.Vf = 4096*100;
	g_xBridgeMeasure.Vr = 4096*100;
	g_xBridgeMeasure.Va = 0*100;
	g_xBridgeMeasure.Vz = 4096*100;
	wSwr = Calculate_Swr(g_xBridgeMeasure.Vf, g_xBridgeMeasure.Vr);
	wZ = Calculate_Z(g_xBridgeMeasure.Vz, g_xBridgeMeasure.Va);
	wR = Calculate_R(wZ, wSwr);
	wX = Calculate_X(wZ, wR);
	wC = Calculate_C(wX, dwCurrentFreq);
	wL = Calculate_L(wX, dwCurrentFreq);
}
#endif
