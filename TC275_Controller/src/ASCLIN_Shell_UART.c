/**********************************************************************************************************************
 * \file ASCLIN_Shell_UART.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 *
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are solely in the form of
 * machine-executable object code generated by a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Data_process.h"
#include <stdarg.h>
#include <stdio.h>
#include "ASCLIN_Shell_UART.h"
#include "Ifx_Types.h"
#include "IfxAsclin_Asc.h"
#include "Ifx_Shell.h"
#include "Ifx_Console.h"
#include "IfxPort.h"
//#include "SPI_CPU.h"
#include "ASCLIN_UART.h"
//#include "OurCan.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* Communication parameters */
#define ISR_PRIORITY_ASCLIN_TX      8                                       /* Priority for interrupt ISR Transmit  */
#define ISR_PRIORITY_ASCLIN_RX      4                                       /* Priority for interrupt ISR Receive   */
#define ISR_PRIORITY_ASCLIN_ER      12                                      /* Priority for interrupt ISR Errors    */
#define ASC_TX_BUFFER_SIZE          256                                     /* Define the TX buffer size in byte    */
#define ASC_RX_BUFFER_SIZE          256                                     /* Define the RX buffer size in byte    */
#define ASC_BAUDRATE                115200                                  /* Define the UART baud rate            */

#define BUFFER_SIZE 1024

/* LED */
#define LED                         &MODULE_P10,2                           /* LED Port Pin                         */

/* Shell commands and help descriptions */
#define COMMAND_INFO                "info"
#define COMMAND_TOGGLE              "toggle"
#define COMMAND_HELP                "help"

//DB data check
#define COMMAND_DBMSG               "message"


#define COMMAND_INFO_HELP_TEXT      "   : Show the example's info"
#define COMMAND_TOGGLE_HELP_TEXT    " : Command to toggle LED" ENDLINE \
                                    "         The correct syntax for this command is" ENDLINE \
                                    "         '" COMMAND_TOGGLE " [1]'"
#define COMMAND_HELP_HELP_TEXT      "   : Show this help list"

#define COMMAND_MSG_LIST            ": Select the number of fucntion " ENDLINE\
                                    "         1. ALL Signal on DB" ENDLINE\
                                    "         2. ALL unprocessed messages" ENDLINE\

#define PRINT_ALL_SIGNAL(io, flag, format, ...) \
    if ((1)) { \
        IfxStdIf_DPipe_print((io), (format ENDLINE), __VA_ARGS__); \
    }

#define PRINT_SIGNAL(io, flag, format, ...) \
    if ((flag)) { \
        IfxStdIf_DPipe_print((io), (format ENDLINE), __VA_ARGS__); \
    }

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void initLED(void);
void initSerialInterface(void);
void printInfo(IfxStdIf_DPipe *io);
boolean printShellInfo(pchar args, void *data, IfxStdIf_DPipe *io);
boolean toggleLEDShell(pchar args, void *data, IfxStdIf_DPipe *io);
boolean showMsgSignal(pchar args, void *data, IfxStdIf_DPipe *io);

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxStdIf_DPipe  g_ascStandardInterface;                                     /* Standard interface object            */
IfxAsclin_Asc   g_asclin;                                                   /* ASCLIN module object                 */
Ifx_Shell       g_shellInterface;                                           /* Shell interface object               */


//uint8 g_rxData[32];

/* The transfer buffers allocate memory for the data itself and for FIFO runtime variables.
 * 8 more bytes have to be added to ensure a proper circular buffer handling independent from
 * the address to which the buffers have been located.
 */
uint8 g_uartTxBuffer[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_uartRxBuffer[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

/* Array that stores the supported Shell commands */
const Ifx_Shell_Command g_shellCommands[] = {
    {COMMAND_INFO,   COMMAND_INFO_HELP_TEXT,    &g_shellInterface, &printShellInfo     },
    {COMMAND_TOGGLE, COMMAND_TOGGLE_HELP_TEXT,  &g_shellInterface, &toggleLEDShell    },
    {COMMAND_HELP,   COMMAND_HELP_HELP_TEXT,    &g_shellInterface, &Ifx_Shell_showHelp },
    {COMMAND_DBMSG,  COMMAND_MSG_LIST,          &g_shellInterface, &showMsgSignal},
    IFX_SHELL_COMMAND_LIST_END
};

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Macro to define Interrupt Service Routine.
 * This macro makes following definitions:
 * 1) Define linker section as .intvec_tc<vector number>_<interrupt priority>.
 * 2) define compiler specific attribute for the interrupt functions.
 * 3) define the Interrupt service routine as ISR function.
 *
 * IFX_INTERRUPT(isr, vectabNum, priority)
 *  - isr: Name of the ISR function.
 *  - vectabNum: Vector table number.
 *  - priority: Interrupt priority. Refer Usage of Interrupt Macro for more details.
 */
IFX_INTERRUPT(asc0TxISR, 0, ISR_PRIORITY_ASCLIN_TX);

void asc0TxISR(void)
{
    IfxStdIf_DPipe_onTransmit(&g_ascStandardInterface);
}

IFX_INTERRUPT(asc0RxISR, 0, ISR_PRIORITY_ASCLIN_RX);

void asc0RxISR(void)
{
    IfxStdIf_DPipe_onReceive(&g_ascStandardInterface);
}

IFX_INTERRUPT(asc0ErrISR, 0, ISR_PRIORITY_ASCLIN_ER);

void asc0ErrISR(void)
{
    IfxStdIf_DPipe_onError(&g_ascStandardInterface);
}

/* Function to print info in the console */
void printInfo(IfxStdIf_DPipe *io)
{
    IfxStdIf_DPipe_print(io, ENDLINE);
    IfxStdIf_DPipe_print(io, "******************************************************************************"ENDLINE);
    IfxStdIf_DPipe_print(io, "This is an example that shows how to use the Infineon Shell from iLLDs.       "ENDLINE);
    IfxStdIf_DPipe_print(io, "In order to toggle the LED enter the command '" COMMAND_TOGGLE "' followed by one of the "ENDLINE);
    IfxStdIf_DPipe_print(io, "following parameters:                                                         "ENDLINE);
    IfxStdIf_DPipe_print(io, "  - '1': toggles the LED                                                      "ENDLINE);
    IfxStdIf_DPipe_print(io, "Any other parameter turns off the LED and reports a Shell command error.      "ENDLINE);
    IfxStdIf_DPipe_print(io, "******************************************************************************"ENDLINE);
}

/* Function to show information about the example through the shell */
boolean printShellInfo(pchar args, void *data, IfxStdIf_DPipe *io)
{
    printInfo(io);
    return TRUE;
}

/* Function to toggle the LED */
boolean toggleLEDShell(pchar args, void *data, IfxStdIf_DPipe *io)
{
    if(args[0] == '1')
    {
        IfxPort_setPinState(LED, IfxPort_State_toggled);
        IfxStdIf_DPipe_print(io, "Toggled LED!" ENDLINE ENDLINE);
    }
    else
    {
        IfxPort_setPinState(LED, IfxPort_State_low);
        IfxStdIf_DPipe_print(io, "Command syntax not correct." ENDLINE \
                "The correct syntax for this command is" ENDLINE "    '" COMMAND_TOGGLE " [1]'" ENDLINE \
                "Turned off the LED!" ENDLINE);
        return FALSE; /* Returning false triggers a Shell command error */
    }
    return TRUE;
}


boolean showMsgSignal(pchar args, void *data, IfxStdIf_DPipe *io)
{
    IfxStdIf_DPipe_print(io, "******************************************************************************"ENDLINE);
    if(args[0] == '1'){
        IfxStdIf_DPipe_print(io, "all messages"ENDLINE);
//        print_all(io);

        int dlc = g_txData[0];
        for(int i=0; i<dlc+2;i++)
        {
            if(i == 1)
                IfxStdIf_DPipe_print(io, "%d : 0X%02X"ENDLINE, i,g_txData[i]);
            else
                IfxStdIf_DPipe_print(io, "%d : %d"ENDLINE, i,g_txData[i]);
        }


    }
    else if(args[0] =='2'){
        for(int i=0;i<4;i++)
        IfxStdIf_DPipe_print(io, "rx messages %c\n"ENDLINE,g_rxData[i]);
//        IfxStdIf_DPipe_print(io, "all messages"ENDLINE);
    }
    else{
        IfxStdIf_DPipe_print(io, "Command syntax not correct." ENDLINE);
        return FALSE;
    }
    IfxStdIf_DPipe_print(io, "******************************************************************************"ENDLINE);
    return TRUE;
}

/* Function to initialize GPIO pins for the LED */
void initLED(void)
{
    /* Initialize GPIO pins for the LED */
    IfxPort_setPinMode(LED, IfxPort_Mode_outputPushPullGeneral);

    /* Turn off the LED */
    IfxPort_setPinState(LED, IfxPort_State_low);
}

/* Function to initialize ASCLIN module */
void initSerialInterface(void)
{
    IfxAsclin_Asc_Config ascConf;

    /* Set default configurations */
    IfxAsclin_Asc_initModuleConfig(&ascConf, &MODULE_ASCLIN3); /* Initialize the structure with default values      */

    /* Set the desired baud rate */
    ascConf.baudrate.baudrate = ASC_BAUDRATE;                                   /* Set the baud rate in bit/s       */
    ascConf.baudrate.oversampling = IfxAsclin_OversamplingFactor_16;            /* Set the oversampling factor      */

    /* Configure the sampling mode */
    ascConf.bitTiming.medianFilter = IfxAsclin_SamplesPerBit_three;             /* Set the number of samples per bit*/
    ascConf.bitTiming.samplePointPosition = IfxAsclin_SamplePointPosition_8;    /* Set the first sample position    */

    /* ISR priorities and interrupt target */
    ascConf.interrupt.txPriority = ISR_PRIORITY_ASCLIN_TX;  /* Set the interrupt priority for TX events             */
    ascConf.interrupt.rxPriority = ISR_PRIORITY_ASCLIN_RX;  /* Set the interrupt priority for RX events             */
    ascConf.interrupt.erPriority = ISR_PRIORITY_ASCLIN_ER;  /* Set the interrupt priority for Error events          */
    ascConf.interrupt.typeOfService = IfxSrc_Tos_cpu0;

    /* Pin configuration */
    const IfxAsclin_Asc_Pins pins = {
            .cts        = NULL_PTR,                         /* CTS pin not used                                     */
            .ctsMode    = IfxPort_InputMode_pullUp,
            .rx         = &IfxAsclin3_RXD_P32_2_IN ,        /* Select the pin for RX connected to the USB port      */
            .rxMode     = IfxPort_InputMode_pullUp,         /* RX pin                                               */
            .rts        = NULL_PTR,                         /* RTS pin not used                                     */
            .rtsMode    = IfxPort_OutputMode_pushPull,
            .tx         = &IfxAsclin3_TX_P15_7_OUT,         /* Select the pin for TX connected to the USB port      */
            .txMode     = IfxPort_OutputMode_pushPull,      /* TX pin                                               */
            .pinDriver  = IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    ascConf.pins = &pins;

    /* FIFO buffers configuration */
    ascConf.txBuffer = g_uartTxBuffer;                      /* Set the transmission buffer                          */
    ascConf.txBufferSize = ASC_TX_BUFFER_SIZE;              /* Set the transmission buffer size                     */
    ascConf.rxBuffer = g_uartRxBuffer;                      /* Set the receiving buffer                             */
    ascConf.rxBufferSize = ASC_RX_BUFFER_SIZE;              /* Set the receiving buffer size                        */

    /* Init ASCLIN module */
    IfxAsclin_Asc_initModule(&g_asclin, &ascConf);          /* Initialize the module with the given configuration   */
}

/* Function to initialize the Shell */
void initShellInterface(void)
{
    /* Initialize the hardware peripherals */
    initLED();
    initSerialInterface();

    /* Initialize the Standard Interface */
    IfxAsclin_Asc_stdIfDPipeInit(&g_ascStandardInterface, &g_asclin);

    /* Initialize the Console */
    Ifx_Console_init(&g_ascStandardInterface);

    /* Print info to the console */
//    printInfo(&g_ascStandardInterface);
//    Ifx_Console_print(ENDLINE "Enter '" COMMAND_HELP "' to see the available commands" ENDLINE);

    /* Initialize the shell */
    Ifx_Shell_Config shellConf;
    Ifx_Shell_initConfig(&shellConf);                       /* Initialize the structure with default values         */

    shellConf.standardIo = &g_ascStandardInterface;         /* Set a pointer to the standard interface              */
    shellConf.commandList[0] = &g_shellCommands[0];         /* Set the supported command list                       */

    Ifx_Shell_init(&g_shellInterface, &shellConf);          /* Initialize the Shell with the given configuration    */
}

/* Function to process the incoming received data */
void runShellInterface(void)
{
    /* Process the received data */
    Ifx_Shell_process(&g_shellInterface);
}
//
//void print_debug(const char *buf){
//
//    IfxStdIf_DPipe_print(&g_ascStandardInterface, "%s",buf);
//}

void myprintf(const char *format, ...) {
    char buffer[BUFFER_SIZE];  // 출력할 문자열을 저장할 버퍼
    char* args;

    va_start(args, format);  // 가변 인자 초기화
    vsprintf(buffer, format, args);  // 포맷된 문자열을 버퍼에 저장
    va_end(args);  // 가변 인자 종료

    // IfxStdIf_DPipe_print를 사용하여 UART로 출력
    IfxStdIf_DPipe_print(&g_ascStandardInterface, "%s", buffer);
}
