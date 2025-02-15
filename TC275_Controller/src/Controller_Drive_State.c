/**********************************************************************************************************************
 * \file Controller_Drive_State.c
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
void Show_Drive_State()
{
    LCD1602_clear();



    static DriveModeState ms_state = MS_AUTO_PARKING;
    static DriveMode prev_state = DRIVE_NOTHING;
    LCD1602_1stLine();

    switch(g_btn_adc_result){
    case (DRIVE_ACCEL):
    {
        //명령송신
        if(dir_state == DIR_D || dir_state == DIR_R){
            msg.move_msg.signal.control_accel = 1;
            msg.move_msg.signal.control_brake = 0;
            sprintf(str, "GOGOGO");

            // 테스트용 ******************** 직접 받아서 할땐 지워야함!!!
            #ifdef BEFORE_RECEIVE_TEST
            msg.cgw_vhc_status_msg.signal.vehicle_velocity++;
            #endif
        }
        else if (dir_state == DIR_P)
        {
            sprintf(str, "CHANGE DR MODE!");
        }

        prev_state = DRIVE_ACCEL;
        break;
    }
    case (DRIVE_BRAKE):
    {
        //명령송신
        if(dir_state == DIR_D || dir_state == DIR_R){
            msg.move_msg.signal.control_accel = 0;
            msg.move_msg.signal.control_brake = 1;
            sprintf(str, "SLOW SLOW");
            // 테스트용 ******************** 직접 받아서 할땐 지워야함!!!
            #ifdef BEFORE_RECEIVE_TEST
            if (msg.cgw_vhc_status_msg.signal.vehicle_velocity >0)
                msg.cgw_vhc_status_msg.signal.vehicle_velocity--;
            #endif

        }
        else if (dir_state == DIR_P)
        {
            sprintf(str, "CHANGE DR MODE!");
        }

        prev_state = DRIVE_BRAKE;
        break;
    }
    case (DRIVE_POWEROFF):
    {
        if(prev_state != DRIVE_POWEROFF){
            prev_state = DRIVE_POWEROFF;
            //차량 모드가 Parking일때만 전원 종료 수행
            if(dir_state == DIR_P)
            {
                g_current_ctrl_state = CTRL_OFF;
                // 명령 송신
                msg.engine_msg.signal.control_engine = g_current_ctrl_state;
#ifndef PERIOD_VER
                Command[ORDER_ENGINE]();
#endif
                LCD1602_clear();
                LCD1602_1stLine();
                sprintf(str, "POWER OFF");
                LCD1602_print(str);
                LCD1602_loading();
            }
            else if(local_vhc_status_sig.vehicle_velocity == 0)
            {
                dir_state = DIR_P;
                //msg.move_msg.signal.control_transmission = dir_state;

                LCD1602_clear();
                LCD1602_1stLine();
                sprintf(str, "PARKING");
                LCD1602_print(str);
                LCD1602_loading();

            }

        }
        // 위 조건을 만족하지 못하고 나왔다면, 속도를 0으로 하라는 메시지 출력
        if(local_vhc_status_sig.vehicle_velocity != 0){
            LCD1602_clear();
            LCD1602_1stLine();
            sprintf(str, "SET SPEED TO 0!!");
            LCD1602_print(str);
        }
        break;
    }
    case (DRIVE_MODE):
    {
        sprintf(str, "DIRECTION CHANGE");

        if(prev_state != DRIVE_MODE){
            prev_state = DRIVE_MODE;

            if(dir_state == DIR_D)
            {
                dir_state = DIR_R;
            }
            else if(dir_state == DIR_R)
            {

                dir_state = DIR_D;

            }
            else if(dir_state == DIR_P)
            {
                dir_state = DIR_D;
            }
            // 명령 송신
            //msg.move_msg.signal.control_transmission = dir_state;
        }
        break;
    }
    // **********OTA시 활성화*********************//
    case (DRIVE_SERVICE):
    {
        LCD1602_clear();
        if(g_current_ota_update == OTA_UPDATED)
        {
            if(prev_state != DRIVE_SERVICE){
                prev_state = DRIVE_SERVICE;
                if((dir_state == DIR_D || dir_state == DIR_R) &&
                        (local_vhc_status_sig.vehicle_velocity == 0))
                {

                    ms_state = MS_AUTO_PARKING;
                    dir_state = DIR_P;

                    //명령 송신 아래 로딩함수 때문에, 바로 전송
                    msg.auto_park_req_msg.signal.auto_parking = DO_AUTO_PARKING;
#ifndef PERIOD_VER
                    Command[ORDER_AUTO_PRK_REQ]();
#endif

                    LCD1602_1stLine();
                    sprintf(str, "AUTO PARKING");
                    LCD1602_print(str);
                    LCD1602_loading();


                    //화면 전환
                    g_current_ctrl_state = CTRL_AUTO_PARKING;

                }
                else if(dir_state == DIR_P)
                {
                    ms_state = MS_DRIVE;
                    dir_state = DIR_D;
#ifdef PERIOD_VER
                    msg.auto_park_req_msg.signal.auto_parking = DO_NOT_AUTO_PARKING;
#endif

#ifndef PERIOD_VER
                    //이벤트 함수라, 굳이 전송 x
//                    msg.auto_park_req_msg.signal.auto_parking = 0;
//                    Command[ORDER_AUTO_PRK_REQ]();
#endif
                    LCD1602_1stLine();
                    sprintf(str, "DRIVING MODE");
                    LCD1602_print(str);
                    LCD1602_loading();
                }
                //명령 송신
                //msg.move_msg.signal.control_transmission = dir_state;

            }
            // 위 조건을 만족하지 못하고 나왔다면, 속도를 0으로 하라는 메시지 출력
            if(local_vhc_status_sig.vehicle_velocity != 0 && dir_state != DIR_P){
                LCD1602_clear();
                LCD1602_1stLine();
                sprintf(str, "SET SPEED TO 0!!");
                LCD1602_print(str);
            }
        }
        else
        {
            LCD1602_1stLine();
            sprintf(str, "DO SUBSCRIBE TO");
            LCD1602_print(str);
            LCD1602_2ndLine();
            sprintf(str, "USE AUTO PARKING");
            LCD1602_print(str);
            LCD1602_loading();

        }



        break;
    }
    //******************************************//
    default :
    {
        sprintf(str, "");
        prev_state = BS_NOTHING;
        msg.move_msg.signal.control_accel = 0;
        msg.move_msg.signal.control_brake = 0;
    }
    }

    LCD1602_print(str);


    LCD1602_2ndLine();
    if(dir_state == DIR_D || dir_state == DIR_R)
    {
        if(dir_state == DIR_D)
        {
            sprintf(str, "D %d",local_vhc_status_sig.vehicle_velocity);
        }
        else if(dir_state == DIR_R)
        {
            sprintf(str, "R %d",local_vhc_status_sig.vehicle_velocity);
        }
        LCD1602_print(str);

        LCD1602_setCursor(2, 7);
        LCD1602_print("DRIVE MODE");

    }
    else if(dir_state == DIR_P)
    {
        sprintf(str, "P %d PARKING MODE",local_vhc_status_sig.vehicle_velocity);
        LCD1602_print(str);
    }

}

void Show_Auto_Parking_State()
{
    LCD1602_clear();


    // 첫째 줄 - 코멘트
    LCD1602_1stLine();


    static ParkingMode prev_state = PARK_NOTHING;


    // 테스트용 ******************** 직접 받아서 할땐 지워야함!!!
    #ifdef BEFORE_RECEIVE_TEST
//    msg.cgw_vhc_status_msg.signal.vehicle_transmission = DIR_D;
    #endif

    switch(g_btn_adc_result){
    case (PARK_BC_BRAKE):
    {
        // parking중에도 Brake가 동작하도록
        //명령송신
        msg.move_msg.signal.control_accel = 0;
        msg.move_msg.signal.control_brake = 1;
        LCD1602_print("PAUSE PARKING");
        LCD1602_2ndLine();
        LCD1602_loading();
        // 테스트용 ******************** 직접 받아서 할땐 지워야함!!!
        #ifdef BEFORE_RECEIVE_TEST
        if (msg.cgw_vhc_status_msg.signal.vehicle_velocity >0)
            msg.cgw_vhc_status_msg.signal.vehicle_velocity--;
        #endif

//        dir_state = local_vhc_status_sig.vehicle_transmission;
        g_current_ctrl_state = CTRL_ON;

        break;
    }

    default :
    {
//        sprintf(str, "");
//        LCD1602_print(str);
    }
    }

    // 테스트용 ******************** 직접 받아서 할땐 지워야함!!!
#ifdef BEFORE_RECEIVE_TEST
    msg.cgw_prk_status_msg.signal.parking_status = ((uint8)msg.move_msg.signal.control_steering_angle)>=45 ? 3 : ((uint8)msg.move_msg.signal.control_steering_angle)%3;
#endif

    switch (local_prk_status_sig.parking_status)
    {
    case (PARK_SEARCHING) :
    {

        LCD1602_print("SEARCHING EMPTY");
        LCD1602_2ndLine();
        LCD1602_print("SPACE");
        static int target = 0;
        for(int i =0 ; i< target; i++)
        {
            LCD1602_print(".");
        }
        target++;
        if(target == 16) target =0;
        prev_state = PARK_SEARCHING;
        break;

    }
    case (PARK_ING_D) :
    {

#ifdef BEFORE_RECEIVE_TEST
        msg.cgw_vhc_status_msg.signal.vehicle_transmission = DIR_D;
#endif
        LCD1602_print("PARKING IN DRIVE");
        LCD1602_2ndLine();
        static int target = 0;
        for(int i =0 ; i< target; i++)
        {
            LCD1602_print(".");
        }
        target++;
        if(target == 16) target =0;

        prev_state = PARK_ING_D;
        break;
    }
    case (PARK_ING_R) :
    {
#ifdef BEFORE_RECEIVE_TEST
        msg.cgw_vhc_status_msg.signal.vehicle_transmission = DIR_R;
#endif
        LCD1602_print("PARKING IN REVRS");
        LCD1602_2ndLine();
        static int target = 0;
        for(int i =0 ; i< target; i++)
        {
            LCD1602_print(".");
        }
        target++;
        if(target == 16) target =0;

        prev_state = PARK_ING_R;
        break;
    }
    case (PARK_FINISH) :
    {
#ifdef BEFORE_RECEIVE_TEST
        msg.cgw_vhc_status_msg.signal.vehicle_transmission = DIR_P;
#endif
        //msg.move_msg.signal.control_transmission = dir_state;

        if(prev_state == PARK_FINISH){
         // Parking 기어 반영을 위함, 한번 더 수행 후 화면전환
            LCD1602_print("PARKING FINISHED");
            LCD1602_2ndLine();
            LCD1602_loading();
            g_current_ctrl_state = CTRL_ON;

        }
        prev_state = PARK_FINISH;
        break;
    }
    default :
    {

    }

    }
    dir_state = local_vhc_status_sig.vehicle_transmission;

}
/*********************************************************************************************************************/
