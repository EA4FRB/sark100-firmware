//*****************************************************************************
//*****************************************************************************
//  FILENAME:  ADCINC12.h
//  Version: 5.3, Updated on 2009/6/23 at 19:29:4
//  Generated by PSoC Designer 
//
//  DESCRIPTION:
//    C declarations for the ACDINC12 User Module.
//-----------------------------------------------------------------------------
//      Copyright (c) Cypress MicroSystems 2002-2003. All Rights Reserved.
//*****************************************************************************
//*****************************************************************************


#include <M8C.h>


#pragma fastcall16 ADCINC12_Start
#pragma fastcall16 ADCINC12_SetPower
#pragma fastcall16 ADCINC12_GetSamples
#pragma fastcall16 ADCINC12_StopAD
#pragma fastcall16 ADCINC12_Stop

#pragma fastcall16 ADCINC12_fIsDataAvailable
#pragma fastcall16 ADCINC12_iGetData
#pragma fastcall16 ADCINC12_ClearFlag

//-------------------------------------------------
// Prototypes of the ADCINC12 API.
//-------------------------------------------------
extern void ADCINC12_Start(BYTE  bPower);
extern void ADCINC12_SetPower(BYTE bPower);
extern void ADCINC12_GetSamples(BYTE bNumSamples);
extern void ADCINC12_StopAD(void);
extern void ADCINC12_Stop(void);

extern BYTE ADCINC12_fIsDataAvailable(void);
extern INT  ADCINC12_iGetData(void);
extern void ADCINC12_ClearFlag(void);

// deprecated function!
#pragma fastcall16 ADCINC12_fIsData
extern BYTE ADCINC12_fIsData(void);


//-------------------------------------------------
// Constants for ADCINC12 API's.
//-------------------------------------------------
#define ADCINC12_OFF         0
#define ADCINC12_LOWPOWER    1
#define ADCINC12_MEDPOWER    2
#define ADCINC12_HIGHPOWER   3

//-------------------------------------------------
// Register Addresses for ADCINC12
//-------------------------------------------------

#pragma ioport  ADCINC12_AtoDcr0:   0x080
BYTE            ADCINC12_AtoDcr0;
#pragma ioport  ADCINC12_AtoDcr1:   0x081
BYTE            ADCINC12_AtoDcr1;
#pragma ioport  ADCINC12_AtoDcr2:   0x082
BYTE            ADCINC12_AtoDcr2;
#pragma ioport  ADCINC12_AtoDcr3:   0x083
BYTE            ADCINC12_AtoDcr3;
#pragma ioport  ADCINC12_CounterFN: 0x124
BYTE            ADCINC12_CounterFN;
#pragma ioport  ADCINC12_CounterSL: 0x125
BYTE            ADCINC12_CounterSL;
#pragma ioport  ADCINC12_CounterOS: 0x126
BYTE            ADCINC12_CounterOS;
#pragma ioport  ADCINC12_CounterDR0:    0x024
BYTE            ADCINC12_CounterDR0;
#pragma ioport  ADCINC12_CounterDR1:    0x025
BYTE            ADCINC12_CounterDR1;
#pragma ioport  ADCINC12_CounterDR2:    0x026
BYTE            ADCINC12_CounterDR2;
#pragma ioport  ADCINC12_CounterCR0:    0x027
BYTE            ADCINC12_CounterCR0;
#pragma ioport  ADCINC12_TimerFN:   0x120
BYTE            ADCINC12_TimerFN;
#pragma ioport  ADCINC12_TimerSL:   0x121
BYTE            ADCINC12_TimerSL;
#pragma ioport  ADCINC12_TimerOS:   0x122
BYTE            ADCINC12_TimerOS;
#pragma ioport  ADCINC12_TimerDR0:  0x020
BYTE            ADCINC12_TimerDR0;
#pragma ioport  ADCINC12_TimerDR1:  0x021
BYTE            ADCINC12_TimerDR1;
#pragma ioport  ADCINC12_TimerDR2:  0x022
BYTE            ADCINC12_TimerDR2;
#pragma ioport  ADCINC12_TimerCR0:  0x023
BYTE            ADCINC12_TimerCR0;
