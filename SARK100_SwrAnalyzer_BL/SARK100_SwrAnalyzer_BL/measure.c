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
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	DEC2009		Creation
//
//*****************************************************************************/

#include <m8c.h>        // part specific constants and macros
#include <math.h>

#include "PSoCAPI.h"
#include "psocgpioint.h"
#include "util.h"

#include "measure.h"
#include "glb_data.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Typedefs
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
static void WaitPgaSettling (void);
static DWORD TakeSample (void);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	Do_Measure
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
void Do_Measure ( void )
{
	ADCINC12_Start(ADCINC12_HIGHPOWER); // Turn on Analog section
	PGA_ADC_Start(PGA_ADC_HIGHPOWER);
	
	PGA_ADC_SetGain(PGA_ADC_G2_67);
	WaitPgaSettling();

	// Read Vf 
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_1); 				//VF

	g_xBridgeMeasure.Vf = TakeSample(); 				// Get Data
	if (g_xBridgeOffset.Vf > g_xBridgeMeasure.Vf)
		g_xBridgeMeasure.Vf = 0;
	else
		g_xBridgeMeasure.Vf -= g_xBridgeOffset.Vf;
	g_xBridgeMeasure.Vf *= g_xBridgeCorrect.Vf;		
	
	// Read Va
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_7); 				//VA
	g_xBridgeMeasure.Va = TakeSample(); 				// Get Data
	if (g_xBridgeOffset.Va > g_xBridgeMeasure.Va)
		g_xBridgeMeasure.Va = 0;
	else
		g_xBridgeMeasure.Va -= g_xBridgeOffset.Va;
	g_xBridgeMeasure.Va *= g_xBridgeCorrect.Va;

	// Read Vz
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_5); 				//VZ
	g_xBridgeMeasure.Vz = TakeSample(); 				// Get Data
	if (g_xBridgeOffset.Vz > g_xBridgeMeasure.Vz)
		g_xBridgeMeasure.Vz = 0;
	else
		g_xBridgeMeasure.Vz -= g_xBridgeOffset.Vz;
	g_xBridgeMeasure.Vz *= g_xBridgeCorrect.Vz;

	// Read Vr 
	// Gain is set to double because dynamic range is half
	PGA_ADC_SetGain(PGA_ADC_G5_33);
	WaitPgaSettling();
	
	AMUX4_ADC_InputSelect(AMUX4_ADC_PORT0_3); 				//VR
	g_xBridgeMeasure.Vr = TakeSample(); 				// Get Data
	if (g_xBridgeOffset.Vr > g_xBridgeMeasure.Vr)
		g_xBridgeMeasure.Vr = 0;
	else
		g_xBridgeMeasure.Vr -= g_xBridgeOffset.Vr;
	g_xBridgeMeasure.Vr *= g_xBridgeCorrect.Vr;

	ADCINC12_Stop();
	PGA_ADC_Stop();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	WaitPgaSettling
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
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static DWORD TakeSample (void)
{
	WORD wVal = 0;
	WORD wZero = 0;
												//Correlated double sampling
	PGA_ADC_GAIN_CR1 &= ~1;						//Set input to RefLO
	ADCINC12_GetSamples(1);	
	while(ADCINC12_fIsDataAvailable() == 0); 			// Wait for data to be ready.
	wZero = (ADCINC12_iGetData()+2048); 				// Get Data
	ADCINC12_ClearFlag();
	
	PGA_ADC_GAIN_CR1 |= 1;						//Set input to PortInp
	ADCINC12_GetSamples(1);	
	while(ADCINC12_fIsDataAvailable() == 0); 			// Wait for data to be ready.
	wVal = (ADCINC12_iGetData()+2048); 				// Get Data
	ADCINC12_ClearFlag();

	if (wVal>=wZero)
		return wVal-wZero;
	else	
		return 0;
}