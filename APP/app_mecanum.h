#ifndef __APP_MECANUM_H__
#define __APP_MECANUM_H__

#include "stdint.h"
#include "AllHeader.h"

// 四轮车底盘间距
#define ROBOT_WIDTH                  (169.0f)    // 毫米
#define ROBOT_LENGTH                 (160.11f)   // 毫米

// 四轮底盘电机间距之和的一半
// #define MECANUM_APB               ((ROBOT_WIDTH + ROBOT_LENGTH)/2.0f)
#define MECANUM_APB                  (164.555f)

// 四轮转一整圈的位移，单位为mm
#define MECANUM_CIRCLE_MM            (204.203f)
#define MECANUM_MINI_CIRCLE_MM       (204.203f)


// 四轮底盘电机间距之和的一半
#define MECANUM_MAX_APB              (214.1f)
// 四轮转一整圈的位移，单位为MM
#define MECANUM_MAX_CIRCLE_MM        (251.327f)

// 四轮大底盘电机间距之和的一半
#define MECANUM_MINI_APB             (174.5f)


// X3 PLUS小车速度限制
#define CAR_X3_PLUS_MAX_SPEED        (700)




void Mecanum_Ctrl(int16_t V_x, int16_t V_y, int16_t V_z, uint8_t adjust);
void Mecanum_State(uint8_t state, uint16_t speed, uint8_t adjust);

void Mecanum_Yaw_Calc(float yaw);

#endif /* __APP_MECANUM_H__ */

