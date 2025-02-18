/**********************************************************************************************************************
 * \file OurCan_signal.h
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

 #ifndef OURCAN_SIGNAL_H_
 #define OURCAN_SIGNAL_H_

 /*********************************************************************************************************************/
 /*-----------------------------------------------------Includes------------------------------------------------------*/
 /*********************************************************************************************************************/
 # include "Platform_Types.h"
 /*********************************************************************************************************************/
 /*------------------------------------------------------Macros-------------------------------------------------------*/
 /*********************************************************************************************************************/

 /*********************************************************************************************************************/
 /*-------------------------------------------------Global variables--------------------------------------------------*/
 /*********************************************************************************************************************/

 /*********************************************************************************************************************/
 /*-------------------------------------------------Data Structures---------------------------------------------------*/
 // VCU TX

 typedef struct //2
 {
      uint8 vehicle_velocity : 7;
      sint8 vehicle_steering_angle : 7;
      uint8 vehicle_transmission : 2;

 }VCU_Vehicle_Status_Msg;

 typedef struct // 1
 {
         uint8 parking_status : 2;

 } VCU_Parking_Status_Msg;

 typedef struct
 {
         uint8 exiting_status : 1;
 }VCU_Exiting_Status_Msg;


 typedef struct // 1
 {
         uint8 vehicle_engine_status : 1;

 } VCU_Vehicle_Engine_Status_Msg;

 typedef struct
 {
         uint8 Lane_Request : 1;
 }VCU_ParkingLane_Request_Msg;

 typedef struct // 1
 {
        uint8 camera_num : 2;

 } VCU_Camera_Msg;

 typedef struct // 1
 {
         uint8 F_obstacle : 1;
         uint8 B_obstacle : 1;
         uint8 R_obstacle : 1;
         uint8 L_obstacle : 1;

 } SCU_Obstacle_Detection_Msg;



 // VCU RX CGW TX
 typedef struct  //4
 {
         uint32 ota_file_size;

 } CGW_OTA_File_Size_Msg;

 typedef struct //8
 {
         uint64 ota_file_data;

 } CGW_OTA_File_Data_Msg;

 typedef struct // 8
 {
         uint64 ota_control;

 } CGW_OTA_Control_Msg;

 typedef struct // 6
 {
         uint16 cordi_data_y1 : 11;
         sint16 cordi_data_x1 : 10;
         uint16 cordi_data_y2 : 11;
         sint16 cordi_data_x2 : 10;

 } CCU_Cordi_data1_Msg;

 typedef struct // 6
 {
         uint16 cordi_data_y3 : 11;
         sint16 cordi_data_x3 : 10;
         uint16 cordi_data_y4 : 11;
         sint16 cordi_data_x4 : 10;
         uint8 using_camera : 2;
         uint8 trust_value : 6;

 } CCU_Cordi_data2_Msg;

 typedef struct
 {
         uint8 right_angle_lane_detected : 1;

 } CCU_RightAngle_detect_Msg;

 typedef struct
 {
         sint8 parking_back_lane_angle : 8;

 } CCU_ParkingAngle_detect_Msg;

 typedef struct
 {
         uint8 parking_back_lane_detected : 1;

 } CCU_Parking_Complete_Msg;

 typedef struct //1
 {
         uint8 control_engine : 1;

 } CGW_Engine_Msg;

 typedef struct // 2
 {
         uint8 control_accel : 1;
         uint8 control_brake : 1;
         sint8 control_steering_angle : 7;
         uint8 control_transmission : 2;

 } CGW_Move_Msg;

 typedef struct // 1
 {
         uint8 auto_parking : 1;

 } CGW_Auto_Parking_Request_Msg;

 typedef struct // 1
 {
         uint8 alert_request : 1;
         uint8 auto_exit_request : 1;

 } CGW_Off_Request_Msg;



 // VCU RX CTRL TX
 typedef struct //1
 {
         uint8 control_engine : 1;

 } CTRL_Engine_Msg;

 typedef struct // 2
 {
         uint8 control_accel : 1;
         uint8 control_brake : 1;
         sint8 control_steering_angle : 7;
         uint8 control_transmission : 2;

 } CTRL_Move_Msg;

 typedef struct // 1
 {
         uint8 auto_parking : 1;

 } CTRL_Auto_Parking_Request_Msg;

 typedef struct // 1
 {
         uint8 alert_request : 1;
         uint8 auto_exit_request : 1;

 } CTRL_Off_Request_Msg;


 /*********************************************************************************************************************/

 /*********************************************************************************************************************/
 /*--------------------------------------------Private Variables/Constants--------------------------------------------*/
 /*********************************************************************************************************************/

 /*********************************************************************************************************************/
 /*------------------------------------------------Function Prototypes------------------------------------------------*/
 /*********************************************************************************************************************/

 #endif /* OURCAN_SIGNAL_H_ */
