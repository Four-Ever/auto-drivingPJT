/**********************************************************************************************************************
 * \file Common_def.h
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

#ifndef COMMON_DEF_H_
#define COMMON_DEF_H_

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Data Structures---------------------------------------------------*/
/*********************************************************************************************************************/
 
/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
typedef enum
{
    false,
    true
}TruthState;

typedef enum
{
    BS_LEFT,
    BS_UP,
    BS_DOWN,
    BS_RIGHT,
    BS_SELECT,
    BS_NOTHING
}ButtonState;

typedef enum
{
    OTA_LEFT,
    OTA_RIGHT=3,
    OTA_SELECT=4
}OtaMode;

typedef enum
{
    DRIVE_POWEROFF,
    DRIVE_ACCEL,
    DRIVE_BRAKE,
    DRIVE_MODE,
    DRIVE_SERVICE,
    DRIVE_NOTHING
}DriveMode;

typedef enum
{
    DIR_P,
    DIR_D,
    DIR_R

}DriveDir;

typedef enum
{
    MS_DRIVE,
    MS_AUTO_PARKING
}DriveModeState;

typedef enum
{
    OFF_POWERON,
    OFF_FIND=3,
    OFF_AUTO_EXIT=4,
    OFF_NOTHING=6
}OffMode;

typedef enum
{

    PARK_BC_BRAKE=2,
    PARK_BC_NOTHING=6

}ParkingBrakeCheck;

typedef enum
{
    PARK_SEARCHING=0,
    PARK_ING_D,
    PARK_ING_R,
    PARK_FINISH,
    PARK_NOTHING

}ParkingMode;


typedef enum
{
    OTA_ORIGINAL,
    OTA_UPDATED
}OTAUpdateState;
typedef enum
{
    PIC_FULL_SQUARE,
    PIC_DOWN_ARROW,
    PIC_LEFT_DOWN_ARROW,
    PIC_ROTATED_LINE,

}Picture;


typedef enum
{
    CTRL_OFF,
    CTRL_ON,
    CTRL_OTA,
    CTRL_AUTO_PARKING,
    CTRL_TEST_LCD
}ControllerState;

typedef enum
{
    DO_NOT_AUTO_PARKING,
    DO_AUTO_PARKING

}ParkingAction;


typedef enum
{
    ORDER_ENGINE,
    ORDER_MOVE,
    ORDER_AUTO_PRK_REQ,
    ORDER_OTA_UDT_CFM,
    ORDER_OFF_REQ,
    ORDER_RECEIVE
}MessageOrder;

/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/


#endif /* COMMON_DEF_H_ */
