/**********************************************************************************************************************
 * \file EncMotor.c
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
#include "Homo_Coordinate.h"

/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/


/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
float32 cam_points[4][2];
float32 world_points[4][2];
int data_ready_flag=0;
int transform_finished=0;
/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
    float32 H[3][3]={    //변환행렬
            {0, 0},
            {0, 0},
            {0, 0}
    };
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/

// 호모그래피 행렬 H를 사용하여 여러 개의 카메라 좌표를 일괄 변환
void transform_points(float32 H[3][3], float32 cam_points[4][2], float32 world_points[4][2]) {
    transform_finished=0;
    for (int i = 0; i < 4; i++) {
        float32 x = cam_points[i][0];  // 카메라 픽셀 x좌표
        float32 y = cam_points[i][1];  // 카메라 픽셀 y좌표

        // 동차좌표 변환
        float32 a = H[0][0] * x + H[0][1] * y + H[0][2];
        float32 b = H[1][0] * x + H[1][1] * y + H[1][2];
        float32 c = H[2][0] * x + H[2][1] * y + H[2][2];

        // 정규화
        world_points[i][0] = a / c;
        world_points[i][1] = b / c;
    }
    transform_finished=1;
};

void TransformData_Init(void){
    cam_points[0][0]=0;
    cam_points[0][1]=0;
    cam_points[1][0]=0;
    cam_points[1][1]=0;

    cam_points[2][0]=0;
    cam_points[2][1]=0;
    cam_points[3][0]=0;
    cam_points[3][1]=0;

    world_points[0][0]=0;
    world_points[0][1]=0;
    world_points[1][0]=0;
    world_points[1][1]=0;

    world_points[2][0]=0;
    world_points[2][1]=0;
    world_points[3][0]=0;
    world_points[3][1]=0;
};

void InitCampoints(void){
    for (int i = 0; i < 4; i++) {
        cam_points[i][0] = 0;
        cam_points[i][1] = 0;
    }
}

void InitWorldpoints(void){
    for (int i = 0; i < 4; i++) {
        world_points[i][0] = 0;
        world_points[i][1] = 0;
    }
}


/*********************************************************************************************************************/
