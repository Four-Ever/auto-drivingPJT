/**********************************************************************************************************************
 * \file GPIO_LED_Button.c
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
 *
 *********************************************************************************************************************/
/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "LED_Buzzer.h"
#include "Ifx_Types.h"
/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
#define BUZZER                IfxGtm_ATOM1_1_TOUT10_P00_1_OUT
#define BUZZER_PWM_PERIOD            1000000 / 5000
#define BUZZER_CLK_FREQ              1000000.0f
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxGtm_Tom_Pwm_Config g_tomConfig; /* Timer configuration structure                */
IfxGtm_Tom_Pwm_Driver g_tomDriver;
uint32 uPeriod = (10000000 / 329.724);/* Timer Driver structure                       */

IfxGtm_Atom_Pwm_Config g_atom_Buzzer_Config; /* Timer configuration structure                    */
IfxGtm_Atom_Pwm_Driver g_atom_Buzzer_Driver; /* Timer Driver structure                           */

int cnt_50ms = 0;
int period_50ms = 0;
/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

int cnt_alarm = 0;
int alarm_request = 0;
int led = 1;
int buzzer = 1;

/* Function to configure the port pins for the push button and the LED */

void init_LED_Buzzer (void)
{
    IfxPort_setPinModeOutput(LED, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);  //led
    IfxPort_setPinLow(LED);

    initBuzzerGtmATomPwm();
    IfxPort_setPinModeOutput(BUZZER2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);  //led
    IfxPort_setPinLow(BUZZER2);
}

void initBuzzerGtmATomPwm (void)
{
    IfxGtm_enable(&MODULE_GTM); /* Enable GTM */

    //IfxGtm_Cmu_setClkFrequency(&MODULE_GTM, IfxGtm_Cmu_Clk_0, BUZZER_PWM_PERIOD);        /* Set the CMU clock 0 frequency    */
    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_CLK0); /* Enable the CMU clock 0           */

    IfxGtm_Atom_Pwm_initConfig(&g_atom_Buzzer_Config, &MODULE_GTM); /* Initialize default parameters    */

    g_atom_Buzzer_Config.atom = BUZZER.atom; /* Select the ATOM depending on the LED     */
    g_atom_Buzzer_Config.atomChannel = BUZZER.channel; /* Select the channel depending on the LED  */
    g_atom_Buzzer_Config.period = BUZZER_PWM_PERIOD; /* Set timer period                         */
    g_atom_Buzzer_Config.pin.outputPin = &BUZZER; /* Set LED as output                        */
    g_atom_Buzzer_Config.synchronousUpdateEnabled = TRUE; /* Enable synchronous update                */

    g_atom_Buzzer_Config.dutyCycle = BUZZER_PWM_PERIOD;

    IfxGtm_Atom_Pwm_init(&g_atom_Buzzer_Driver, &g_atom_Buzzer_Config); /* Initialize the PWM                       */
    IfxGtm_Atom_Pwm_start(&g_atom_Buzzer_Driver, TRUE); /* Start the PWM                            */
}
void setBuzzerPeriod (int cycle)
{
    g_atom_Buzzer_Config.dutyCycle = cycle;
}

/* Depending on the the state of the "BUTTON" port pin, the LED is turned on or off */
void LED_Buzzer_Blink (void)
{
    if (led == 0)
    {
        IfxPort_setPinLow(LED);
        IfxPort_setPinLow(BUZZER2);

        led = 1;
    }
    else
    {
        IfxPort_setPinHigh(LED);
        IfxPort_setPinHigh(BUZZER2);
        led = 0;
    }
}

void toggle_buzzer (void)
{
    if (buzzer == 0)
    {
        IfxPort_setPinLow(BUZZER2);

        buzzer = 1;
    }
    else
    {
        IfxPort_setPinHigh(BUZZER2);
        buzzer = 0;
    }
}

void Buzzer (void)
{
    if (period_50ms == 0)
    {
        IfxPort_setPinLow(BUZZER2); // turn_off
    }
    else if (period_50ms != 0)
    {
        cnt_50ms++;
        if (cnt_50ms >= period_50ms)
        {
            cnt_50ms = 0;
            toggle_buzzer();
        }
    }
}
void FindCar_Plz (void)
{
    if (alarm_request == 1)
    {
        if (cnt_alarm <= 5)
        {
            LED_Buzzer_Blink();
            //Buzzer();

        }

        else if (cnt_alarm > 5)
        {
            //setDutyCycleB(0);
            //buzzer=0;
            led = 1;
            alarm_request = 0;
            cnt_alarm = 0;
        }
    }
}

void set_Buzzer_period (int R_distance)
{
    // within 2cm : 100ms period
    // within 4cm : 200ms period
    // within 6cm : 300ms period
    // within 8cm : 400ms period
    // within 10cm : 500ms period
    if (R_distance <= 5 && R_distance > 0)    //5cm보다 작으면
    {
        period_50ms = 1;
    }
    else if (period_50ms <= 100) // 10cm
    {
        period_50ms = (int) (R_distance);
    }
    else
    {
        period_50ms = 0;
    }
}

void Obstacle_Alarm (void)
{

}

