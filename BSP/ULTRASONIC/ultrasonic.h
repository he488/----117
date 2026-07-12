#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "AllHeader.h"

/*超声波传感器 — 引脚定义*/
// TRIG（触发信号）→ PA12（推挽输出，发送10us以上高电平触发测距）
// ECHO（回响信号）→ PD2（浮空输入，接收高电平脉冲，宽度对应距离）
#define TRIG_RCC		RCC_APB2Periph_GPIOA
#define ECHO_RCC		RCC_APB2Periph_GPIOD

#define TRIG_PIN		GPIO_Pin_12
#define ECHO_PIN		GPIO_Pin_2

#define TRIG_PORT		GPIOA
#define ECHO_PORT		GPIOD

void Ultrasonic_GPIO_Init(void);
void Ultrasonic_servo_mode(void);

// 使用软件计时（不占用定时器），避免与编码器TIM2冲突
extern float GetUltrasonicDistance(void);
// 不调用 Ultrasonic_Timer2_Init()，改用软件计时




#endif