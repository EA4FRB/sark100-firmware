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
// 	FILE NAME: 	STORAGE.C
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
#include <stdlib.h>
#include <string.h>
#include <m8c.h>
#include "PSoCAPI.h"

#include "glb_data.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------
#define offsetof(st, m) \
    ((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define MAGIC_NUMBER	0x55ab

//-----------------------------------------------------------------------------
//  Typedefs
//-----------------------------------------------------------------------------
typedef struct
{
	// Calibration data
	BRIDGE_VOLTAGES xBandCorrFactor[BAND_MAX];	
	BYTE bGainDDS[BAND_MAX];	
	BRIDGE_VOLTAGES xBridgeOffset;
	BYTE bIsCalibrated;
	
	// Configuration data
	CONFIG_DATA xConf;
	
	// Integrity control
	WORD wMagic;
} RECORD_DATA;

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

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	STR_SaveCalibration
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
void STR_SaveCalibration ( void )
{
	E2PROM_bE2Write(offsetof(RECORD_DATA,xBandCorrFactor), (unsigned char*) &g_xBandCorrFactor[0], sizeof(g_xBandCorrFactor), 25);
	E2PROM_bE2Write(offsetof(RECORD_DATA,xBridgeOffset), (unsigned char*)&g_xBridgeOffset, sizeof(g_xBridgeOffset), 25);
	E2PROM_bE2Write(offsetof(RECORD_DATA,bIsCalibrated), &g_bIsCalibrated, sizeof(g_bIsCalibrated), 25);
	E2PROM_bE2Write(offsetof(RECORD_DATA,bGainDDS), (unsigned char*) &g_bGainDDS[0], sizeof(g_bGainDDS), 25);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	STR_SaveConfig
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
void STR_SaveConfig ( void )
{
	E2PROM_bE2Write(offsetof(RECORD_DATA,xConf), (unsigned char*)&g_xConf, sizeof(g_xConf), 25);
}
//-----------------------------------------------------------------------------
//  FUNCTION NAME:	
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
void STR_Restore ( void )
{
	WORD magic;
	BYTE bBand;
	CHAR cRc;
	
	E2PROM_E2Read(offsetof(RECORD_DATA,wMagic), (unsigned char*) &magic, sizeof(WORD));
	if (magic != MAGIC_NUMBER)
	{
		// Set defaults
		for (bBand=0;bBand<BAND_MAX;bBand++)
		{
			g_xBandCorrFactor[bBand].Vz = 100;
			g_xBandCorrFactor[bBand].Vr = 100; 
			g_xBandCorrFactor[bBand].Va = 100; 
			g_xBandCorrFactor[bBand].Vf = 100;
			
			g_bGainDDS[bBand] = g_bDefGainDdsIdx[bBand];
		}		
		g_xBridgeOffset.Vf = 0;
		g_xBridgeOffset.Vr = 0;
		g_xBridgeOffset.Vz = 0;
		g_xBridgeOffset.Va = 0;
		
		g_bIsCalibrated = FALSE;
		
		STR_SaveCalibration();
				
		g_xConf.bStep = STEP_10KHZ;
		STR_SaveConfig();

		magic = MAGIC_NUMBER;
		E2PROM_bE2Write(offsetof(RECORD_DATA,wMagic), (unsigned char*) &magic, sizeof(magic), 25);
	}
	E2PROM_E2Read(offsetof(RECORD_DATA,xBandCorrFactor), (unsigned char*) &g_xBandCorrFactor[0], sizeof(g_xBandCorrFactor));
	E2PROM_E2Read(offsetof(RECORD_DATA,bIsCalibrated), &g_bIsCalibrated, sizeof(g_bIsCalibrated));
	E2PROM_E2Read(offsetof(RECORD_DATA,bGainDDS), (unsigned char*)&g_bGainDDS[0], sizeof(g_bGainDDS));
	E2PROM_E2Read(offsetof(RECORD_DATA,xBridgeOffset), (unsigned char*)&g_xBridgeOffset, sizeof(g_xBridgeOffset));
	E2PROM_E2Read(offsetof(RECORD_DATA,xConf), (unsigned char*)&g_xConf, sizeof(g_xConf));
}
