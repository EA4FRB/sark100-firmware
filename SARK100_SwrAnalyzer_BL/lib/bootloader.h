#include <m8c.h>

#define Boot_RxD_RXBUF_ENABLE 0

//-------------------------------------------------
// Prototypes of the Boot_RxD API.
//-------------------------------------------------

#if ( Boot_RxD_RXBUF_ENABLE )
extern char Boot_RxD_aRxBuffer[];
extern BYTE Boot_RxD_bRxCnt;
extern BYTE Boot_RxD_fStatus;
#endif

// Create pragmas to support proper argument and return value passing
#pragma fastcall16  Boot_RxD_EnableInt
#pragma fastcall16  Boot_RxD_DisableInt
#pragma fastcall16  Boot_RxD_Start
#pragma fastcall16  Boot_RxD_Stop
#pragma fastcall16  Boot_RxD_bReadRxData
#pragma fastcall16  Boot_RxD_bReadRxStatus

#pragma fastcall16  Boot_RxD_cGetChar
#pragma fastcall16  Boot_RxD_cReadChar
#pragma fastcall16  Boot_RxD_iReadChar

#if ( Boot_RxD_RXBUF_ENABLE )
#pragma fastcall16  Boot_RxD_CmdReset
#pragma fastcall16  Boot_RxD_bCmdCheck
#pragma fastcall16  Boot_RxD_bErrCheck
#pragma fastcall16  Boot_RxD_bCmdLength
#pragma fastcall16  Boot_RxD_szGetParam
#pragma fastcall16  Boot_RxD_szGetRestOfParams
#endif

//-------------------------------------------------
// Prototypes of the Boot_RxD API.
//-------------------------------------------------
extern void  Boot_RxD_EnableInt(void);
extern void  Boot_RxD_DisableInt(void);
extern void  Boot_RxD_Start(BYTE bParity);
extern void  Boot_RxD_Stop(void);
extern BYTE  Boot_RxD_bReadRxData(void);
extern BYTE  Boot_RxD_bReadRxStatus(void);

// High level RX functions
extern CHAR         Boot_RxD_cGetChar(void);
extern CHAR         Boot_RxD_cReadChar(void);
extern INT          Boot_RxD_iReadChar(void);

#if ( Boot_RxD_RXBUF_ENABLE )
extern void   Boot_RxD_CmdReset(void);
extern BYTE   Boot_RxD_bCmdCheck(void);
extern BYTE   Boot_RxD_bErrCheck(void);
extern BYTE   Boot_RxD_bCmdLength(void);
extern char * Boot_RxD_szGetParam(void);
extern char * Boot_RxD_szGetRestOfParams(void);
#endif

// Old function call names, do not use.
// These names will be removed in a future release.
#pragma fastcall16 bBoot_RxD_ReadRxData
#pragma fastcall16 bBoot_RxD_ReadRxStatus
extern BYTE bBoot_RxD_ReadRxData(void);
extern BYTE bBoot_RxD_ReadRxStatus(void);

//-------------------------------------------------
// Constants for Boot_RxD API's.
//-------------------------------------------------

//------------------------------------
// Receiver Interrupt masks
//------------------------------------
#define Boot_RxD_INT_REG_ADDR                  ( 0x0e1 )
#define Boot_RxD_bINT_MASK                     ( 0x08 )

//------------------------------------
// Receiver Parity masks
//------------------------------------
#define  Boot_RxD_PARITY_NONE         0x00
#define  Boot_RxD_PARITY_EVEN         0x02
#define  Boot_RxD_PARITY_ODD          0x06

//------------------------------------
//  Receiver Status Register masks
//------------------------------------
#define  Boot_RxD_RX_ACTIVE           0x10
#define  Boot_RxD_RX_COMPLETE         0x08
#define  Boot_RxD_RX_PARITY_ERROR     0x80
#define  Boot_RxD_RX_OVERRUN_ERROR    0x40
#define  Boot_RxD_RX_FRAMING_ERROR    0x20
#define  Boot_RxD_RX_NO_ERROR         0xE0

#define  Boot_RxD_RX_NO_DATA         0x01

#define  Boot_RxD_RX_BUF_ERROR            0xF0  // Mask for any Rx that may occur.
#define  Boot_RxD_RX_BUF_OVERRUN          0x10  // This indicates the software buffer has
                                                           // been over run.
#define  Boot_RxD_RX_BUF_CMDTERM          0x01  // Command terminator has been received.

// Old defines, will be removed in future release
#define  RX8_PARITY_NONE         0x00
#define  RX8_PARITY_EVEN         0x02
#define  RX8_PARITY_ODD          0x06
#define  RX8_RX_ACTIVE           0x10
#define  RX8_RX_COMPLETE         0x08
#define  RX8_RX_PARITY_ERROR     0x80
#define  RX8_RX_OVERRUN_ERROR    0x40
#define  RX8_RX_FRAMING_ERROR    0x20
#define  RX8_RX_NO_ERROR         0xE0

//-------------------------------------------------
// Register Addresses for Boot_RxD
//-------------------------------------------------
#pragma ioport  Boot_RxD_CONTROL_REG:   0x02f              // Control register
BYTE            Boot_RxD_CONTROL_REG;
#pragma ioport  Boot_RxD_RX_SHIFT_REG:  0x02c              // RX Shift Register register
BYTE            Boot_RxD_RX_SHIFT_REG;
#pragma ioport  Boot_RxD_RX_BUFFER_REG: 0x02e              // RX Buffer Register
BYTE            Boot_RxD_RX_BUFFER_REG;
#pragma ioport  Boot_RxD_FUNC_REG:  0x12c                  // Function register
BYTE            Boot_RxD_FUNC_REG;
#pragma ioport  Boot_RxD_INPUT_REG: 0x12d                  // Input register
BYTE            Boot_RxD_INPUT_REG;
#pragma ioport  Boot_RxD_OUTPUT_REG:    0x12e              // Output register
BYTE            Boot_RxD_OUTPUT_REG;

// end of file Boot_RxD.h













/* Create pragmas to support proper argument and return value passing */
#pragma fastcall16  Boot_TxD_SetTxIntMode
#pragma fastcall16  Boot_TxD_EnableInt
#pragma fastcall16  Boot_TxD_DisableInt
#pragma fastcall16  Boot_TxD_Start
#pragma fastcall16  Boot_TxD_Stop
#pragma fastcall16  Boot_TxD_SendData
#pragma fastcall16  Boot_TxD_bReadTxStatus

// High level TX functions
#pragma fastcall16  Boot_TxD_PutSHexByte
#pragma fastcall16  Boot_TxD_PutSHexInt
#pragma fastcall16  Boot_TxD_CPutString
#pragma fastcall16  Boot_TxD_PutString
#pragma fastcall16  Boot_TxD_PutChar
#pragma fastcall16  Boot_TxD_Write
#pragma fastcall16  Boot_TxD_CWrite
#pragma fastcall16  Boot_TxD_PutCRLF

//-------------------------------------------------
// Prototypes of the Boot_TxD API.
//-------------------------------------------------
extern void  Boot_TxD_SetTxIntMode(BYTE bTxIntMode);
extern void  Boot_TxD_EnableInt(void);
extern void  Boot_TxD_DisableInt(void);
extern void  Boot_TxD_Start(BYTE bParity);
extern void  Boot_TxD_Stop(void);
extern void  Boot_TxD_SendData(BYTE bTxData);
extern BYTE  Boot_TxD_bReadTxStatus(void);

// High level TX functions
extern void   Boot_TxD_CPutString(const char * szRomString);
extern void   Boot_TxD_PutString(char * szRamString);
extern void   Boot_TxD_PutChar(CHAR cData);
extern void   Boot_TxD_Write(char * szRamString, BYTE bCount);
extern void   Boot_TxD_CWrite(const char * szRomString, INT iCount);
extern void   Boot_TxD_PutSHexByte(BYTE bValue);
extern void   Boot_TxD_PutSHexInt(INT iValue);
extern void   Boot_TxD_PutCRLF(void);

// Old style function name, Do Not Use.
// Will be removfr in a future release
#pragma fastcall16 bBoot_TxD_ReadTxStatus
extern BYTE bBoot_TxD_ReadTxStatus(void);

//------------------------------------
//  Transmitter Parity masks
//------------------------------------
#define  Boot_TxD_PARITY_NONE         0x00
#define  Boot_TxD_PARITY_EVEN         0x02
#define  Boot_TxD_PARITY_ODD          0x06

//------------------------------------
//  Transmitter Status Register masks
//------------------------------------
#define  Boot_TxD_TX_COMPLETE         0x20
#define  Boot_TxD_TX_BUFFER_EMPTY     0x10

#define Boot_TxD_INT_MODE_TX_REG_EMPTY 0x00
#define Boot_TxD_INT_MODE_TX_COMPLETE  0x01

//------------------------------------
// Transmitter Interrupt masks
//------------------------------------
#define Boot_TxD_INT_REG_ADDR                  ( 0x0e1 )
#define Boot_TxD_bINT_MASK                     ( 0x04 )

// Old style defines, do not use.  These
// will be removed in a future release.
#define  TX8_PARITY_NONE         0x00
#define  TX8_PARITY_EVEN         0x02
#define  TX8_PARITY_ODD          0x06
#define  TX8_TX_COMPLETE         0x20
#define  TX8_TX_BUFFER_EMPTY     0x10



//-------------------------------------------------
// Register Addresses for Boot_TxD
//-------------------------------------------------
#pragma ioport  Boot_TxD_CONTROL_REG:   0x02b              // Control register
BYTE            Boot_TxD_CONTROL_REG;
#pragma ioport  Boot_TxD_TX_SHIFT_REG:  0x028              // TX Shift Register register
BYTE            Boot_TxD_TX_SHIFT_REG;
#pragma ioport  Boot_TxD_TX_BUFFER_REG: 0x029              // TX Buffer Register
BYTE            Boot_TxD_TX_BUFFER_REG;
#pragma ioport  Boot_TxD_FUNC_REG:  0x128                  // Function register
BYTE            Boot_TxD_FUNC_REG;
#pragma ioport  Boot_TxD_INPUT_REG: 0x129                  // Input register
BYTE            Boot_TxD_INPUT_REG;
#pragma ioport  Boot_TxD_OUTPUT_REG:    0x12a              // Output register
BYTE            Boot_TxD_OUTPUT_REG;

// end of file Boot_TxD.h








#pragma fastcall16 Boot_Counter_EnableInt
#pragma fastcall16 Boot_Counter_DisableInt
#pragma fastcall16 Boot_Counter_Start
#pragma fastcall16 Boot_Counter_Stop
#pragma fastcall16 Boot_Counter_wReadCounter              // Read  DR0
#pragma fastcall16 Boot_Counter_WritePeriod               // Write DR1
#pragma fastcall16 Boot_Counter_wReadCompareValue         // Read  DR2
#pragma fastcall16 Boot_Counter_WriteCompareValue         // Write DR2

// The following symbols are deprecated.
// They may be omitted in future releases
//
#pragma fastcall16 wBoot_Counter_ReadCounter              // Read  DR0 (Deprecated)
#pragma fastcall16 wBoot_Counter_ReadCompareValue         // Read  DR2 (Deprecated)

//-------------------------------------------------
// Prototypes of the Boot_Counter API.
//-------------------------------------------------
extern void Boot_Counter_EnableInt(void);                           // Proxy Class 1
extern void Boot_Counter_DisableInt(void);                          // Proxy Class 1
extern void Boot_Counter_Start(void);                               // Proxy Class 1
extern void Boot_Counter_Stop(void);                                // Proxy Class 1
extern WORD Boot_Counter_wReadCounter(void);                        // Proxy Class 2
extern void Boot_Counter_WritePeriod(WORD wPeriod);                 // Proxy Class 1
extern WORD Boot_Counter_wReadCompareValue(void);                   // Proxy Class 1
extern void Boot_Counter_WriteCompareValue(WORD wCompareValue);     // Proxy Class 1

// The following functions are deprecated.
// They may be omitted in future releases
//
extern WORD wBoot_Counter_ReadCounter(void);            // Deprecated
extern WORD wBoot_Counter_ReadCompareValue(void);       // Deprecated


//--------------------------------------------------
// Constants for Boot_Counter API's.
//--------------------------------------------------

#define Boot_Counter_CONTROL_REG_START_BIT     ( 0x01 )
#define Boot_Counter_INT_REG_ADDR              ( 0x0e1 )
#define Boot_Counter_INT_MASK                  ( 0x02 )


//--------------------------------------------------
// Constants for Boot_Counter user defined values
//--------------------------------------------------

#define Boot_Counter_PERIOD                    ( 0x8000 )
#define Boot_Counter_COMPARE_VALUE             ( 0x4000 )


//-------------------------------------------------
// Register Addresses for Boot_Counter
//-------------------------------------------------

#pragma ioport  Boot_Counter_COUNTER_LSB_REG:   0x020      //DR0 Counter register LSB
BYTE            Boot_Counter_COUNTER_LSB_REG;
#pragma ioport  Boot_Counter_COUNTER_MSB_REG:   0x024      //DR0 Counter register MSB
BYTE            Boot_Counter_COUNTER_MSB_REG;
#pragma ioport  Boot_Counter_PERIOD_LSB_REG:    0x021      //DR1 Period register LSB
BYTE            Boot_Counter_PERIOD_LSB_REG;
#pragma ioport  Boot_Counter_PERIOD_MSB_REG:    0x025      //DR1 Period register MSB
BYTE            Boot_Counter_PERIOD_MSB_REG;
#pragma ioport  Boot_Counter_COMPARE_LSB_REG:   0x022      //DR2 Compare register LSB
BYTE            Boot_Counter_COMPARE_LSB_REG;
#pragma ioport  Boot_Counter_COMPARE_MSB_REG:   0x026      //DR2 Compare register MSB
BYTE            Boot_Counter_COMPARE_MSB_REG;
#pragma ioport  Boot_Counter_CONTROL_LSB_REG:   0x023      //Control register LSB
BYTE            Boot_Counter_CONTROL_LSB_REG;
#pragma ioport  Boot_Counter_CONTROL_MSB_REG:   0x027      //Control register MSB
BYTE            Boot_Counter_CONTROL_MSB_REG;
#pragma ioport  Boot_Counter_FUNC_LSB_REG:  0x120          //Function register LSB
BYTE            Boot_Counter_FUNC_LSB_REG;
#pragma ioport  Boot_Counter_FUNC_MSB_REG:  0x124          //Function register MSB
BYTE            Boot_Counter_FUNC_MSB_REG;
#pragma ioport  Boot_Counter_INPUT_LSB_REG: 0x121          //Input register LSB
BYTE            Boot_Counter_INPUT_LSB_REG;
#pragma ioport  Boot_Counter_INPUT_MSB_REG: 0x125          //Input register MSB
BYTE            Boot_Counter_INPUT_MSB_REG;
#pragma ioport  Boot_Counter_OUTPUT_LSB_REG:    0x122      //Output register LSB
BYTE            Boot_Counter_OUTPUT_LSB_REG;
#pragma ioport  Boot_Counter_OUTPUT_MSB_REG:    0x126      //Output register MSB
BYTE            Boot_Counter_OUTPUT_MSB_REG;
#pragma ioport  Boot_Counter_INT_REG:       0x0e1          //Interrupt Mask Register
BYTE            Boot_Counter_INT_REG;


//-------------------------------------------------
// Boot_Counter Macro 'Functions'
//-------------------------------------------------

#define Boot_Counter_Start_M \
   ( Boot_Counter_CONTROL_LSB_REG |=  Boot_Counter_CONTROL_REG_START_BIT )

#define Boot_Counter_Stop_M  \
   ( Boot_Counter_CONTROL_LSB_REG &= ~Boot_Counter_CONTROL_REG_START_BIT )

#define Boot_Counter_EnableInt_M   \
   M8C_EnableIntMask(  Boot_Counter_INT_REG, Boot_Counter_INT_MASK )

#define Boot_Counter_DisableInt_M  \
   M8C_DisableIntMask( Boot_Counter_INT_REG, Boot_Counter_INT_MASK )

// end of file Boot_Counter.h
