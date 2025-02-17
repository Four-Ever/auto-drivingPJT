/**********************************************************************************************************************
 * \file STM_Interrupt.c
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
#include "STM_Interrupt.h"
#include "stdlib.h"
/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define ISR_PRIORITY_STM        40                              /* Priority for interrupt ISR                       */
#define TIMER_INT_TIME          10                             /* Time between interrupts in ms                    */

//#define LED                     &MODULE_P13,0                   /* LED toggled in Interrupt Service Routine (ISR)   */
#define STM                     &MODULE_STM0                    /* STM0 is used in this example                     */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxStm_CompareConfig g_STMConf;                                 /* STM configuration structure                      */
Ifx_TickTime g_ticksFor1ms;                                   /* Variable to store the number of ticks to wait    */

extern IfxGpt12_IncrEnc_Config gpt12Config;
extern uint8 CPR;
sint32 Enc_count_new = 0;
sint32 Enc_count_old = 0;
float32 Enc_count_diff = 0;
float32 motor_speed_rpm=0;

sint32 s32_motor_speed_rpm=0;
sint32 s32_DisSum = 0;
sint32 i1 = 0;

PIDREG3 speed_pid = PIDREG3_DEFAULTS;

float32 s_T_samp= 0.001*TIMER_INT_TIME;
float32 RPM_max = 5000, RPM_min = -5000;

//float32 Kp_s,Ki_s,Kd_s;
//float32 RPM_CMD1;


sint32 Enc_count;
/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
//void initLED(void);
//void initSTM(void);
extern void Encoder_update(void);
void move_to_tardis(void);
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
IFX_INTERRUPT(isrSTM, 0, ISR_PRIORITY_STM);

//void pid_reset(PIDREG3 *v);

void RPM_cal(void)
{
    Encoder_update();   //지워도 되는지 확인할것
    Enc_count_new = gpt12Config.module->T2.U;

    if (abs(Enc_count_new - Enc_count_old) > 32768)
    {
        // 오버, 언더플로우 발생
        if (Enc_count_new > Enc_count_old)
        {
            // 언더플로우 (역방향)
            Enc_count_diff = (float32)((Enc_count_new - 65535) - Enc_count_old);
        }
        else
        {
            // 오버플로우 (정방향)
            Enc_count_diff = (float32)((65535 - Enc_count_old) + Enc_count_new + 1);
        }
    }
    else
    {
        // 정상적일 때,
        Enc_count_diff = (float32)(Enc_count_new - Enc_count_old);
    }
    speed_pid.DisSum += Enc_count_diff * tick_dis;
    s32_DisSum = (sint32)(speed_pid.DisSum * 10000);

    //s32_DisSum = (sint32)(Enc_count_new);

    motor_speed_rpm = Enc_count_diff/(float32)CPR/(float32)(TIMER_INT_TIME*0.001)*60.0f;
    s32_motor_speed_rpm = (sint32)motor_speed_rpm;

    Enc_count_old = Enc_count_new;
}


void PI_const_update(void)
{

    speed_pid.Kp = Kp_s;
    speed_pid.Ki = Ki_s;
    speed_pid.Kd = Kd_s;
    speed_pid.Kc = 1/Kp_s;
    speed_pid.T_samp = s_T_samp;
//        speed_pid.Current_mode = 1;
    speed_pid.OutMax = RPM_max;
    speed_pid.OutMin = RPM_min;

}
void PI_Speed_con(void)
{
//    RPM_Err = RPM_CMD - motor_speed_rpm;
    PI_const_update();
    // Initialze the PID module for Current
    if(RPM_CMD1==0)
    {
        speed_pid.reset((void *)&speed_pid);
        setMotorControl(0,0);

    }
    else
    {
        speed_pid.Ref=RPM_CMD1;   // speed reference
        speed_pid.Fdb= motor_speed_rpm; // speed measured by ENC
        speed_pid.calc((void *)&speed_pid);   // Calculate speed PID Controller
    }

}


void isrSTM(void)
{
    /* Update the compare register value that will trigger the next interrupt and toggle the LED */
//    IfxPort_setPinState(LED, IfxPort_State_toggled);

    RPM_cal();
//    static sint32 ii =0;
//    ii++;
    PI_Speed_con();

    if (speed_pid.Out>=0)
    {
        setMotorControl(0,1);  //void setMotorControl(uint8 direction, uint8 enable)

        PWM_set(speed_pid.Out);      /* Change the intensity of the LED  */
    }
    else {
        setMotorControl(1,1);  //void setMotorControl(uint8 direction, uint8 enable)

        PWM_set(speed_pid.Out*-1);      /* Change the intensity of the LED  */
    }

    if (speed_pid.DriveMode == move_distance_control)
    {
        move_to_tardis();
    }

    IfxStm_increaseCompare(STM, g_STMConf.comparator, g_ticksFor1ms);
}

/* Function to initialize the STM */
void initSTM(void)
{
    IfxStm_initCompareConfig(&g_STMConf);           /* Initialize the configuration structure with default values   */

    g_STMConf.triggerPriority = ISR_PRIORITY_STM;   /* Set the priority of the interrupt                            */
    g_STMConf.typeOfService = IfxSrc_Tos_cpu0;      /* Set the service provider for the interrupts                  */
    g_STMConf.ticks = g_ticksFor1ms;              /* Set the number of ticks after which the timer triggers an
                                                     * interrupt for the first time                                 */
    IfxStm_initCompare(STM, &g_STMConf);            /* Initialize the STM with the user configuration               */
}


/* Function to initialize all the peripherals and variables used */
void initPeripherals(void)
{
    /* Initialize time constant */
    g_ticksFor1ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, TIMER_INT_TIME);
    initSTM();                                      /* Configure the STM module                               */

}


void init_move_distance_control(float32 tarDis, float32 tarVel)
{
    speed_pid.DriveMode = move_distance_control;

    speed_pid.DisSum = 0;
    speed_pid.TargetDis = tarDis;
    speed_pid.TargetVel = tarVel;

    RPM_CMD1 = speed_pid.TargetVel;
}


void move_to_tardis(void)
{
    if (speed_pid.TargetDis >= 0)
    {
        if (speed_pid.DisSum > speed_pid.TargetDis)
        {
            speed_pid.DriveMode = move_pwm_control;

            RPM_CMD1 = 0;

            speed_pid.DisSum = 0;
            speed_pid.TargetDis = 0;
            speed_pid.TargetVel = 0;
        }
    }

    else if (speed_pid.TargetDis < 0)
    {
        if (speed_pid.DisSum < speed_pid.TargetDis)
        {
            speed_pid.DriveMode = move_pwm_control;

            RPM_CMD1 = 0;

            speed_pid.DisSum = 0;
            speed_pid.TargetDis = 0;
            speed_pid.TargetVel = 0;
        }
    }
}
