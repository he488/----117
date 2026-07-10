#ifndef __HC595_H
#define __HC595_H 

#include "AllHeader.h"


#define SER_Pin GPIO_Pin_12
#define SCK_Pin GPIO_Pin_13
#define RCLK_Pin GPIO_Pin_14

#define SER_L GPIO_ResetBits(GPIOA,SER_Pin)//SER
#define SER_H GPIO_SetBits(GPIOA,SER_Pin)

#define SCK_L GPIO_ResetBits(GPIOA,SCK_Pin)//SCK
#define SCK_H GPIO_SetBits(GPIOA,SCK_Pin)

#define RCLK_L GPIO_ResetBits(GPIOA,RCLK_Pin)//RCLK
#define RCLK_H GPIO_SetBits(GPIOA,RCLK_Pin)

void HC595_Init(void);
void LED_Display_4(float out_num,unsigned char decimal_places);

#endif

