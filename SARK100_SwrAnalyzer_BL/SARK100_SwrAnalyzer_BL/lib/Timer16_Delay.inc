;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME: Timer16_Delay.inc
;;   Version: 2.6, Updated on 2009/6/24 at 12:17:24
;;  Generated by PSoC Designer 
;;
;;  DESCRIPTION: Assembler declarations for the Timer16 user module interface
;;               for the 22/24/27/29xxx PSoC family of devices
;;-----------------------------------------------------------------------------
;;  Copyright (c) Cypress MicroSystems 2000-2004. All Rights Reserved.
;;*****************************************************************************
;;*****************************************************************************

include "m8c.inc"


;--------------------------------------------------
; Constants for Timer16_Delay API's.
;--------------------------------------------------

Timer16_Delay_CONTROL_REG_START_BIT:       equ 0x01    ; Control register start bit
Timer16_Delay_INT_REG:                     equ 0x0e1
Timer16_Delay_INT_MASK:                    equ 0x10


;--------------------------------------------------
; Constants for Timer16_Delay user defined values
;--------------------------------------------------

Timer16_Delay_PERIOD:                      equ 0x0
Timer16_Delay_COMPARE_VALUE:               equ 0x0


;--------------------------------------------------
; Register Address Constants for Timer16_Delay
;--------------------------------------------------

Timer16_Delay_COUNTER_LSB_REG:         equ 0x2c   ; DR0 Counter register
Timer16_Delay_COUNTER_MSB_REG:         equ 0x30
Timer16_Delay_PERIOD_LSB_REG:          equ 0x2d   ; DR1 Period register
Timer16_Delay_PERIOD_MSB_REG:          equ 0x31
Timer16_Delay_COMPARE_LSB_REG:         equ 0x2e   ; DR2 CompareValue register
Timer16_Delay_COMPARE_MSB_REG:         equ 0x32
Timer16_Delay_CONTROL_LSB_REG:         equ 0x2f   ; Control register
Timer16_Delay_CONTROL_MSB_REG:         equ 0x33
Timer16_Delay_FUNC_LSB_REG:            equ 0x2c   ; Function register
Timer16_Delay_FUNC_MSB_REG:            equ 0x30
Timer16_Delay_INPUT_LSB_REG:           equ 0x2d   ; Input register
Timer16_Delay_INPUT_MSB_REG:           equ 0x31
Timer16_Delay_OUTPUT_LSB_REG:          equ 0x2e   ; Output register
Timer16_Delay_OUTPUT_MSB_REG:          equ 0x32


;--------------------------------------------------
; Timer16_Delay Macro 'Functions'
;--------------------------------------------------

   macro Timer16_Delay_Start_M
   or    reg[Timer16_Delay_CONTROL_LSB_REG],  Timer16_Delay_CONTROL_REG_START_BIT
   endm

   macro Timer16_Delay_Stop_M
   and   reg[Timer16_Delay_CONTROL_LSB_REG], ~Timer16_Delay_CONTROL_REG_START_BIT
   endm

   macro Timer16_Delay_EnableInt_M
   M8C_EnableIntMask Timer16_Delay_INT_REG, Timer16_Delay_INT_MASK
   endm

   macro Timer16_Delay_DisableInt_M
   M8C_DisableIntMask Timer16_Delay_INT_REG, Timer16_Delay_INT_MASK
   endm


; end of file Timer16_Delay.inc