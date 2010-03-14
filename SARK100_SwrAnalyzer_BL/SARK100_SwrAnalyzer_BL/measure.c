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
// 	FILE NAME: 	MEASURE.C
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//
//	Measure reflectometer voltages
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC2009		Creation
//
//*****************************************************************************/

#include <m8c.h>        				// Part specific constants and macros
#include <math.h>

#include "PSoCAPI.h"
#include "psocgpioint.h"
#include "util.h"

#include "measure.h"
#include "glb_data.h"

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static void WaitPgaSettling (void);
static DWORD TakeSample (void);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Do_Measure
//
//  DESCRIPTION:
//
//	Measure reflectometer voltages.
//
//  ARGUMENTS:
//     	g_xBridgeOffset
//		g_xBridgeCorrect
//
//  RETURNS:
//     g_xBridgeMeasure
//
//-----------------------------------------------------------------------------
void Do_Measure ( void )
{
//	ADCINC12_Start(ADCINC12_HIGHPOWER); // Turn on Analog section
//	PGA_ADC_Start(PGA_ADC_HIGHPOWER);

	PGA_ADC_SetGain(PGA_ADC_G2_67);
	WaitPgaSettling();

										// Read Vf
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_1);

	g_xBridgeMeasure.Vf = TakeSample();
	if (g_xBridgeOffset.Vf > g_xBridgeMeasure.Vf)
		g_xBridgeMeasure.Vf = 0;
	else
		g_xBridgeMeasure.Vf -= g_xBridgeOffset.Vf;

										// Read Va
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_7);
	g_xBridgeMeasure.Va = TakeSample();
	if (g_xBridgeOffset.Va > g_xBridgeMeasure.Va)
		g_xBridgeMeasure.Va = 0;
	else
		g_xBridgeMeasure.Va -= g_xBridgeOffset.Va;

										// Read Vz
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_5);
	g_xBridgeMeasure.Vz = TakeSample();
	if (g_xBridgeOffset.Vz > g_xBridgeMeasure.Vz)
		g_xBridgeMeasure.Vz = 0;
	else
		g_xBridgeMeasure.Vz -= g_xBridgeOffset.Vz;

										// Read Vr
										// Gain is set to double because dynamic range is half
	PGA_ADC_SetGain(PGA_ADC_G5_33);
	WaitPgaSettling();
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_3);
	g_xBridgeMeasure.Vr = TakeSample();
	if (g_xBridgeOffset.Vr > g_xBridgeMeasure.Vr)
		g_xBridgeMeasure.Vr = 0;
	else
		g_xBridgeMeasure.Vr -= g_xBridgeOffset.Vr;

//	ADCINC12_Stop();
//	PGA_ADC_Stop();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	WaitPgaSettling
//
//  DESCRIPTION:
//
//	Waits settling time needed for PGA
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void WaitPgaSettling (void)
{
	Delay_Ms(1);
/*
	volatile int ii;
	for (ii=0;ii<20;ii++)
	{;}
*/
}
//-----------------------------------------------------------------------------
//  FUNCTION NAME:	TakeSample
//
//  DESCRIPTION:
//
//	Takes sample from ADC using correlated double sampling
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     Measured value
//
//-----------------------------------------------------------------------------
static DWORD TakeSample (void)
{
	WORD wVal = 0;
	WORD wZero = 0;
										// Correlated double sampling
	PGA_ADC_GAIN_CR1 &= ~1;				// Set input to RefLO
	ADCINC12_GetSamples(1);
										// Wait for data to be ready.
	while(ADCINC12_fIsDataAvailable() == 0);
	wZero = (ADCINC12_iGetData()+2048);
	ADCINC12_ClearFlag();

	PGA_ADC_GAIN_CR1 |= 1;				// Set input to PortInp
	ADCINC12_GetSamples(1);
										// Wait for data to be ready.
	while(ADCINC12_fIsDataAvailable() == 0);
	wVal = (ADCINC12_iGetData()+2048);
	ADCINC12_ClearFlag();

	if (wVal >= wZero)
		return wVal-wZero;
	else
		return 0;
}