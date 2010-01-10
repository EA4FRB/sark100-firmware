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
// 	FILE NAME: 	BOOTLOADER.C
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
#include <m8c.h>        // part specific constants and macros
#include "bootloader.h"    // API definitions for all BootLoader Modules
#include <FlashBlock.h>

#define LAST_BLOCK_TO_CHECK	0		// Minimum = 1; Maximum = 455; if 0 then don't calculate CS
#define ENABLE_CONNECT_BY_PSOC	1	// 0-disable 	1-enable

#pragma text:BootCheckSum
#pragma abs_address:0x7200
const char BootCheckSumBlock[];
#pragma abs_address:0x73FE
const int LastBlock_To_Check = LAST_BLOCK_TO_CHECK;
#pragma end_abs_address

#pragma text:BootLoaderArea

//#define SETBUTTON()	PRT0DR|=0x20	// pull up for button on P0[5] pin
//#define GETBUTTON() (PRT0DR&0x20)		// get value on button pin
//#define BOOTLOADER_MODE_LED_ON() PRT1DR|=0x80
//#define BOOTLOADER_MODE_LED_OFF() PRT1DR&=0x7F

//extern void Boot_LoadConfigInit(void);
extern char FlashCheckSum(int);
extern char Boot_Is_Program_Good(void);

char Boot_ASCIItoBYTE(char Low,char High){
    char byte;
	if (High>='a') byte=(High-'a'+10)<<4; else byte=(High-'0')<<4;
	if (Low>='a') byte|=(Low-'a'+10); else byte|=(Low-'0');
	return byte;
}

BYTE Boot_UART_cGetChar(void){
	BYTE bRxStatus;
	while (!(bRxStatus=Boot_RxD_bReadRxStatus() & RX8_RX_COMPLETE));
    return Boot_RxD_bReadRxData();
}

void Boot_UART_PutChar(char TxData){
	while (!(Boot_TxD_bReadTxStatus() & TX8_TX_BUFFER_EMPTY));
	Boot_TxD_SendData(TxData);
}

void Boot_UART_CPutString(char* pstr){
	while (*pstr != 0){
	   Boot_UART_PutChar(*pstr);
	   pstr++;
	}
}

void Boot_PerformWrite(){
	char data[68];
	char i,ch,cl;
	int BlockID,j;
	FLASH_WRITE_STRUCT fwStruct;
	FLASH_READ_STRUCT frStruct;
	char led=0x00;	//LED off

    while(1){
//       if (led) BOOTLOADER_MODE_LED_OFF(); else BOOTLOADER_MODE_LED_ON();
       led=~led;
       while ((ch=Boot_UART_cGetChar())!='S'); // waiting for start symbol 'S'
	   if ((ch = Boot_UART_cGetChar())=='S') break; // All blocks are sent ?
	      else {
		    cl=Boot_UART_cGetChar();
	        data[0]=Boot_ASCIItoBYTE(cl,ch);
	      }
	   for (i=1; i<68; i++) {
	      ch=Boot_UART_cGetChar();  // Read high nibble
	      cl=Boot_UART_cGetChar();  // Read low nibble
	      data[i]=Boot_ASCIItoBYTE(cl,ch);  // Write transformed byte in buffer
	   }

	   // Read Last Block Symbol 'F'
	   if ((ch=Boot_UART_cGetChar())!='F') ; // Error Info!

	   // Write Block to Flash
	   fwStruct.wARG_BlockId = (((int)data[1])<<2) + (data[2]>>6); // Form Block ID form Block Address
	   fwStruct.pARG_FlashBuffer = data+4;	// Data start Address
	   fwStruct.cARG_Temperature = 25;		// Temperature in Celsius
	   bFlashWriteBlock(&fwStruct);			// Write Block

	   // Read back Block
	   frStruct.wARG_BlockId = fwStruct.wARG_BlockId;	// Read back the same block
	   frStruct.pARG_FlashBuffer = data+4;  // Buffer Address
	   frStruct.wARG_ReadCount = 64;		// Read 64 bytes
	   FlashReadBlock(&frStruct);			// Read Block

	   // Sending Back Written Block for Analyzing
	   Boot_UART_PutChar('S');	   // Start frame Symbol
	   for (i=0;i<68;i++){
	      ch = data[i]>>4;
	      if (ch>=0x0A) ch+=('a'-10); else ch+='0';
	      Boot_UART_PutChar(ch);		// Send High Nibble
	      cl = data[i]&0x0F;
	      if (cl>=0x0A) cl+=('a'-10); else cl+='0';
	      Boot_UART_PutChar(cl);		// Send Low Nibble
	   }
	   Boot_UART_PutChar('F');	   // Last frame Symbol
	}

	if (LastBlock_To_Check==0) return;	// Don't calculate a checksum
	BlockID = 456;	// first block where checksum is saved (blocks of CheckSum Area:456,457,458,459,460,461,462,463)
	// Calculate CheckSum and Write It In Flash
	for (j=1;j<=LastBlock_To_Check;j++){
		if ((j&0x3F)==0) {
 		  fwStruct.wARG_BlockId = BlockID;       // Block ID
	      fwStruct.pARG_FlashBuffer = data;  	// Data start Address
	      fwStruct.cARG_Temperature = 25;		// Temperature in Celsius
	      bFlashWriteBlock(&fwStruct);			// Write Block
 		  BlockID++;							// Increment Block ID
		}
		data[j&0x3F] = FlashCheckSum(j);		// data[i%64] <- CheckSum of i-th block
    }

    fwStruct.wARG_BlockId = BlockID;         // Block ID
    fwStruct.pARG_FlashBuffer = data;  	// Data start Address
    fwStruct.cARG_Temperature = 25;		// Temperature in Celsius

    if (BlockID!=463){
       bFlashWriteBlock(&fwStruct);		// Write Block
       BlockID=463;
    };

    data[62]=(char)(LastBlock_To_Check>>8);	// Save last block to check (MSB)
    data[63]=(char)LastBlock_To_Check;		// Save last block to check (LSB)

    fwStruct.wARG_BlockId = BlockID;         // Block ID
    fwStruct.pARG_FlashBuffer = data;  	// Data start Address
    fwStruct.cARG_Temperature = 25;		// Temperature in Celsius
    bFlashWriteBlock(&fwStruct);		// Write Block
}

void BootLoader(){
//    long int x;
//    char z;
	int j;
	char buffer[10];
	char Error,i,k;
	char CheckSum;
    char strConnect[8];//={'C','O','N','N','E','C','T','\0'};
    char strAnswer[4];//={'O','K','!','\0'};
	// Initiate communications Strings:
    // strConnect[]="CONNECT", // Initiate string form PC
    // strAnswer[]="OK!";      // Answer to PC
	strConnect[0]='C';strConnect[1]='O';strConnect[2]='N';strConnect[3]='N';strConnect[4]='E';
    strConnect[5]='C';strConnect[6]='T';strConnect[7]=0;
	strAnswer[0]='O';strAnswer[1]='K';strAnswer[2]='!';strAnswer[3]=0;

 	// Initiate UART Module
	Boot_RxD_Start(RX8_PARITY_NONE);
	Boot_TxD_Start(TX8_PARITY_NONE);

#if ENABLE_CONNECT_BY_PSOC
	// Check conditions for remote programming only through UART
	// if was Power On Reset then check this
	//
	if (CPU_SCR0 & CPU_SCR0_PORS_MASK){
      	Boot_UART_CPutString(strConnect);	// Send Connect Message to PC
      	Boot_Counter_DisableInt();	// Disable Counter Interrupts
      	Boot_Counter_WritePeriod(8191);	// a half second timeout (Counter Clock=32kHz)
      	Boot_Counter_WriteCompareValue(4094);// compare value
      	Boot_Counter_Start();		// Start Counter
      	k=0;
      	while (Boot_Counter_wReadCounter()!=0){	// Timeout - a half second
	       if (Boot_RxD_bReadRxStatus() & RX8_RX_COMPLETE){ // if character was got by RxD
	           buffer[k]=Boot_RxD_bReadRxData();		// then read this character
	           if (buffer[k]==strAnswer[k]) k++;	// if match then go on
	             else k=0;							// Start receiving symbol from the beginning
	       }
	       if (k==3) {
	         Boot_UART_CPutString(strAnswer);	// send back strAnswer to establish connection
      	     Boot_PerformWrite();			// reprogram Flash
    	     M8C_Reset;						// Perform Software Reset by Supervisory Call
	       }
      	};
      	Boot_Counter_Stop();				// Stop Counter
	}
#endif

	// Control if a BoootLoader Program is correct
	Error=0;
	if (!Boot_Is_Program_Good()) Error=1;
	// Control CheckSum if needed
	if (LastBlock_To_Check!=0) {	// Don't calculate checksum if 0
      for (j=1;j<=LastBlock_To_Check;j++){
         CheckSum=BootCheckSumBlock[j];
         if (CheckSum!=FlashCheckSum(j)) Error=1;	// Set CheckSum Error Flag
      }
    }
    // Check button to enter bootloader mode
//    SETBUTTON();		// pull up for Button
//    if (!GETBUTTON())
      if (!Error) asm("ljmp __Start");	// if button not pressed - Start Firmwire

    // Here it communicates with PC and then starts programming
//	BOOTLOADER_MODE_LED_ON();		// light on LED when entered BootLoader Mode

 	// Connect with PC Terminal Program
	Error=1;
	// Set Communication here
	while (Error){
	 	Error=0;
	 	for (i=0;i<7; i++) {
	 	  buffer[i]=Boot_UART_cGetChar();
	 	  if (buffer[i]!=*(strConnect+i)) {
	 	       Error=1;
	 	       break;
	 	  }
	 	}
	 	if (Error) continue;

	    // Send Back Annswer - "Ok!"
      	Boot_UART_CPutString(strAnswer);
    	// Get Answer back -"Ok!"
    	for (i=0;i<3; i++) {
	 	  buffer[i]=Boot_UART_cGetChar();
	 	  if (buffer[i]!=*(strAnswer+i)) {
	 	       Error=1;
	 	       break;
	 	  }
	 	}
	}

	// Waiting for blocks to be written
	Boot_PerformWrite();

	M8C_Reset;		// Perform Software Reset by Supervisory Call
}