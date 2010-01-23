;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME:   E2PROM.asm
;;  Version: 1.7, Updated on 2009/6/23 at 19:32:46
;;  Generated by PSoC Designer 5.0.972.0
;;
;;  DESCRIPTION: EEPROM emulation User Module implementation file
;;     for CY8C22/24/26/27/29xxx and related devices.
;;
;;     Routines in this file perform E2PROM User Module Read and
;;     Write operations.
;;
;;     They provide a layer of abstraction on top of the E2PROMLIB.asm routines
;;     to allow instancing of the EEPROM devices.  Together the E2PROM and
;;     E2PROMLIB algorithms emulate a byte-wise EEPROM device on a block oriented
;;     flash architecture.
;;
;;  NOTE: User Module APIs conform to the fastcall16 convention for marshalling
;;        arguments and observe the associated "Registers are volatile" policy.
;;        This means it is the caller's responsibility to preserve any values
;;        in the X and A registers that are still needed after the API functions
;;        returns. For Large Memory Model devices it is also the caller's 
;;        responsibility to perserve any value in the CUR_PP, IDX_PP, MVR_PP and 
;;        MVW_PP registers. Even though some of these registers may not be modified
;;        now, there is no guarantee that will remain the case in future releases.
;;-----------------------------------------------------------------------------
;;  Copyright (c) Cypress MicroSystems 2001-2004. All Rights Reserved.
;;*****************************************************************************
;;*****************************************************************************

;Preset CPU_EQUATE for CY8C25/26xxx identification
CPU_F:   equ   0

include "m8c.inc"
include "E2PROM.inc"
include "memory.inc"

;-------------------------------------------------------------------
;  Declare the functions global for both assembler and C compiler.
;
;  Note that there are two names for each API. First name is
;  assembler reference. Name with underscore is name refence for
;  C compiler.  Calling function in C source code does not require
;  the underscore.
;-------------------------------------------------------------------

;-----------------------------------------------
;  Global Symbols
;-----------------------------------------------

export   E2PROM_Start
export  _E2PROM_Start
export   E2PROM_Stop
export  _E2PROM_Stop
export   E2PROM_bE2Write
export  _E2PROM_bE2Write
export   E2PROM_E2Read
export  _E2PROM_E2Read

export   E2PROM_RESERVED

;-----------------------------------------------------------------
;  Allocate the E2PROM EEPROM device in Flash memory
;     This will allow the linker to perform memory collision
;     checking and the EEPROM device will be displayed in the
;     map file.
;     Note that this memory region can also be accessed from "C".
;-----------------------------------------------------------------
AREA  E2PROM_AREA   (ROM,ABS,CON)
   ORG   E2PROM_START_ADDR

_E2PROM_RESERVED::
 E2PROM_RESERVED::     blkb     180h


AREA  UserModules (ROM,REL)

;-----------------------------------------------------------------------------
;  FUNCTION NAME: E2PROM_Start
;
;  DESCRIPTION:
;     Start routine for the E2PROM user module.  This routine is
;     a place holder for conformance and consistency.
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;     None
;
;  RETURNS:
;     None
;
;  SIDE EFFECTS: 
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
.SECTION

 E2PROM_Start:
_E2PROM_Start:
   RAM_PROLOGUE RAM_USE_CLASS_1
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret

.ENDSECTION


;-----------------------------------------------------------------------------
;  FUNCTION NAME: E2PROM_Stop
;
;  DESCRIPTION:
;     Stop routine for the E2PROM user module.  This routine is
;     a place holder for conformance and consistency.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;     None
;
;  RETURNS:
;     None
;
;  SIDE EFFECTS: 
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;
.SECTION

 E2PROM_Stop:
_E2PROM_Stop:
   RAM_PROLOGUE RAM_USE_CLASS_1
   RAM_EPILOGUE RAM_USE_CLASS_1
   ret

.ENDSECTION


;-----------------------------------------------------------------------------
;  FUNCTION NAME: E2PROM_E2Write
;
;  DESCRIPTION:
;     Writes the specified E2PROM data at the wAddr and wByteCount from RAM into
;     Flash into the defined E2PROM instance.
;
;     Prototype in C is:
;
;        #pragma  fastcall16 E2Write
;        void E2PROM_bE2Write( WORD wAddr, BYTE * pbData, WORD wByteCount,
;                                        CHAR cTemperature );
;
;     Easiest method to call from assembly is as follows:
;           push  X                    ; push X only if X needs to be perserved
;           mov   X, SP
;           push  <cTemperature>
;           push  <wByteCount>
;           push  <wByteCount+1>
;           push  <pbDataDest>                  
;           push  <pbDataDest+1>
;           push  <wAddr>
;           push  <wAddr+1>
;           call  E2PROM_bE2Write
;           add   SP, -E2_RD_ARG_STACK_FRAME_SIZE
;           pop   X                    ; restore X only if it was saved above
;
;           where <> refers to any addressing mode or number of instructions to
;              place the referenced data on the stack frame.
;
;     Other method is to create a stack frame using the defined equates and
;     load them into the stack frame using [X+E2_WR_ARG_*] offset addressing mode.
;     If this method is used in the LMM (large memory model) then make sure
;     that the index page tracks the stack page or is set to the same page
;     as the stack page.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;     wAddr:         WORD   - relative OFFSET in defined E2PROM to write data
;     pbData:        BYTE * - pointer to the RAM buffer of data to write
;     wByteCount:    WORD   - number of bytes to write into E2PROM
;     cTemperature:  CHAR   - temperature in degrees celsius
;
;  RETURNS:    E2PROM_NOERROR, E2PROM_FAILURE, or
;              E2PROM_STACKOVERFLOW
;
;  SIDE EFFECTS: 
;    If a partial block is to be saved to flash, then a 64 byte buffer
;    is temporary allocated.
;
;    The A and X registers may be modified by this or future implementations
;    of this function.  The same is true for all RAM page pointer registers in
;    the Large Memory Model.  When necessary, it is the calling function's
;    responsibility to perserve their values across calls to fastcall16 
;    functions.
;          
;    Currently only the page pointer registers listed below are modified:
;       CUR_PP 
;       IDX_PP
;       MVW_PP
;                
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;------------------------------------------------------------------------------
.SECTION

 E2PROM_bE2Write::
_E2PROM_bE2Write::

;  RAM_PROLOGUE RAM_PROXY_CLASS_4 RAM_USE_CLASS_2  -->  implemented in E2PROMLIB  
;  Save the Flag register to perserve the native paging mode
IF (CPU_F)     ;only save CPU flag if NOT CY8C25/26xxx PSoC devices
   mov   A, reg[CPU_F]                            
   push  A
ENDIF
   mov   A, >E2PROM_START_BLOCK          ; push the FirstBlockID - MSB
   push  A
   mov   A, <E2PROM_START_BLOCK          ; push the FirstBlockID - LSB
   push  A
   ljmp  bE2Write                                  ; jmp to E2PROMLIB 


; NOTE that return will be done by a RETI instruction to restore the orginal paging
;      mode and flag status.

.ENDSECTION


;-----------------------------------------------------------------------------
;  FUNCTION NAME: E2PROM_E2Read
;
;  DESCRIPTION:
;     Reads the specified E2PROM data at offset=wAddr for wByteCount bytes  and
;     places the data read into the RAM buffer pbDataDest.
;
;     Prototype in C is:
;
;        #pragma  fastcall16 E2Read
;        void E2PROM_E2Read( WORD wAddr, BYTE * pbDataDest, WORD wByteCount );
;
;     Easiest method to call from assembly is as follows:
;           push  X                             ; only if X needs to be preserved
;           mov   X, SP
;           push  <wByteCount>
;           push  <wByteCount+1>
;           push  <pbDataDest
;           push  <pbDataDest+1>
;           push  <wAddr>
;           push  <wAddr+1>
;           call  E2PROM_E2Read
;           add   SP, -E2_RD_ARG_STACK_FRAME_SIZE
;           pop   X                             ; restore only if X needs to be preserved
;
;           where <> refers to any addressing mode or number of instructions to
;              place the referenced data on the stack frame.
;
;     Other method is to create a stack frame using the defined equates and
;     load them into the stack frame using [X+E2_WR_ARG_*] offset addressing mode.
;     If this method is used in the LMM (large memory model) then make sure
;     that the index page tracks the stack page or is set to the same page
;     as the stack page.
;
;-----------------------------------------------------------------------------
;
;  ARGUMENTS:
;     wAddr:         WORD   - relative OFFSET in defined E2PROM to read data
;     pbDataDest:    BYTE * - pointer to the RAM buffer to place read data
;     wByteCount:    WORD   - number of bytes to read from E2PROM
;
;  RETURNS:       none
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED 
;    AS MAY THE PAGE POINTER REGISTERS! 
;    
;    In the large memory model currently only the page
;    pointer registers listed below are modified.  This does
;    not guarantee that in future implementations of this
;    function other page pointer registers will not be
;    modified.
;    
;    Page Pointer Registers Modified:
;          CUR_PP
;          MVW_PP 
;
;-----------------------------------------------------------------------------
.SECTION

 E2PROM_E2Read::
_E2PROM_E2Read::
;  RAM_PROLOGUE RAM_PROXY_CLASS_NULL RAM_USE_CLASS_4  -->  implemented in E2PROMLIB  
;  Save the Flag register to perserve the native paging mode
IF (CPU_F)     ;only save CPU flag if NOT CY8C25/26xxx PSoC devices
   mov   A, reg[CPU_F]                            
   push  A
ENDIF

;  Load the Block ID into X and A
   mov   A, >E2PROM_START_BLOCK       
   mov   X, A
   mov   A, <E2PROM_START_BLOCK
   ljmp  E2Read

; NOTE that return will be done by a RETI instruction to restore the orginal paging
;      mode and flag status.


.ENDSECTION

; End of File
