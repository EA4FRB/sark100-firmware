;------------------------------------------------------------------------------
;   FILENAME:   FlashBlock.asm
;    @Version@
;------------------------------------------------------------------------------
;   DESCRIPTION:
;   Routines in this file perform Flash Block Write and Read operations for
;   Large Memory Model family members of the Ge family - CY8C27060 and CY8C29xxx
;------------------------------------------------------------------------------
;   Copyright (c) Cypress MicroSystems 2004. All rights reserved.
;------------------------------------------------------------------------------

export    bFlashWriteBlock
export   _bFlashWriteBlock
export    FlashReadBlock
export   _FlashReadBlock
export   FlashCheckSum
export   _FlashCheckSum
export   Boot_Is_Program_Good
export   _Boot_Is_Program_Good


include "m8c.inc"
include "memory.inc"
include "flashblock.inc"


Area BootLoaderArea(ROM,REL)
;--------------------------------------------
;  Declare Flash RAM storage at top of RAM
;     This data area is reserved for exclusive
;     use by Supervisory operations.
;--------------------------------------------
   area SSCParmBlk(RAM,ABS)
   org  00F8H


SSCParameterBlk:                   blk      8     ; reserve 8 bytes: F8 - FF for supervisory operation

;--------------------------------
; SSC_Action macro command codes
;--------------------------------
FLASH_OPER_KEY:                     equ      3Ah   ; flash operation key
FLASH_WRITE:                        equ      2     ; flash write operation supervisory command
FLASH_ERASE:                        equ      3     ; flash erase operation supervisory command
FLASH_TEMP_TABLE_LOOKUP:            equ      6     ; flash temperature table command

;-------------------------------------
; Flash temperature programming table
;-------------------------------------
TEMPERATURE_TABLE_ID:               equ      3     ; flash temperature programming table ID

;--------------------------------------------------------------
; Supervisory Operation Templates:
;     Each system supervisory call uses the reserved data area
;     a little different.  Create overlay templates to improve
;     readability and maintenance.
;--------------------------------------------------------------

;Common Supervisory Code Variables
   bSSC_KEY1:                       equ      F8h   ; supervisory key
   bTEMP_SPACE:                     equ      F8h   ; temporary storage when not used for KEY1
   bSSC_KEYSP:                      equ      F9h   ; supervisory stack ptr key
   bSSC_ResultCode:                 equ      F8h   ; result code

;SSC Return Codes
   bSSC_FAIL:                       equ      01h   ; fail return code
   bSSC_SUCCESS:                    equ      00h   ; success return code

;Flash Erase Operation Template
   bSSC_FLASH_BlockId:              equ      FAh   ; blockId for Erase and Read
   bSSC_FLASH_BlockID_BANK:         equ      FAh   ; block ID page register in bank 1
   bSSC_FLASH_PulseWidth:           equ      FCh   ; Erase pulse width

;Flash Write Operation Template
   bSSC_WRITE_BlockId:              equ      FAh   ; block ID
   bSSC_WRITE_BlockID_BANK:         equ      FAh   ; block ID page register in bank 1
   pSSC_WRITE_FlashBuffer:          equ      FBh   ; pointer to data buffer
   bSSC_WRITE_PulseWidth:           equ      FCh   ; flash write pulse width or ReadCount
   bSSC_WRITE_State:                equ      FDh   ; reserved
   bSSC_WRITE_Delay:                equ      FEh   ; flash macro sequence delay count
   bSSC_WRITE_ResultCode:           equ      FFh   ; temporary result code

;Flash Write Operation Return Code
   bSSC_WRITE_FAIL:                 equ      00h   ; failure
   bSSC_WRITE_SUCCESS:              equ      01h   ; pass

;Flash Sequence Time Delay
   bSSC_WRITE_DELAY:                equ      56h   ; 100us delay at 12MHz

;Flash Read Operation Template
   pSSC_READ_FlashBuffer:           equ      FBh   ; pointer to data buffer
   wSSC_READ_Counter:               equ      FDh   ; byte counter

; Table Lookup Template - NOTE that result is a table overlaying reserved area
   bSSC_TABLE_TableId:              equ      FAh   ; table ID

; Temperature Table Template - returned data after performing a
;     Table lookup of table #7 - Pulse Width Equation data based on Temperature - y= mt+b
;     Composed to two structures - 3 Bytes each - contains slope, y intercept and multiplier
;     Structure 1 is for temperatures from [-40,0]C and structure 2 is for temperaturs from [0,40]C.
   sTEMP_LineEquationBelowZero:     equ      F8h   ; Line Equation data below zero - m,b,Erase->Program multiplier
   sTEMP_LineEquationAboveZero:     equ      FBh   ; Line Equation data above zero - m,b,Erase->Program multiplier
   bTEMP_TempTableRevision:         equ      FFh   ; Table Revision number
   bTEMP_PulseWidthErase:           equ      FEh   ; Temporary storage area after table validation
   bTEMP_PulseWidthProgram:         equ      FFh   ; Temporary storage area after table validation
   ;offsets into each substructure
   cTEMP_SlopeOffset:               equ      0     ; Slope offset in Temp table template
   cTEMP_InterceptOffset:           equ      1     ; Intercept offset in Temp table template
   bTEMP_ProgMultOffset:            equ      2     ; Program multiplier

   VALID_TABLE_REVISION:            equ      1     ; Temp Table revision number


; Flash State mode bits - these bits are used to prevent inadvertent jumping into
; the flash write API.  If the state does not match then a HALT instruction will
; be executed.

   STATE_SET_CLOCK:                 equ      01h
   STATE_CALCULATE_PW:              equ      02h
   STATE_ERASE_BLOCK:               equ      04h
   STATE_WRITE_BLOCK:               equ      08h
   STATE_RESTORE_CLOCK:             equ      10h
   STATE_DONE:                      equ      20h

;--------------------------------------------
;  Restore area to Code area
;--------------------------------------------
Area BootLoaderArea(ROM,REL)
// ----------------------------------------------------------------------------------------
//	Control subroutine which determine whether user program is based on BootLoader Project
//  Return: 0 - if bad program
// 	  	    1 - if all right
// ----------------------------------------------------------------------------------------
Boot_Is_Program_Good:
_Boot_Is_Program_Good:
	push X
	mov A,[__r0]
	push A
	
	mov A,2
	mov X,A0h			; init start control sequence address
	mov [__r0],A		; initiate counter
Start_Control:	
	mov A,0
	romx
	cmp A,[__r0]		; does addr 00A0 contain 0x7D (ljump)
	jnz Error_Program
	inc [__r0]
	inc X
	mov A,X
	cmp A,A6h			; is > than last sequence address?
	jnz Start_Control	; goto next sequence address
	
	pop A	
	mov [__r0],A
	pop X
	mov A,1		; restore register from stack
	ret			; if ok then return 1
Error_Program:
	pop A	
	mov [__r0],A
	pop X		; restore register from stack
	mov A,0		; if bad program then return 0
	ret	

// ---------------------------
// Flash Block CheckSum
// Entrance: A - LSB of block ID
//           X - MSB of block ID 
// Exit: A - CheckSum
FlashCheckSum:
_FlashCheckSum:

	push X
	mov X,SP

	mov A,[__r0]
	push A
	mov A,[__r1]
	push A
	mov A,[__r2]
	push A

    mov A,[X-5]						; A = MSB of BlockID
	mov X,[X-4]						; X = LSB of BlockID

					// calculate address addr = BlockID<<6 = BlockID*64
	mov [__r0],X
    asl [__r0]
    rlc A
    asl [__r0]
    rlc A
    asl [__r0]
    rlc A
	asl [__r0]
    rlc A
    asl [__r0]
    rlc A
    asl [__r0]
    rlc A
    mov X,[__r0]
	
	mov [__r1],A		; Address MSB
	mov [__r0],64		; Cycle Iterator
	mov [__r2],0		; CheckSum
calc_cs:
	dec [__r0]			; decrement Cycle Iterator
	mov A,[__r1]
	romx
	add [__r2],A
	inc X				; increment X to get next address
	or [__r0],0			; if ([__r0]!=0) goto calc_cs;
	jnz calc_cs			; 

	mov A,[__r2]		; return CheckSum  
	
	pop X
	mov [__r2],X
	pop X
	mov [__r1],X
	pop X
	mov [__r0],X

//	mov [bSSC_WRITE_BlockId],A
//	SSC_Action FLASH_CHECKSUM		// CheckSum return in [KEY2][KEY1]
//	mov A,[bSSC_KEY1]		
	pop X
	ret
;-----------------------------------------------------------------------------
;  MACRO NAME: SSC_Action
;
;  DESCRIPTION:
;     Performs locally defined supervisory operations.
;
;     !!! DO NOT CHANGE THIS CODE !!!
;        This sequence of opcodes provides a
;        signature for the debugger and ICE.
;     !!! DO NOT CHANGE THIS CODE !!!
;
;  ARGUMENTS:
;     BYTE  bOperation   - specified supervisory operation - defined operations
;                          are:  FLASH_WRITE, FLASH_ERASE, FLASH_TEMP_TABLE_LOOKUP.
;
;  RETURNS:
;     none.
;
;  SIDE EFFECTS:
;     A and X registers are destroyed
;
;  PROCEDURE:
;     1) specify a 3 byte stack frame.  Save in [KEYSP]
;     2) insert the flash Supervisory key in [KEY1]
;     3) store flash operation function code in A
;     4) call the supervisory code
;-----------------------------------------------------------------------------
macro SSC_Action
      mov   X, SP                         ; copy SP into X
      mov   A, X                          ; mov to A
      add   A, 3                          ; create 3 byte stack frame
      mov   [bSSC_KEYSP], A               ; save stack frame for supervisory code
      mov   [bSSC_KEY1], FLASH_OPER_KEY   ; load the supervisory code for flash operations
      mov   A, @0                         ; load A with specific Flash operation
      SSC                                 ; SSC call the supervisory code
endm

;-----------------------------------------------------------------------------
;  FUNCTION NAME: bFlashWriteBlock
;
;  DESCRIPTION:
;     Writes 64 bytes of data to the flash at the specified blockId.
;
;     Regardless of the size of the buffer, this routine always writes 64
;     bytes of data. If the buffer is less than 64 bytes, then the next
;     64-N bytes of data will be written to fill the rest of flash block data.
;
;  ARGUMENTS:
;     X ->  psBlockWriteData  -  a structure that holds the
;                                calling arguments and some reserved space
;
;  RETURNS:
;     BYTE - successful if NON-Zero returned.
;
;     ASSEMBLER - returned in Accumulator.
;
;  SIDE EFFECTS:
;     1) CPU clock temporarily set to 12MHz.
;
;  PROCEDURE:
;     1) Setup the proper CPU clock - 12 MHz or 6MHz depending on state of SLIMO
;     2) Compute the pulsewidths
;     3) Erase the specified block
;     4) Program the specified block
;     5) restore the original CPU rate
;     6) check the result code and return
;-----------------------------------------------------------------------------
; Place Halt instruction code here to mitigate wondering into this code from the top
   halt
.SECTION
 bFlashWriteBlock:
_bFlashWriteBlock:
   ; Preserve the SMM or LMM paging mode
   RAM_SETPAGE_CUR 0          ; set paging mode

   mov   [bTEMP_SPACE], A     ; temporarily store the MSB of the WriteBlock structure
   mov   A, reg[CPU_F]        ; grab the current CPU flag register and save on stack
   push  A
   mov   A, [bTEMP_SPACE]     ; restore the MSB of the WriteBlock structure

   ; Since the flash SSC operations all reference page 0, save and set the current ptr
   ; to page 0 and the X_ptr to track input data page.
   RAM_PROLOGUE RAM_USE_CLASS_3
   RAM_SETPAGE_IDX A

   ; Set the intial state variable - if code entered from the top - then state
   ; variable will not catch this inadvertent entry.  However, any entry from
   ; made after this statement should be caught!
   mov   [bSSC_WRITE_State], STATE_SET_CLOCK

; Step 1 - setup the proper CPU clock - 12 MHz if SLIMO NOT enabled, else 6MHz
   ; Check the state variable - are we supposed to be here?
   cmp   [bSSC_WRITE_State], STATE_SET_CLOCK
   jnz   bFlashWriteStateError

   ; State - Set Clock
   M8C_SetBank1
   mov   A, reg[OSC_CR0]                  ; Get the System Oscillator control register
   push  A                                ;     and save it on the stack
   and   A, ~OSC_CR0_CPU                  ; Clear the CPU clock selects

   ; Check state of SLIMO
   tst   reg[CPU_SCR1], CPU_SCR1_SLIMO
   jz    .Set12MHz                        ; if Z=0 then SLIMO NOT enabled - set 12MHz

.Set6MHz:
   or    A, OSC_CR0_CPU_24MHz             ; SLIMO enabled - set CPU clock to 6MHz
   jmp   .SetOSC                          ;  IMO clock max is 6MHz - DIVISOR = 1 ==> 24MHz setting
.Set12MHz:
   or    A, OSC_CR0_CPU_12MHz             ; Set CPU clock to 12 MHz
.SetOSC:
   mov   reg[OSC_CR0], A
   M8C_SetBank0

; Step 2 - compute the pulsewidths
ComputePulseWidths:
   asl   [bSSC_WRITE_State]               ; update the state variable
   ; Check the state variable - are we supposed to be here?
   cmp   [bSSC_WRITE_State], STATE_CALCULATE_PW
   jnz   bFlashWriteStateError

   ; State - Calculate PW
   call  bComputePulseWidth
   ; Preset the resturn code to Success
   cmp   A, bSSC_SUCCESS                  ; Check the return value
   jz    EraseBlock                       ;     the pulse width was computed OK
   ; A bad pulse width table was found!
   ; Need to set the result code, restore the clock, and then exit!
   mov   [bSSC_WRITE_ResultCode], bSSC_WRITE_FAIL
   mov   [bSSC_WRITE_State], STATE_RESTORE_CLOCK
   jmp   RestoreClock

; Step 3 - Erase the specified flash block
EraseBlock:
   asl   [bSSC_WRITE_State]               ; update the state variable
   ; State - Erase Block

   mov   A, [X+wARG_BlockId+1]            ; set block ID to be 128 blocks by N Banks
   rlc   A
   mov   A, [X+wARG_BlockId]
   rlc   A
   M8C_SetBank1
   mov   reg[bSSC_FLASH_BlockID_BANK], A  ; set the bank of the blockID
   M8C_SetBank0
   push  A
   mov   A, [X+wARG_BlockId+1]
   and   A, 0x7F
   mov   [bSSC_WRITE_BlockId], A          ; set the block-within-Bank-ID
   push  A

   mov   A, [X+bDATA_PWErase]             ; set the pulse width
   mov   [bSSC_WRITE_PulseWidth], A
   mov   [bSSC_WRITE_Delay], bSSC_WRITE_DELAY   ; load the sequence delay count
   ; Check the state variable - are we supposed to be here?
   mov   A, [bSSC_WRITE_State]
   cmp   A, STATE_ERASE_BLOCK
   jnz   bFlashWriteStateError
   push  A                                ; save the State variable
   push  X

.Literal
_FlashBlockLocal1::
   SSC_Action FLASH_ERASE                 ; Erase the specified block
.EndLiteral

   pop   X
   pop   A                                ; restore the State variable
   mov   [bSSC_WRITE_State], A
   ; Check the return code for success
   cmp   [bSSC_ResultCode], bSSC_SUCCESS
   jz    WriteBlock
   ; Erase operation failed!
   ; Need to set the result code, restore the clock, and then exit!
   mov   [bSSC_WRITE_ResultCode], bSSC_WRITE_FAIL
   mov   [bSSC_WRITE_State], STATE_RESTORE_CLOCK
   add   SP, -2                           ; retire the bank and block #s
   jmp   RestoreClock

; Step 4 - Program the flash
WriteBlock:
   ; Update the state variable
   asl   [bSSC_WRITE_State]
   ; State - Write Block

   pop   A                                ; load WRITE opeation parameters
   mov   [bSSC_WRITE_BlockId], A          ; set the LSB of the blockID
   pop   A
   M8C_SetBank1
   mov   reg[bSSC_FLASH_BlockID_BANK], A  ; set the bank of the blockID
   M8C_SetBank0

   mov   A, [X+pARG_FlashBuffer+1]        ; set the LSB of the RAM buffer ptr
   mov   [pSSC_WRITE_FlashBuffer], A
   mov   A, [X+pARG_FlashBuffer]          ; set the MSB of the RAM buffer ptr
   mov   reg[MVR_PP], A


   mov   A, [X+bDATA_PWProgram]
   mov   [bSSC_WRITE_PulseWidth], A
   mov   [bSSC_WRITE_Delay], bSSC_WRITE_DELAY   ; load the sequence delay count
   ; Check the state variable - are we supposed to be here?
   mov   A, [bSSC_WRITE_State]
   cmp   A, STATE_WRITE_BLOCK
   jnz   bFlashWriteStateError
   push  A                                ; save the State variable
   push  X

.Literal
_FlashBlockLocal2::
   SSC_Action FLASH_WRITE                 ; Program the flash
.EndLiteral

   pop   X
   pop   A                                ; restore the state variable
   mov   [bSSC_WRITE_State], A
   ; Set the return code
   mov   [bSSC_WRITE_ResultCode], bSSC_WRITE_SUCCESS
   cmp   [bSSC_ResultCode], bSSC_SUCCESS
   jz    NextState
   ; Write operation failed!
   ; Need to set the result code, restore the clock, and then exit!
   mov   [bSSC_WRITE_ResultCode], bSSC_WRITE_FAIL

; Step 5 - restore the original CPU rate
NextState:
   asl   [bSSC_WRITE_State]               ; update the state variable
   ; Check the state variable - are we supposed to be here?
RestoreClock:
   cmp   [bSSC_WRITE_State], STATE_RESTORE_CLOCK
   jnz   bFlashWriteStateError

   ; State - Restore Clock
   pop   A
   M8C_SetBank1
   mov   reg[OSC_CR0], A                  ; Restore org CPU rate
   M8C_SetBank0                           ; Switch back to Bank 0
   asl   [bSSC_WRITE_State]               ; update the state variable

; Step 6 - Compute the return result code
   mov   A, [bSSC_WRITE_ResultCode]

bFlashWriteBlockEnd:
; check the state variable for proper exit -
   cmp   [bSSC_WRITE_State], STATE_DONE
   jz    bFlashWriteExit

; if we arrived here, it means that the flashWrite API was randomly entered!!!
bFlashWriteStateError:
   halt
   jmp   bFlashWriteStateError

bFlashWriteExit:
   RAM_EPILOGUE RAM_USE_CLASS_3

; return with a RETI to preserve the last paging mode - SMM or LMM
   reti



; Put halt here in case we jump inadvertently
   halt
   halt
   halt

;-----------------------------------------------------------------------------
;  FUNCTION NAME:    ComputePulseWidth
;
;  DESCRIPTION:
;     Computes the Block Erase and Block Program pulse width counts for the
;     Flash Erase and Flash Program supervisory calls.
;
;     This routine gets its data from the FlashWriteBlock data structure
;     and saves the return data in the same structure.
;
;     First, the Temperature data table is accessed via the Table Read SSC
;     function.  Then the Erase  and Program pulse width counts are computed.
;
;     Temperature table gives the slope, Y intercept, and Erase to Program pulse
;     width converion.  Two equations are given - temperatures below 0 and
;     temperatures above 0. Data is scaled to fit within specified byte range.
;
;        PW(erase) = B - M*T*2/256 and PW(program)= PW(erase)*Multiplier/64
;
;     ADJUSTMENT FOR SLIMO:
;     --------------------
;     After calculation of both the PW(erase) and PW(Program), the SLIMO bit
;     is detected. If the SLIMO bit is enabled then both programming pulses are
;     divided by TWO and incremented by one for roundoff.  This is due to the fact
;     that the CPU clock will be set for 6MHz instead of 12MHz which means that the
;     SSC EraseBlk and WriteBlk operation will take twice as long.
;
;  ARGUMENTS:
;     X points to bFlashWriteBlock calling structure.
;
;  RETURNS:
;     BYTE  bResult - return in Accumulator
;           0 = valid
;           1 = invalid revision
;
;     Erase and Program pulse widths are returned in bFlashWriteBlock calling
;     structure.
;
;  SIDE EFFECTS:
;     none.
;
;  REQUIREMENTS:
;
;     1) The calculated erase pulse width is always < 128 (does not overflow 7 bits)
;     2) The calculated write pulse width is always < 256 (does not overflow 8 bits)
;     3) If SLIMO is enabled, then this algorithm assumes the CPU clock will be set
;        for 6MHz and NOT 12MHz!
;
;     These requirements MUST be guaranteed during device calibration.
;     They are not checked.  If they are not met, the pulse width calculation will fail.
;
;  PROCEDURE:
;     1) Get the flash programming temperature table
;     2) Check the table revision number
;     3) Select the correct data set, based on temperature
;     4) Compute the Erase Pulsewidth count
;     5) Compute the Program Pulsewidth count
;     6) Save the result data
;     7) Adjust for SLIMO
;
;-----------------------------------------------------------------------------
bComputePulseWidth:

   ; 1) Get the flash programming temperature table
   mov   [bSSC_TABLE_TableId], TEMPERATURE_TABLE_ID
   RAM_SETPAGE_MVW 0                      ; set table WRITE page to 0
   M8C_SetBank1
   mov   reg[bSSC_FLASH_BlockID_BANK], 0  ; set the SSC operation page to 0
   M8C_SetBank0

   ; Check the State
   mov   A, [bSSC_WRITE_State]
   cmp   A, STATE_CALCULATE_PW
   jnz   bFlashWriteStateError
   push  A                                ; save the State variable
   push  X

.Literal
_FlashBlockLocal3::
   SSC_Action  FLASH_TEMP_TABLE_LOOKUP
.EndLiteral

   pop   X

   ; 2) Check table revision
   cmp   [bTEMP_TempTableRevision], VALID_TABLE_REVISION
   jnz   ComputePulseWidthTableError      ; jump if revision is out of date!

   ; 3) Select the correct data set, based on temperature
   push  X                                ; save the X pointer
   REG_PRESERVE  IDX_PP                   ; save the X pointer page

   mov   A, [X+cARG_Temperature]          ; load temperature into the MULTIPLIER
   mov   REG[MUL_X], A
   and   A, 80h                           ; Test for sign of temperature
   jnz   BelowZero

AboveZero:                                ; Positive temperature
   mov   A, [sTEMP_LineEquationAboveZero + bTEMP_ProgMultOffset]
   mov   [X+bDATA_PWMultiplier], A        ; Save the multiplier for later use
   mov   X, sTEMP_LineEquationAboveZero   ; X pts to Temp table above zero
   jmp   ComputeData

BelowZero:                                ; Negative temperature
   mov   A, [sTEMP_LineEquationBelowZero + bTEMP_ProgMultOffset]
   mov   [X+bDATA_PWMultiplier], A        ; Save the multiplier for later use
   mov   X, sTEMP_LineEquationBelowZero   ; X pts to Temp table data below zero

   ; 4) Compute the Erase PulseWidth count => PW(e) = B - M*T*2/256
ComputeData:
   RAM_SETPAGE_IDX  0                     ; set the X pointer page to pt to zero page
                                          ; where the temp table data is located.
   mov   A, [X+cTEMP_SlopeOffset]         ; compute M*T
   mov   REG[MUL_Y], A
   mov   A, REG[MUL_DL]                   ; compute M*T*2
   asl   A
   mov   A, REG[MUL_DH]
   rlc   A                                ; A = M*T*2/256
   cpl   A                                ; 2's complement the data - complement and then increment
   inc   A                                ; A = -(M*T*2/256)
   add   A, [X+cTEMP_InterceptOffset]     ; Add it to B to compute PW(e) => B - (M*T*2/256) => ERASE PulseWidth
   mov   [bTEMP_PulseWidthErase], A       ; Save the Erase Pulse width in temp area

   ; 5) Compute the Program PulseWidth      PW(program) = PW(erase) * ProgramMultiplier / 64
   and   A, 7Fh                           ; Mac is signed - Erase pulse width MUST always be < 128
   mov   REG[MUL_X], A                    ; compute PW(e) * ProgMult
   mov   A, [X+bTEMP_ProgMultOffset]      ; Mac is signed - First multiply by high 7 bits of ProgMult
   asr   A                                ; shift high 7 bits down to low 7 bits (divide by 2)
   and   A, 0x7f                          ; zero out high bit to make it an unsigned divide by 2
   mov   REG[MUL_Y], A                    ; Do the 7 bit x 7 bit hardware multiply
.mult7x7done:
   mov   A, REG[MUL_DH]                   ; Load 16 bit result into (PulseWidthProg, A)
   mov   [bTEMP_PulseWidthProgram], A
   mov   A, REG[MUL_DL]
   asl   A                                ; Shift left to compensate for divide by 2 above
   rlc   [bTEMP_PulseWidthProgram]
.shift7x7done:
   tst   [X+bTEMP_ProgMultOffset], 0x01   ; If low bit of ProgMult was set (lost during divide by 2),
   jz    .mult7x8done                     ; add 1 * PulseWidthErase to product
   add   A, [bTEMP_PulseWidthErase]
   adc   [bTEMP_PulseWidthProgram], 0
.mult7x8done:                             ; PW(e) * ProgMult is in (PulseWidthProg, A)
   asl   A                                ; shift left twice to get
   rlc   [bTEMP_PulseWidthProgram]        ; 4 * PW(e) * ProgMult in (PulseWidthProg, A)
   asl   A                                ; or 4*PW(e)*ProgMult/256 == PW(e)*ProgMult/64 in PulseWidthProg
   rlc   [bTEMP_PulseWidthProgram]        ; The product MUST be < 2**14 for this to work.
                                          ; PW(p) = PW(e) * ProgMult / 64
   ; 6) Save the result data
SaveResultData:
   REG_RESTORE IDX_PP                     ; restore the XPP to point to the PW Data
   mov   A, [bTEMP_PulseWidthProgram]
   pop   X                                ; restore the X pointer
   mov   [X+bDATA_PWProgram], A           ; Save Program pulse width in BlockWrite calling frame
   mov   A, [bTEMP_PulseWidthErase]       ; Save Erase pulse width in BlockWrite calling frame
   mov   [X+bDATA_PWErase], A

   ; 7) Adjust the Pulse Width for SLIMO setting
AdjustForSLIMO:
   tst   reg[CPU_SCR1], CPU_SCR1_SLIMO    ; Check state of SLIMO
   jz    ComputePulseWidthEnd             ; if Z=0 then SLIMO NOT enabled - do nothing

   ;SLIMO is enabled - divide the pulsewidths by two and add one for round-off error
   asr   [X+bDATA_PWProgram]
   inc   [X+bDATA_PWProgram]
   asr   [X+bDATA_PWErase]
   inc   [X+bDATA_PWErase]

ComputePulseWidthEnd:                     ; NORMAL Termination
   pop   A                                ;  restore the STATE
   cmp   A, STATE_CALCULATE_PW            ;  make sure we are supposed to be here
   jnz   bFlashWriteStateError
   mov   [bSSC_WRITE_State], A            ;  restore the state variable
   mov   A, bSSC_SUCCESS                  ; load return value with success!
   ret

ComputePulseWidthTableError:              ; TABLE Error Termination
   pop   A                                ;  restore the STATE
   cmp   A, STATE_CALCULATE_PW            ;  make sure we are supposed to be here
   jnz   bFlashWriteStateError
   mov   [bSSC_WRITE_State], A            ;  restore the state variable
   mov   A, bSSC_FAIL                     ; load return value with Failure!
   ret

.ENDSECTION

;------------------------------------------------------------------------------
;   FUNCTION NAME: FlashReadBlock
;
;   DESCRIPTION:
;   Reads a specified flash block to a buffer in RAM.
;
;   ARGUMENTS:
;   A,X -> FLASH_READ_STRUCT
;
;   RETURNS:
;   Data read is returned at specified pFlashBuffer.
;
;   SIDE EFFECTS:
;   Uses SSC storage at FBh and FDh
;
;   PROCEDURE:
;   BlockID is converted to absolute address and then ROMX command is used
;   to read the data from flash into the specified buffer.
;
;------------------------------------------------------------------------------
.SECTION
 FlashReadBlock:
_FlashReadBlock:

   ; Preserve the SMM or LMM paging mode
   RAM_SETPAGE_CUR  0
   mov   [bTEMP_SPACE], A     ; temporarily store the MSB of the WriteBlock structure
   mov   A, reg[CPU_F]        ; grab the current CPU flag register and save on stack
   push  A
   mov   A, [bTEMP_SPACE]     ; restore the MSB of the WriteBlock structure

   ; Since the SSC operations all reference page 0, save and set the current ptr
   ; to page 0 and the X_ptr to track the stack page.
   RAM_PROLOGUE RAM_USE_CLASS_3
   RAM_SETPAGE_IDX  A

   ; mov some args to SSC storage

   mov   A, [X+pARG_FlashBuffer+1]    ; get pointer - LSB
   mov   [pSSC_READ_FlashBuffer],A    ; use SSC storage area
   mov   A, [X+pARG_FlashBuffer]      ; get pointer - MSB
   RAM_SETPAGE_MVW A                  ; set the MSB in the MVI Write Pointer
   mov   A, [X+wARG_ReadCount]        ; get count
   inc   A                            ; bump by one to account for testing
   mov   [wSSC_READ_Counter],A        ; use SSC storage area
   mov   A, [X+wARG_ReadCount+1]
   mov   [wSSC_READ_Counter+1],A

   ; Compute the absolute address of the flash block

   mov   A, [X+wARG_BlockId+1]        ; compute the LSB = wBlockId * 64
   asl   A
   rlc   [X+wARG_BlockId]
   asl   A
   rlc   [X+wARG_BlockId]
   asl   A
   rlc   [X+wARG_BlockId]
   asl   A
   rlc   [X+wARG_BlockId]
   asl   A
   rlc   [X+wARG_BlockId]
   asl   A
   rlc   [X+wARG_BlockId]

   push  A                            ; save LSB
   mov   A, [X+wARG_BlockId]          ; mov MSB into A
   pop   X                            ; put LSB into X

   ; Read the Flash
ReadFlash:
   push	 A                            ; save MSB
   romx                               ; Read the flash
   mvi   [pSSC_READ_FlashBuffer], A   ; store the data in the RAM buffer
   pop   A                            ; restore MSB
   inc   X                            ; increment the LSB of the flash addr
   jnz   TestCounter
   inc   A
TestCounter:
   dec   [wSSC_READ_Counter+1]        ; decrement the byte counter
   jnz   TestPageBoundary
   dec   [wSSC_READ_Counter]
   jz    ReadFlashDone                ; if counter is zero - done!

TestPageBoundary:                     ; Test Buffer pointer to see if the page ptr
   cmp   [pSSC_READ_FlashBuffer],0x00 ; has wrapped
   jnz   ReadFlash
   push  A
   mov   A, reg[MVW_PP]
   inc   A
   mov   reg[MVW_PP], A
   pop   A
   jmp   ReadFlash

   ; Done reading the flash
ReadFlashDone:
   RAM_EPILOGUE RAM_USE_CLASS_3

   ;return using RETI to be sure the SMM or LMM paging mode is restored
   reti

.ENDSECTION
;---------------------
;  End of File
;---------------------
