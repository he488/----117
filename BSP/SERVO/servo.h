#ifndef __SERVO_H
#define	__SERVO_H

#include "stm32f10x.h"

extern int Angle_J1;

/*定义需要初始化的舵机宏定义开关*/
#define USE_SERVO_J1		0
#define USE_SERVO_J2		0
#define USE_SERVO_J3		0

#define Servo_J1_PIN	GPIO_Pin_7
#define Servo_J1_PORT	GPIOB
#define Servo_J1_RCC	RCC_APB2Periph_GPIOB

#define Servo_J2_PIN	GPIO_Pin_6
#define Servo_J2_PORT	GPIOB
#define Servo_J2_RCC	RCC_APB2Periph_GPIOB

#define Servo_J3_PIN	GPIO_Pin_6
#define Servo_J3_PORT	GPIOB
#define Servo_J3_RCC	RCC_APB2Periph_GPIOB

void Servo_J1(int v_iAngle);/*定义一个脉冲函数，用来模拟方式产生PWM值*/
void Servo_GPIO_Init(void);

void front_detection(void);
void left_detection(void);
void right_detection(void);

void TIM1_Int_Init(u16 arr,u16 psc);


#endif
