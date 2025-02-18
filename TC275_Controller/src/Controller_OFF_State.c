/**********************************************************************************************************************
 * \file Controller_OFF_State.c
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
#include "Controller_Logic.h"
#include "TC275_LCD_16x2.h"
#include "Data_process.h"

#include <stdio.h>
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
static char str[20];
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/


void Show_Off_State()
{
    if(local_udt_req_sig.ota_update_request == 1 &&
            g_current_ota_update == OTA_ORIGINAL &&
            g_isreq_reject == false)
    {
        g_current_ctrl_state = CTRL_OTA_CONFIRM;
    }

    LCD1602_clear();


    // 첫째 줄 - 코멘트
    LCD1602_1stLine();



    LCD1602_print("PWR");
    LCD1602_print_picture(PIC_ROTATED_LINE);
    LCD1602_print("VEHICLE FIND");


    // 둘째 줄
    LCD1602_2ndLine();

    LCD1602_print_picture(PIC_LEFT_DOWN_ARROW);

    LCD1602_print("ON");

    LCD1602_print_picture(PIC_DOWN_ARROW);
    LCD1602_print(" ");
    LCD1602_print_picture(PIC_DOWN_ARROW);

    LCD1602_print("AUTO LEAVE");





    static OffMode prev_state = OFF_NOTHING;


    switch(g_btn_adc_result){
    case (OFF_POWERON):
    {
        LCD1602_clear();


        LCD1602_1stLine();
        sprintf(str, "POWER ON");
        LCD1602_print(str);
        LCD1602_loading();
        if(prev_state != OFF_POWERON){
            prev_state = OFF_POWERON;
            //화면전환
            g_current_ctrl_state = CTRL_ON;

            //명령 송신
            msg.engine_msg.signal.control_engine = CTRL_ON;

#ifndef PERIOD_VER
            Command[ORDER_ENGINE]();
#endif
            //********* 시동키면 P로 수정해야할수도 *************
            msg.move_msg.signal.control_transmission = DIR_P;



        }
        break;
    }
    case (OFF_FIND):
    {
        LCD1602_clear();

        if(prev_state != OFF_FIND){
            prev_state = OFF_FIND;
            //SPI송신
            msg.off_req_msg.signal.alert_request = 1;
#ifndef PERIOD_VER
            Command[ORDER_OFF_REQ]();
            // 1 신호만 보내고 기본신호 0으로 변경,
            // Msg에 두개의 signal이 있어서 변경안해놓으면 신호겹칠듯
            msg.off_req_msg.signal.alert_request = 0;
#endif



        }
        LCD1602_1stLine();
        sprintf(str, "ALERT MY CAR");
        LCD1602_print(str);
        LCD1602_loading();

        break;
    }

    // **********OTA시 활성화*********************//
    case (OFF_AUTO_EXIT):
    {
        LCD1602_clear();


        if(g_current_ota_update == OTA_UPDATED)
        {
            if(prev_state != OFF_AUTO_EXIT){
                prev_state = OFF_AUTO_EXIT;

                //명령 송신
                msg.off_req_msg.signal.auto_exit_request = 1;
#ifndef PERIOD_VER
                Command[ORDER_OFF_REQ]();
                // 1 신호만 보내고 기본신호 0으로 변경,
                // Msg에 두개의 signal이 있어서 변경안해놓으면 신호겹칠듯
                msg.off_req_msg.signal.auto_exit_request = 0;
#endif

            }
            LCD1602_1stLine();
            sprintf(str, "LEAVING MY CAR");
            LCD1602_print(str);
            LCD1602_2ndLine();
            LCD1602_loading();

            g_current_ctrl_state = CTRL_AUTO_EXIT;
        }
        else
        {
            LCD1602_1stLine();
            sprintf(str, "DO SUBSCRIBE TO");
            LCD1602_print(str);
            LCD1602_2ndLine();
            sprintf(str, "USE AUTO LEAVING");
            LCD1602_print(str);
            LCD1602_loading();

        }
        break;
    }


    //******************************************//
    default :
    {
//        sprintf(str, "");
#ifdef PERIOD_VER
        //2턴 지나고 나서 0으로 바꿈
        if(prev_state == OFF_NOTHING){
            msg.off_req_msg.signal.alert_request = 0;
            msg.off_req_msg.signal.auto_exit_request = 0;
        }
#endif
        prev_state = OFF_NOTHING;
    }
    }
//    LCD1602_print(str);
}


void Show_Auto_Exit_State()
{

#ifdef BEFORE_RECEIVE_TEST
    msg.cgw_exit_status_msg.signal.exiting_status = (uint8)msg.move_msg.signal.control_steering_angle %2;
    ready_flag.cgw_exit_status_flag = RECEIVE_COMPLETED;
#endif
    LCD1602_clear();
    // 첫째 줄 - 코멘트
    LCD1602_1stLine();

    switch (local_exit_status_sig.exiting_status)
    {
        case (EXIT_ING) :
        {

            LCD1602_print("EXITING PARKING");
            LCD1602_2ndLine();
            LCD1602_print("SPACE");
            static int target = 0;
            for(int i =0 ; i< target; i++)
            {
                LCD1602_print(".");
            }
            target++;
            if(target == 16) target =0;
            break;

        }
        case (EXIT_FINISHED) :
        {
            LCD1602_print("EXITING FINISHED");
            LCD1602_2ndLine();
            LCD1602_loading();
            g_current_ctrl_state = CTRL_OFF;
            break;
        }
        default :
        {

        }
    }
}
/*********************************************************************************************************************/
