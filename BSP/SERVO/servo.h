#ifndef __SERVO_H
#define	__SERVO_H

#include "stm32f10x.h"

extern int Angle_J1;

/*定义需要初始化的舵机宏定义开关*/
#define USE_SERVO_J1		0
#define USE_SERVO_J2		0
#define USE_SERVO_J3		0

/* ★ 舵机引脚定义：PC0（J1）、PC1（J2）、PC2（J3）
 * 原 PB6/PB7 被编码器4占用，改用 PC0/PC1/PC2（RGB_SWITCH=0，空闲可用）
 * 软件 PWM 控制，50Hz，1~2ms 脉宽 = 0°~180°
 */
#define Servo_J1_PIN	GPIO_Pin_0
#define Servo_J1_PORT	GPIOC
#define Servo_J1_RCC	RCC_APB2Periph_GPIOC

#define Servo_J2_PIN	GPIO_Pin_1
#define Servo_J2_PORT	GPIOC
#define Servo_J2_RCC	RCC_APB2Periph_GPIOC

#define Servo_J3_PIN	GPIO_Pin_2
#define Servo_J3_PORT	GPIOC
#define Servo_J3_RCC	RCC_APB2Periph_GPIOC

void Servo_J1(int v_iAngle);/*定义一个脉冲函数，用来模拟方式产生PWM值*/
void Servo_GPIO_Init(void);

void front_detection(void);
void left_detection(void);
void right_detection(void);

void TIM1_Int_Init(u16 arr,u16 psc);


#endif


