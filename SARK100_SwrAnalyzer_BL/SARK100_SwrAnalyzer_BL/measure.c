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
//  Defines
//-----------------------------------------------------------------------------
#define NUM_AVE_SAMPLES			4		// Maximum 16

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static DWORD TakeSample (void);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Do_Measure
//
//  DESCRIPTION:
//
//	Measure reflectometer voltages.
//
//  ARGUMENTS:
//
//  RETURNS:
//     g_xBridgeMeasure
//
//-----------------------------------------------------------------------------
void Do_Measure ( void )
{
										// Read Vf
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_1);
	g_xBridgeMeasure.Vf = TakeSample();

										// Read Va
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_7);
	g_xBridgeMeasure.Va = TakeSample();

										// Read Vz
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_5);
	g_xBridgeMeasure.Vz = TakeSample();

										// Read Vr
										// Gain is set to double because dynamic range is half
	PGA_ADC_SetGain(PGA_ADC_G5_33);
	Delay_Ms(1);
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_3);
	g_xBridgeMeasure.Vr = TakeSample();

	PGA_ADC_SetGain(PGA_ADC_G2_67);		// Restores gain
}
//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Do_MeasureRfLevel
//
//  DESCRIPTION:
//
//	Measure Rf Level
//
//  ARGUMENTS:
//
//  RETURNS:
//     g_xBridgeMeasure
//
//-----------------------------------------------------------------------------
void Do_MeasureRfLevel ( void )
{
										// Read Vz
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_5);
	g_xBridgeMeasure.Vz = TakeSample();
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
	WORD wZero;
	BYTE ii;
										// Correlated double sampling
	PGA_ADC_GAIN_CR1 &= ~1;				// Set input to RefLO
	ADCINC12_GetSamples(1);
										// Wait for data to be ready.
	while(ADCINC12_fIsDataAvailable() == 0);
	wZero = (ADCINC12_iGetData()+2048);
	ADCINC12_ClearFlag();

	PGA_ADC_GAIN_CR1 |= 1;				// Set input to PortInp

	if (g_bScanning == FALSE)			// If not scanning do some averaging
	{
		ADCINC12_GetSamples(NUM_AVE_SAMPLES);
		for (ii=0;ii<NUM_AVE_SAMPLES;ii++)
		{
										// Wait for data to be ready.
			while(ADCINC12_fIsDataAvailable() == 0);
			wVal += (ADCINC12_iGetData()+2048);
			ADCINC12_ClearFlag();
		}
		wVal /= NUM_AVE_SAMPLES;
	}
	else
	{
		ADCINC12_GetSamples(1);
										// Wait for data to be ready.
		while(ADCINC12_fIsDataAvailable() == 0);
		wVal = (ADCINC12_iGetData()+2048);
		ADCINC12_ClearFlag();
	}	
	
	if (wVal >= wZero)
		return wVal-wZero;
	else
		return 0;
}

