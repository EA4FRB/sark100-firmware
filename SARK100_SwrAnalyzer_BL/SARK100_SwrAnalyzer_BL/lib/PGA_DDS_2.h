//*****************************************************************************
//*****************************************************************************
//  FILENAME:  PGA_DDS_2.h  ( PGA )
//  Version: 3.2, Updated on 2009/6/23 at 19:34:6
//  Generated by PSoC Designer 
//
//  DESCRIPTION:  PGA User Module C Language interface file for the
//                22/24/27/29xxx PSoC family of devices.
//-----------------------------------------------------------------------------
//      Copyright (c) Cypress MicroSystems 2001-2004. All Rights Reserved.
//*****************************************************************************
//*****************************************************************************
#include <M8C.h>

//-------------------------------------------------
// Constants for PGA_DDS_2 API's.
//-------------------------------------------------
#define PGA_DDS_2_OFF         0
#define PGA_DDS_2_LOWPOWER    1
#define PGA_DDS_2_MEDPOWER    2
#define PGA_DDS_2_HIGHPOWER   3

#define PGA_DDS_2_G48_0    0x0C
#define PGA_DDS_2_G24_0    0x1C

#define PGA_DDS_2_G16_0    0x08
#define PGA_DDS_2_G8_00    0x18
#define PGA_DDS_2_G5_33    0x28
#define PGA_DDS_2_G4_00    0x38
#define PGA_DDS_2_G3_20    0x48
#define PGA_DDS_2_G2_67    0x58
#define PGA_DDS_2_G2_27    0x68
#define PGA_DDS_2_G2_00    0x78
#define PGA_DDS_2_G1_78    0x88
#define PGA_DDS_2_G1_60    0x98
#define PGA_DDS_2_G1_46    0xA8
#define PGA_DDS_2_G1_33    0xB8
#define PGA_DDS_2_G1_23    0xC8
#define PGA_DDS_2_G1_14    0xD8
#define PGA_DDS_2_G1_06    0xE8
#define PGA_DDS_2_G1_00    0xF8
#define PGA_DDS_2_G0_93    0xE0
#define PGA_DDS_2_G0_87    0xD0
#define PGA_DDS_2_G0_81    0xC0
#define PGA_DDS_2_G0_75    0xB0
#define PGA_DDS_2_G0_68    0xA0
#define PGA_DDS_2_G0_62    0x90
#define PGA_DDS_2_G0_56    0x80
#define PGA_DDS_2_G0_50    0x70
#define PGA_DDS_2_G0_43    0x60
#define PGA_DDS_2_G0_37    0x50
#define PGA_DDS_2_G0_31    0x40
#define PGA_DDS_2_G0_25    0x30
#define PGA_DDS_2_G0_18    0x20
#define PGA_DDS_2_G0_12    0x10
#define PGA_DDS_2_G0_06    0x00



#pragma fastcall16 PGA_DDS_2_Start
#pragma fastcall16 PGA_DDS_2_SetPower
#pragma fastcall16 PGA_DDS_2_SetGain
#pragma fastcall16 PGA_DDS_2_Stop

//-------------------------------------------------
// Prototypes of the PGA_DDS_2 API.
//-------------------------------------------------
extern void PGA_DDS_2_Start(BYTE bPowerSetting);     // Proxy class 2
extern void PGA_DDS_2_SetPower(BYTE bPowerSetting);  // Proxy class 2
extern void PGA_DDS_2_SetGain(BYTE bGainSetting);    // Proxy class 2
extern void PGA_DDS_2_Stop(void);                    // Proxy class 1

//-------------------------------------------------
// Register Addresses for PGA_DDS_2
//-------------------------------------------------

#pragma ioport  PGA_DDS_2_GAIN_CR0: 0x079
BYTE            PGA_DDS_2_GAIN_CR0;
#pragma ioport  PGA_DDS_2_GAIN_CR1: 0x07a
BYTE            PGA_DDS_2_GAIN_CR1;
#pragma ioport  PGA_DDS_2_GAIN_CR2: 0x07b
BYTE            PGA_DDS_2_GAIN_CR2;
#pragma ioport  PGA_DDS_2_GAIN_CR3: 0x078
BYTE            PGA_DDS_2_GAIN_CR3;


// end of file PGA_DDS_2.h