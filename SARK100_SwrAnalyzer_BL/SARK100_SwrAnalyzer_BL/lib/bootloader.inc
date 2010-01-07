include "m8c.inc"

;--------------------------------------------------
; Constants for Boot_RxD API's
;--------------------------------------------------

;mask value for global int reg bit for RX instance
Boot_RxD_bINT_MASK:        equ 08h
;RX8 interrupt address
Boot_RxD_INT_REG:          equ 0e1h

Boot_RxD_RXBUF_ENABLE:                      equ   0

Boot_RxD_RX_IGNORE_BELOW:                    equ  20h
Boot_RxD_RX_BUFFER_SIZE:                     equ  10h
Boot_RxD_CMD_TERM:                           equ  dh
Boot_RxD_DELIMITER:                          equ  20h

; This equate will be removed in future revisions.
; Do not use.
bBoot_RxD_INT_MASK:        equ 08h
;------------------------------------
;  RX8 Parity masks
;------------------------------------
Boot_RxD_PARITY_NONE:           equ   0h
Boot_RxD_PARITY_EVEN:           equ   2h
Boot_RxD_PARITY_ODD:            equ   6h

;------------------------------------
;  RX8 Status Register masks
;------------------------------------

Boot_RxD_RX_ACTIVE:             equ   10h
Boot_RxD_RX_COMPLETE:           equ   08h
Boot_RxD_RX_REG_FULL:           equ   08h
Boot_RxD_RX_PARITY_ERROR:       equ   80h
Boot_RxD_RX_OVERRUN_ERROR:      equ   40h
Boot_RxD_RX_FRAMING_ERROR:      equ   20h
Boot_RxD_RX_ERROR:              equ   E0h
Boot_RxD_RX_NO_ERROR:           equ   E0h
Boot_RxD_RX_ENABLE:             equ   01h

Boot_RxD_RX_BUF_ERROR:                  equ   F0h  ; Mask for any Rx that may occur.
Boot_RxD_RX_BUF_OVERRUN:                equ   10h  ; This indicates the software buffer has
                                                        ; been over run.
Boot_RxD_RX_BUF_CMDTERM:                equ   01h  ; Command terminator has been received.

Boot_RxD_RX_NO_DATA:                    equ   01h
Boot_RxD_RX_NO_ERROR:                   equ   E0h

; Old equates, will be removed in future release
; Do not use
RX8_PARITY_NONE:           equ   0h
RX8_PARITY_EVEN:           equ   2h
RX8_PARITY_ODD:            equ   6h
RX8_RX_ACTIVE:             equ   10h
RX8_RX_COMPLETE:           equ   08h
RX8_RX_PARITY_ERROR:       equ   80h
RX8_RX_OVERRUN_ERROR:      equ   40h
RX8_RX_FRAMING_ERROR:      equ   20h
RX8_RX_NO_ERROR:           equ   E0h


;--------------------------------------------------
; Register Address Constants for Boot_RxD
;--------------------------------------------------
Boot_RxD_CONTROL_REG:   equ 2fh                  ;Control register
Boot_RxD_RX_SHIFT_REG:  equ 2ch                  ;RX Shift Register register
Boot_RxD_RX_BUFFER_REG: equ 2eh                  ;RX Buffer Register
Boot_RxD_FUNC_REG:  equ 2ch                      ;Function register
Boot_RxD_INPUT_REG: equ 2dh                      ;Input register
Boot_RxD_OUTPUT_REG:    equ 2eh                  ;Output register

; end of file Boot_RxD.inc







;---------------------------------------------------
; Constants for Boot_TxD API's
;---------------------------------------------------

;mask value for global int reg bit for TX instance
Boot_TxD_bINT_MASK:        equ 04h
;TX8 interrupt address
Boot_TxD_INT_REG:          equ 0e1h

; Old equate, will be removed in future release, Do not use.
bBoot_TxD_INT_MASK:        equ 04h


;------------------------------------
;  Transmitter Parity masks
;------------------------------------
Boot_TxD_PARITY_NONE:       equ   00h
Boot_TxD_PARITY_EVEN:       equ   02h
Boot_TxD_PARITY_ODD:        equ   06h

;------------------------------------
;  Transmitter Status Register masks
;------------------------------------
Boot_TxD_TX_COMPLETE:        equ   20h
Boot_TxD_TX_BUFFER_EMPTY:    equ   10h


Boot_TxD_INT_MODE_TX_REG_EMPTY:              equ 0x00
Boot_TxD_INT_MODE_TX_COMPLETE:               equ 0x01

; Do not use! old style equates.  Will be moved
; in future versions.
TX8_PARITY_NONE:           equ   00h
TX8_PARITY_EVEN:           equ   02h
TX8_PARITY_ODD:            equ   06h
TX8_TX_COMPLETE:           equ   20h
TX8_TX_BUFFER_EMPTY:       equ   10h

;----------------------------------------------------
;  Register Address Constants for Boot_TxD
;----------------------------------------------------
Boot_TxD_CONTROL_REG:   equ 2bh                  ; Control register
Boot_TxD_TX_SHIFT_REG:  equ 28h                  ; TX Shift Register register
Boot_TxD_TX_BUFFER_REG: equ 29h                  ; TX Buffer Register
Boot_TxD_FUNC_REG:  equ 28h                      ; Function register
Boot_TxD_INPUT_REG: equ 29h                      ; Input register
Boot_TxD_OUTPUT_REG:    equ 2ah                  ; Output register

; end of file Boot_TxD.inc









;--------------------------------------------------
; Constants for Boot_Counter API's.
;--------------------------------------------------

Boot_Counter_CONTROL_REG_START_BIT:        equ 0x01    ; Control register start bit
Boot_Counter_INT_REG:                      equ 0x0e1
Boot_Counter_INT_MASK:                     equ 0x02


;--------------------------------------------------
; Constants for Boot_Counter user defined values
;--------------------------------------------------

Boot_Counter_PERIOD:                       equ 0x8000
Boot_Counter_COMPARE_VALUE:                equ 0x4000


;--------------------------------------------------
; Register Address Constants for Boot_Counter
;--------------------------------------------------

Boot_Counter_COUNTER_LSB_REG:          equ 0x20   ; DR0 Counter register
Boot_Counter_COUNTER_MSB_REG:          equ 0x24
Boot_Counter_PERIOD_LSB_REG:           equ 0x21   ; DR1 Period register
Boot_Counter_PERIOD_MSB_REG:           equ 0x25
Boot_Counter_COMPARE_LSB_REG:          equ 0x22   ; DR2 CompareValue register
Boot_Counter_COMPARE_MSB_REG:          equ 0x26
Boot_Counter_CONTROL_LSB_REG:          equ 0x23   ; Control register
Boot_Counter_CONTROL_MSB_REG:          equ 0x27
Boot_Counter_FUNC_LSB_REG:             equ 0x20   ; Function register
Boot_Counter_FUNC_MSB_REG:             equ 0x24
Boot_Counter_INPUT_LSB_REG:            equ 0x21   ; Input register
Boot_Counter_INPUT_MSB_REG:            equ 0x25
Boot_Counter_OUTPUT_LSB_REG:           equ 0x22   ; Output register
Boot_Counter_OUTPUT_MSB_REG:           equ 0x26


;--------------------------------------------------
; Boot_Counter Macro 'Functions'
;--------------------------------------------------

   macro Boot_Counter_Start_M
   or    reg[Boot_Counter_CONTROL_LSB_REG],  Boot_Counter_CONTROL_REG_START_BIT
   endm

   macro Boot_Counter_Stop_M
   and   reg[Boot_Counter_CONTROL_LSB_REG], ~Boot_Counter_CONTROL_REG_START_BIT
   endm

   macro Boot_Counter_EnableInt_M
   M8C_EnableIntMask Boot_Counter_INT_REG, Boot_Counter_INT_MASK
   endm

   macro Boot_Counter_DisableInt_M
   M8C_DisableIntMask Boot_Counter_INT_REG, Boot_Counter_INT_MASK
   endm


; end of file Boot_Counter.inc

