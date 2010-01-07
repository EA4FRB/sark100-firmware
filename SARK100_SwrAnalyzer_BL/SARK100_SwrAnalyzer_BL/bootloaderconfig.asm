include "m8c.inc"
include "memory.inc"
include "bootloader.inc"

Area BootLoaderArea(ROM,REL)

;-----------------------------------------------
;  Global Symbols
;-----------------------------------------------
export   Boot_RxD_Start
export  _Boot_RxD_Start
export   Boot_RxD_bReadRxData
export  _Boot_RxD_bReadRxData
export   Boot_RxD_bReadRxStatus
export  _Boot_RxD_bReadRxStatus

export   Boot_TxD_Start
export  _Boot_TxD_Start
export   Boot_TxD_SendData
export  _Boot_TxD_SendData
export   Boot_TxD_bReadTxStatus
export  _Boot_TxD_bReadTxStatus

export  Boot_Counter_DisableInt
export _Boot_Counter_DisableInt
export  Boot_Counter_Start
export _Boot_Counter_Start
export  Boot_Counter_Stop
export _Boot_Counter_Stop
export  Boot_Counter_WritePeriod
export _Boot_Counter_WritePeriod
export  Boot_Counter_WriteCompareValue
export _Boot_Counter_WriteCompareValue
export  Boot_Counter_wReadCounter
export _Boot_Counter_wReadCounter




//---------------------------------------------------------------------------------
// Configuration
//---------------------------------------------------------------------------------

export Boot_LoadConfigInit
export _Boot_LoadConfigInit

//export NO_SHADOW
//export _NO_SHADOW

FLAG_CFG_MASK:      equ 10h         ;M8C flag register REG address bit mask
END_CONFIG_TABLE:   equ ffh         ;end of config table indicator

Area BootLoaderArea(ROM,REL)

;---------------------------------------------------------------------------
; LoadConfigInit - Establish the start-up configuration (except for a few
;                  parameters handled by boot code, like CPU speed). This
;                  function can be called from user code, but typically it
;                  is only called from boot.
;
;       INPUTS: None.
;      RETURNS: Nothing.
; SIDE EFFECTS: Registers are volatile: the A and X registers can be modified!
;               In the large memory model currently only the page
;               pointer registers listed below are modified.  This does
;               not guarantee that in future implementations of this
;               function other page pointer registers will not be
;               modified.
;          
;               Page Pointer Registers Modified: 
;               CUR_PP
;
_Boot_LoadConfigInit:
 Boot_LoadConfigInit:
    RAM_PROLOGUE RAM_USE_CLASS_4
    
	lcall	LoadConfigTBL_bootloader_cy8c29_Ordered
	lcall	LoadConfig_bootloader_cy8c29

    RAM_EPILOGUE RAM_USE_CLASS_4
    ret

;---------------------------------------------------------------------------
; Load Configuration bootloader_cy8c29
;
;    Load configuration registers for bootloader_cy8c29.
;    IO Bank 0 registers a loaded first,then those in IO Bank 1.
;
;       INPUTS: None.
;      RETURNS: Nothing.
; SIDE EFFECTS: Registers are volatile: the CPU A and X registers may be
;               modified as may the Page Pointer registers!
;               In the large memory model currently only the page
;               pointer registers listed below are modified.  This does
;               not guarantee that in future implementations of this
;               function other page pointer registers will not be
;               modified.
;          
;               Page Pointer Registers Modified: 
;               CUR_PP
;
_LoadConfig_bootloader_cy8c29:
 LoadConfig_bootloader_cy8c29:
    RAM_PROLOGUE RAM_USE_CLASS_4

	push	x
    M8C_SetBank0                    ; Force bank 0
    mov     a, 0                    ; Specify bank 0
    asr     a                       ; Store in carry flag
                                    ; Load bank 0 table:
    mov     A, >LoadConfigTBL_bootloader_cy8c29_Bank0
    mov     X, <LoadConfigTBL_bootloader_cy8c29_Bank0
    lcall   LoadConfig              ; Load the bank 0 values

    mov     a, 1                    ; Specify bank 1
    asr     a                       ; Store in carry flag
                                    ; Load bank 1 table:
    mov     A, >LoadConfigTBL_bootloader_cy8c29_Bank1
    mov     X, <LoadConfigTBL_bootloader_cy8c29_Bank1
    lcall   LoadConfig              ; Load the bank 1 values

	pop		x

    RAM_EPILOGUE RAM_USE_CLASS_4
    ret




;---------------------------------------------------------------------------
; LoadConfig - Set IO registers as specified in ROM table of (address,value)
;              pairs. Terminate on address=0xFF.
;
;  INPUTS:  [A,X] points to the table to be loaded
;           Flag Register Carry bit encodes the Register Bank
;           (Carry=0 => Bank 0; Carry=1 => Bank 1)
;
;  RETURNS: nothing.
;
;  STACK FRAME:  X-4 I/O Bank 0/1 indicator
;                X-3 Temporary store for register address
;                X-2 LSB of config table address
;                X-1 MSB of config table address
;
LoadConfig:
    RAM_PROLOGUE RAM_USE_CLASS_2
    add     SP, 2                   ; Set up local vars
    push    X                       ; Save config table address on stack
    push    A
    mov     X, SP
    mov     [X-4], 0                ; Set default Destination to Bank 0
    jnc     .BankSelectSaved        ; Carry says Bank 0 is OK
    mov     [X-4], 1                ; No Carry: default to Bank 1
.BankSelectSaved:
    pop     A
    pop     X

LoadConfigLp:
    M8C_SetBank0                    ; Switch to bank 0
    M8C_ClearWDT                    ; Clear the watchdog for long inits
    push    X                       ; Preserve the config table address
    push    A
    romx                            ; Load register address from table
    cmp     A, END_CONFIG_TABLE     ; End of table?
    jz      EndLoadConfig           ;   Yes, go wrap it up
    mov     X, SP                   ;
    tst     [X-4], 1                ; Loading IO Bank 1?
    jz      .IOBankNowSet           ;    No, Bank 0 is fine
    M8C_SetBank1                    ;   Yes, switch to Bank 1
.IOBankNowSet:
    mov     [X-3], A                ; Stash the register address
    pop     A                       ; Retrieve the table address
    pop     X
    inc     X                       ; Advance to the data byte
    adc     A, 0
    push    X                       ; Save the config table address again
    push    A
    romx                            ; load config data from the table
    mov     X, SP                   ; retrieve the register address
    mov     X, [X-3]
    mov     reg[X], A               ; Configure the register
    pop     A                       ; retrieve the table address
    pop     X
    inc     X                       ; advance to next table entry
    adc     A, 0
    jmp     LoadConfigLp            ; loop to configure another register
EndLoadConfig:
    add     SP, -4
    RAM_EPILOGUE RAM_USE_CLASS_2
    ret

LoadConfigTBL_bootloader_cy8c29_Ordered:
;  Ordered Global Register values
	M8C_SetBank1
	mov	reg[00h], 10h		; Port_0_DriveMode_0 register (PRT0DM0)
	mov	reg[01h], efh		; Port_0_DriveMode_1 register (PRT0DM1)
	M8C_SetBank0
	mov	reg[03h], efh		; Port_0_DriveMode_2 register (PRT0DM2)
	mov	reg[02h], 10h		; Port_0_GlobalSelect register (PRT0GS)
	M8C_SetBank1
	mov	reg[02h], 00h		; Port_0_IntCtrl_0 register (PRT0IC0)
	mov	reg[03h], 00h		; Port_0_IntCtrl_1 register (PRT0IC1)
	M8C_SetBank0
	mov	reg[01h], 00h		; Port_0_IntEn register (PRT0IE)
	M8C_SetBank1
	mov	reg[04h], 00h		; Port_1_DriveMode_0 register (PRT1DM0)
	mov	reg[05h], ffh		; Port_1_DriveMode_1 register (PRT1DM1)
	M8C_SetBank0
	mov	reg[07h], f7h		; Port_1_DriveMode_2 register (PRT1DM2)
	mov	reg[06h], 08h		; Port_1_GlobalSelect register (PRT1GS)
	M8C_SetBank1
	mov	reg[06h], 00h		; Port_1_IntCtrl_0 register (PRT1IC0)
	mov	reg[07h], 00h		; Port_1_IntCtrl_1 register (PRT1IC1)
	M8C_SetBank0
	mov	reg[05h], 00h		; Port_1_IntEn register (PRT1IE)
	M8C_SetBank1
	mov	reg[08h], 00h		; Port_2_DriveMode_0 register (PRT2DM0)
	mov	reg[09h], ffh		; Port_2_DriveMode_1 register (PRT2DM1)
	M8C_SetBank0
	mov	reg[0bh], ffh		; Port_2_DriveMode_2 register (PRT2DM2)
	mov	reg[0ah], 00h		; Port_2_GlobalSelect register (PRT2GS)
	M8C_SetBank1
	mov	reg[0ah], 00h		; Port_2_IntCtrl_0 register (PRT2IC0)
	mov	reg[0bh], 00h		; Port_2_IntCtrl_1 register (PRT2IC1)
	M8C_SetBank0
	mov	reg[09h], 00h		; Port_2_IntEn register (PRT2IE)
	M8C_SetBank1
	mov	reg[0ch], 00h		; Port_3_DriveMode_0 register (PRT3DM0)
	mov	reg[0dh], 00h		; Port_3_DriveMode_1 register (PRT3DM1)
	M8C_SetBank0
	mov	reg[0fh], 00h		; Port_3_DriveMode_2 register (PRT3DM2)
	mov	reg[0eh], 00h		; Port_3_GlobalSelect register (PRT3GS)
	M8C_SetBank1
	mov	reg[0eh], 00h		; Port_3_IntCtrl_0 register (PRT3IC0)
	mov	reg[0fh], 00h		; Port_3_IntCtrl_1 register (PRT3IC1)
	M8C_SetBank0
	mov	reg[0dh], 00h		; Port_3_IntEn register (PRT3IE)
	M8C_SetBank1
	mov	reg[10h], 00h		; Port_4_DriveMode_0 register (PRT4DM0)
	mov	reg[11h], 00h		; Port_4_DriveMode_1 register (PRT4DM1)
	M8C_SetBank0
	mov	reg[13h], 00h		; Port_4_DriveMode_2 register (PRT4DM2)
	mov	reg[12h], 00h		; Port_4_GlobalSelect register (PRT4GS)
	M8C_SetBank1
	mov	reg[12h], 00h		; Port_4_IntCtrl_0 register (PRT4IC0)
	mov	reg[13h], 00h		; Port_4_IntCtrl_1 register (PRT4IC1)
	M8C_SetBank0
	mov	reg[11h], 00h		; Port_4_IntEn register (PRT4IE)
	M8C_SetBank1
	mov	reg[14h], 00h		; Port_5_DriveMode_0 register (PRT5DM0)
	mov	reg[15h], 00h		; Port_5_DriveMode_1 register (PRT5DM1)
	M8C_SetBank0
	mov	reg[17h], 00h		; Port_5_DriveMode_2 register (PRT5DM2)
	mov	reg[16h], 00h		; Port_5_GlobalSelect register (PRT5GS)
	M8C_SetBank1
	mov	reg[16h], 00h		; Port_5_IntCtrl_0 register (PRT5IC0)
	mov	reg[17h], 00h		; Port_5_IntCtrl_1 register (PRT5IC1)
	M8C_SetBank0
	mov	reg[15h], 00h		; Port_5_IntEn register (PRT5IE)
	M8C_SetBank1
	mov	reg[18h], 00h		; Port_6_DriveMode_0 register (PRT6DM0)
	mov	reg[19h], 00h		; Port_6_DriveMode_1 register (PRT6DM1)
	M8C_SetBank0
	mov	reg[1bh], 00h		; Port_6_DriveMode_2 register (PRT6DM2)
	mov	reg[1ah], 00h		; Port_6_GlobalSelect register (PRT6GS)
	M8C_SetBank1
	mov	reg[1ah], 00h		; Port_6_IntCtrl_0 register (PRT6IC0)
	mov	reg[1bh], 00h		; Port_6_IntCtrl_1 register (PRT6IC1)
	M8C_SetBank0
	mov	reg[19h], 00h		; Port_6_IntEn register (PRT6IE)
	M8C_SetBank1
	mov	reg[1ch], 00h		; Port_7_DriveMode_0 register (PRT7DM0)
	mov	reg[1dh], 00h		; Port_7_DriveMode_1 register (PRT7DM1)
	M8C_SetBank0
	mov	reg[1fh], 00h		; Port_7_DriveMode_2 register (PRT7DM2)
	mov	reg[1eh], 00h		; Port_7_GlobalSelect register (PRT7GS)
	M8C_SetBank1
	mov	reg[1eh], 00h		; Port_7_IntCtrl_0 register (PRT7IC0)
	mov	reg[1fh], 00h		; Port_7_IntCtrl_1 register (PRT7IC1)
	M8C_SetBank0
	mov	reg[1dh], 00h		; Port_7_IntEn register (PRT7IE)
	ret
LoadConfigTBL_bootloader_cy8c29_Bank0:
;  Instance name Boot_Counter, User Module Counter16
;       Instance name Boot_Counter, Block Name CNTR16_LSB(DBB00)
	db		23h, 00h		;Boot_Counter_CONTROL_LSB_REG(DBB00CR0)
	db		21h, 00h		;Boot_Counter_PERIOD_LSB_REG(DBB00DR1)
	db		22h, 00h		;Boot_Counter_COMPARE_LSB_REG(DBB00DR2)
;       Instance name Boot_Counter, Block Name CNTR16_MSB(DBB01)
	db		27h, 00h		;Boot_Counter_CONTROL_MSB_REG(DBB01CR0)
	db		25h, 80h		;Boot_Counter_PERIOD_MSB_REG(DBB01DR1)
	db		26h, 40h		;Boot_Counter_COMPARE_MSB_REG(DBB01DR2)
;  Instance name Boot_RxD, User Module RX8
;       Instance name Boot_RxD, Block Name RX8(DCB03)
	db		2fh, 00h		;Boot_RxD_CONTROL_REG  (DCB03CR0)
	db		2dh, 00h		;Boot_RxD_(DCB03DR1)
	db		2eh, 00h		;Boot_RxD_RX_BUFFER_REG(DCB03DR2)
;  Instance name Boot_TxD, User Module TX8
;       Instance name Boot_TxD, Block Name TX8(DCB02)
	db		2bh, 00h		;Boot_TxD_CONTROL_REG  (DCB02CR0)
	db		29h, 00h		;Boot_TxD_TX_BUFFER_REG(DCB02DR1)
	db		2ah, 00h		;Boot_TxD_(DCB02DR2)
;  Global Register values Bank 0
	db		60h, 28h		; AnalogColumnInputSelect register (AMX_IN)
	db		66h, 00h		; AnalogComparatorControl1 register (CMP_CR1)
	db		63h, 05h		; AnalogReferenceControl register (ARF_CR)
	db		65h, 00h		; AnalogSyncControl register (ASY_CR)
	db		e6h, 00h		; DecimatorControl_0 register (DEC_CR0)
	db		e7h, 00h		; DecimatorControl_1 register (DEC_CR1)
	db		d6h, 00h		; I2CConfig register (I2C_CFG)
	db		b0h, 90h		; Row_0_InputMux register (RDI0RI)
	db		b1h, 00h		; Row_0_InputSync register (RDI0SYN)
	db		b2h, 00h		; Row_0_LogicInputAMux register (RDI0IS)
	db		b3h, 33h		; Row_0_LogicSelect_0 register (RDI0LT0)
	db		b4h, 33h		; Row_0_LogicSelect_1 register (RDI0LT1)
	db		b5h, 02h		; Row_0_OutputDrive_0 register (RDI0SRO0)
	db		b6h, 00h		; Row_0_OutputDrive_1 register (RDI0SRO1)
	db		b8h, 55h		; Row_1_InputMux register (RDI1RI)
	db		b9h, 00h		; Row_1_InputSync register (RDI1SYN)
	db		bah, 10h		; Row_1_LogicInputAMux register (RDI1IS)
	db		bbh, 33h		; Row_1_LogicSelect_0 register (RDI1LT0)
	db		bch, 33h		; Row_1_LogicSelect_1 register (RDI1LT1)
	db		bdh, 00h		; Row_1_OutputDrive_0 register (RDI1SRO0)
	db		beh, 00h		; Row_1_OutputDrive_1 register (RDI1SRO1)
	db		c0h, 00h		; Row_2_InputMux register (RDI2RI)
	db		c1h, 00h		; Row_2_InputSync register (RDI2SYN)
	db		c2h, 20h		; Row_2_LogicInputAMux register (RDI2IS)
	db		c3h, 33h		; Row_2_LogicSelect_0 register (RDI2LT0)
	db		c4h, 33h		; Row_2_LogicSelect_1 register (RDI3LT1)
	db		c5h, 00h		; Row_2_OutputDrive_0 register (RDI2SRO0)
	db		c6h, 00h		; Row_2_OutputDrive_1 register (RDI2SRO1)
	db		c8h, 55h		; Row_3_InputMux register (RDI3RI)
	db		c9h, 00h		; Row_3_InputSync register (RDI3SYN)
	db		cah, 30h		; Row_3_LogicInputAMux register (RDI3IS)
	db		cbh, 33h		; Row_3_LogicSelect_0 register (RDI3LT0)
	db		cch, 33h		; Row_3_LogicSelect_1 register (RDI3LT1)
	db		cdh, 00h		; Row_3_OutputDrive_0 register (RDI3SRO0)
	db		ceh, 00h		; Row_3_OutputDrive_1 register (RDI3SRO1)
	db		6ch, 00h		; TMP_DR0 register (TMP_DR0)
	db		6dh, 00h		; TMP_DR1 register (TMP_DR1)
	db		6eh, 00h		; TMP_DR2 register (TMP_DR2)
	db		6fh, 00h		; TMP_DR3 register (TMP_DR3)
	db		ffh

LoadConfigTBL_bootloader_cy8c29_Bank1:
;  Instance name Boot_Counter, User Module Counter16
;       Instance name Boot_Counter, Block Name CNTR16_LSB(DBB00)
	db		20h, 11h		;Boot_Counter_FUNC_LSB_REG(DBB00FN)
	db		21h, 17h		;Boot_Counter_INPUT_LSB_REG(DBB00IN)
	db		22h, 40h		;Boot_Counter_OUTPUT_LSB_REG(DBB00OU)
;       Instance name Boot_Counter, Block Name CNTR16_MSB(DBB01)
	db		24h, 31h		;Boot_Counter_FUNC_MSB_REG(DBB01FN)
	db		25h, 37h		;Boot_Counter_INPUT_MSB_REG(DBB01IN)
	db		26h, 40h		;Boot_Counter_OUTPUT_MSB_REG(DBB01OU)
;  Instance name Boot_RxD, User Module RX8
;       Instance name Boot_RxD, Block Name RX8(DCB03)
	db		2ch, 05h		;Boot_RxD_FUNC_REG     (DCB03FN)
	db		2dh, f1h		;Boot_RxD_INPUT_REG    (DCB03IN)
	db		2eh, 40h		;Boot_RxD_OUTPUT_REG   (DCB03OU)
;  Instance name Boot_TxD, User Module TX8
;       Instance name Boot_TxD, Block Name TX8(DCB02)
	db		28h, 1dh		;Boot_TxD_FUNC_REG     (DCB02FN)
	db		29h, 01h		;Boot_TxD_INPUT_REG    (DCB02IN)
	db		2ah, 44h		;Boot_TxD_OUTPUT_REG   (DCB02OU)
;  Global Register values Bank 1
	db		61h, 00h		; AnalogClockSelect1 register (CLK_CR1)
	db		69h, 00h		; AnalogClockSelect2 register (CLK_CR2)
	db		60h, 00h		; AnalogColumnClockSelect register (CLK_CR0)
	db		62h, 00h		; AnalogIOControl_0 register (ABF_CR0)
	db		67h, 33h		; AnalogLUTControl0 register (ALT_CR0)
	db		68h, 33h		; AnalogLUTControl1 register (ALT_CR1)
	db		63h, 00h		; AnalogModulatorControl_0 register (AMD_CR0)
	db		66h, 00h		; AnalogModulatorControl_1 register (AMD_CR1)
	db		d1h, 00h		; GlobalDigitalInterconnect_Drive_Even_Input register (GDI_E_IN)
	db		d3h, 00h		; GlobalDigitalInterconnect_Drive_Even_Output register (GDI_E_OU)
	db		d0h, 00h		; GlobalDigitalInterconnect_Drive_Odd_Input register (GDI_O_IN)
	db		d2h, 00h		; GlobalDigitalInterconnect_Drive_Odd_Output register (GDI_O_OU)
	db		e1h, 00h		; OscillatorControl_1 register (OSC_CR1)
	db		e2h, 00h		; OscillatorControl_2 register (OSC_CR2)
	db		dfh, 19h		; OscillatorControl_3 register (OSC_CR3)
	db		deh, 00h		; OscillatorControl_4 register (OSC_CR4)
	db		ddh, 00h		; OscillatorGlobalBusEnableControl register (OSC_GO_EN)
	db		e7h, 00h		; Type2Decimator_Control register (DEC_CR2)
	db		ffh

; PSoC Configuration file trailer PsocConfig.asm








;-----------------------------------------------
;  EQUATES
;-----------------------------------------------
bfCONTROL_REG_START_BIT:   equ   1     ; Control register start bit
bfFUNCTION_REG_TX_INT_MODE_BIT:	equ 0x10	; the TX Int Mode bit
INPUT_REG_NULL:                equ 0x00    ; Clear the input register		

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_RxD_Start(BYTE bParity)
;
;  DESCRIPTION:
;    Sets the start bit and parity in the Control register of this user module.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;    BYTE bParity - parity of received data.  Use defined masks.
;    passed in A register.
;
;  RETURNS: none
;
;  SIDE EFFECTS:
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_RxD_Start:
_Boot_RxD_Start:
   RAM_PROLOGUE RAM_USE_CLASS_1
   or    A, bfCONTROL_REG_START_BIT
   mov   REG[Boot_RxD_CONTROL_REG], A
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_Counter_Stop
;
;  DESCRIPTION:
;     Disables counter operation by clearing the start bit in the Control
;     register of the LSB block.
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:    None
;  RETURNS:      Nothing
;  SIDE EFFECTS: 
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_Counter_Stop:
_Boot_Counter_Stop:
   RAM_PROLOGUE RAM_USE_CLASS_1
   Boot_Counter_Stop_M
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret


.ENDSECTION


.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_RxD_bReadRxData
;
;  DESCRIPTION:
;     Reads the RX buffer register.  Should check the status regiser to make
;     sure data is valid.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:  none
;
;  RETURNS:
;    bRxData - returned in A.
;
;  SIDE EFFECTS:
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_RxD_bReadRxData:
_Boot_RxD_bReadRxData:
 bBoot_RxD_ReadRxData:
_bBoot_RxD_ReadRxData:
   RAM_PROLOGUE RAM_USE_CLASS_1
   mov A, REG[Boot_RxD_RX_BUFFER_REG]
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_RxD_bReadRxStatus
;
;  DESCRIPTION:
;    Reads the RX Status bits in the Control/Status register.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:  none
;
;  RETURNS:
;     BYTE  bRXStatus - transmit status data.  Use the following defined bits
;                       masks: RX_COMPLETE and RX_BUFFER_EMPTY
;
;  SIDE EFFECTS:
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_RxD_bReadRxStatus:
_Boot_RxD_bReadRxStatus:
 bBoot_RxD_ReadRxStatus:
_bBoot_RxD_ReadRxStatus:
   RAM_PROLOGUE RAM_USE_CLASS_1
   mov A,  REG[Boot_RxD_CONTROL_REG]
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_TxD_Start(BYTE bParity)
;
;  DESCRIPTION:
;     Sets the start bit and parity in the Control register of this user module.
;     The transmitter will begin transmitting if a byte has been written into the
;     transmit buffer.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;    BYTE bParity - parity of transmitted data.  Use defined masks.
;
;  RETURNS:  none
;
;  SIDE EFFECTS:
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_TxD_Start:
_Boot_TxD_Start:
   RAM_PROLOGUE RAM_USE_CLASS_1
   or    A, bfCONTROL_REG_START_BIT
   mov   REG[Boot_TxD_CONTROL_REG], A
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_TxD_SendData
;
;  DESCRIPTION:
;     Sends one byte through serial port.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;     BYTE  TxData - data to transmit.
;
;  RETURNS:
;
;  SIDE EFFECTS:
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_TxD_SendData:
_Boot_TxD_SendData:
   RAM_PROLOGUE RAM_USE_CLASS_1
   mov REG[Boot_TxD_TX_BUFFER_REG], A
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_TxD_bReadTxStatus
;
;  DESCRIPTION:
;     Reads the Tx Status bits in the Control/Status register.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;
;  RETURNS:
;     BYTE  bTxStatus - transmit status data.  Use the following defined bits
;                       masks: TX_COMPLETE and TX_BUFFER_EMPTY
;
;  SIDE EFFECTS:
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_TxD_bReadTxStatus:
_Boot_TxD_bReadTxStatus:
 bBoot_TxD_ReadTxStatus:
_bBoot_TxD_ReadTxStatus:
   RAM_PROLOGUE RAM_USE_CLASS_1
   mov A,  REG[Boot_TxD_CONTROL_REG]
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret

.ENDSECTION











.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_Counter_DisableInt
;
;  DESCRIPTION:
;     Disables this counter's interrupt by clearing the interrupt enable
;     mask bit associated with this User Module.
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:    None
;  RETURNS:      Nothing
;  SIDE EFFECTS: 
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_Counter_DisableInt:
_Boot_Counter_DisableInt:
   RAM_PROLOGUE RAM_USE_CLASS_1
   Boot_Counter_DisableInt_M
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret


.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_Counter_Start
;
;  DESCRIPTION:
;     Sets the start bit in the Control register of this user module.  The
;     counter will begin counting on the next input clock as soon as the
;     enable input is asserted high.
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:    None
;  RETURNS:      Nothing
;  SIDE EFFECTS: 
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_Counter_Start:
_Boot_Counter_Start:
   RAM_PROLOGUE RAM_USE_CLASS_1
   Boot_Counter_Start_M
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret


.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_Counter_WritePeriod
;
;  DESCRIPTION:
;     Write the 16-bit period value into the Period register (DR1).
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: fastcall16 WORD wPeriodValue (LSB in A, MSB in X)
;  RETURNS:   Nothing
;  SIDE EFFECTS:
;    If the counter user module is stopped, then this value will also be
;    latched into the Count registers (DR0).
;     
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_Counter_WritePeriod:
_Boot_Counter_WritePeriod:
   RAM_PROLOGUE RAM_USE_CLASS_1
   mov   reg[Boot_Counter_PERIOD_LSB_REG], A
   mov   A, X
   mov   reg[Boot_Counter_PERIOD_MSB_REG], A
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret


.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_Counter_WriteCompareValue
;
;  DESCRIPTION:
;     Writes compare value into the Compare register (DR2).
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:    fastcall16 WORD wCompareValue (LSB in A, MSB in X)
;  RETURNS:      Nothing
;  SIDE EFFECTS: 
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
 Boot_Counter_WriteCompareValue:
_Boot_Counter_WriteCompareValue:
   RAM_PROLOGUE RAM_USE_CLASS_1
   mov   reg[Boot_Counter_COMPARE_LSB_REG], A
   mov   A, X
   mov   reg[Boot_Counter_COMPARE_MSB_REG], A
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret


.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: Boot_Counter_wReadCounter
;
;  DESCRIPTION:
;     Returns the value in the Count register (DR0), preserving the value in
;     the compare register (DR2). Interrupts are prevented during the transfer
;     from the Count to the Compare registers by holding the clock low in
;     the MSB PSoC block.
;-----------------------------------------------------------------------------
;
;  ARGUMENTS: None
;  RETURNS:   fastcall16 WORD wCount (value of DR0 in the X & A registers)
;  SIDE EFFECTS:
;     1) The user module is stopped momentarily and one or more counts may be missed.
;     2) The A and X registers may be modified by this or future implementations
;        of this function.  The same is true for all RAM page pointer registers in
;        the Large Memory Model.  When necessary, it is the calling function's
;        responsibility to perserve their values across calls to fastcall16 
;        functions.
;
 Boot_Counter_wReadCounter:
_Boot_Counter_wReadCounter:
 wBoot_Counter_ReadCounter:                      ; this name deprecated
_wBoot_Counter_ReadCounter:                      ; this name deprecated

   bOrigCompareValue:      EQU   0                  ; Frame offset to temp Compare store
   bOrigControlReg:        EQU   2                  ; Frame offset to temp CR0     store
   bOrigClockSetting:      EQU   3                  ; Frame offset to temp Input   store
   wCounter:               EQU   4                  ; Frame offset to temp Count   store
   STACK_FRAME_SIZE:       EQU   6                  ; max stack frame size is 6 bytes

   RAM_PROLOGUE RAM_USE_CLASS_2
   mov   X, SP                                      ; X <-  stack frame pointer
   mov   A, reg[Boot_Counter_COMPARE_MSB_REG]    ; Save the Compare register on the stack
   push  A                                          ;
   mov   A, reg[Boot_Counter_COMPARE_LSB_REG]    ;
   push  A                                          ;   -stack frame now 2 bytes-
   mov   A, reg[Boot_Counter_CONTROL_LSB_REG]    ; Save CR0 (running or stopped state)
   push  A                                          ;   -stack frame now 3 bytes-
   Boot_Counter_Stop_M                           ; Disable (stop) the Counter if running
   M8C_SetBank1                                     ;
   mov   A, reg[Boot_Counter_INPUT_LSB_REG]      ; save the LSB clock input setting
   push  A                                          ;   on the stack (now 4 bytes) and ...
                                                    ;   hold the clock low:
   mov   reg[Boot_Counter_INPUT_LSB_REG], INPUT_REG_NULL
   M8C_SetBank0                                     ; Extract the Count via DR2 register
   mov   A, reg[Boot_Counter_COUNTER_MSB_REG]    ; DR2 <- DR0 (in the MSB block)
   mov   A, reg[Boot_Counter_COMPARE_MSB_REG]    ; Stash the Count MSB on the stack
   push  A                                          ;   -stack frame is now 5 bytes
   mov   A, reg[Boot_Counter_COUNTER_LSB_REG]    ; DR2 <- DR0 (in the LSB block)
   mov   A, reg[Boot_Counter_COMPARE_LSB_REG]    ; Stash the Count LSB on the stack
   push  A                                          ;   -stack frame is now 6 bytes-
   mov   A, [X+bOrigCompareValue]                   ; Restore the Compare MSB register
   mov   reg[Boot_Counter_COMPARE_MSB_REG], A    ;
   mov   A, [X+bOrigCompareValue+1]                 ; Restore the Compare LSB register
   mov   reg[Boot_Counter_COMPARE_LSB_REG], A    ;
   M8C_SetBank1                                     ; ---Restore the counter operation
   mov   A, [X+bOrigClockSetting]                   ; Grab the LSB clock setting...
   mov   reg[Boot_Counter_INPUT_LSB_REG], A      ;   and restore it
   M8C_SetBank0                                     ; Now re-enable (start) the counter
   mov   A, [X+bOrigControlReg]                     ;   if it was running when
   mov   reg[Boot_Counter_CONTROL_LSB_REG], A    ;   this function was first called
   pop   A                                          ; Setup the return value
   pop   X                                          ;
   ADD   SP, -(STACK_FRAME_SIZE-2)                  ; Zap remainder of stack frame
   RAM_EPILOGUE RAM_USE_CLASS_2
   ret

.ENDSECTION