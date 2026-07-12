#ifndef __BSP_ENCODER_H__
#define __BSP_ENCODER_H__
#include "AllHeader.h"

//R2  L2
#define HAL_1A_PIN GPIO_Pin_0
#define HAL_1A_PORT GPIOA
#define HAL_1A_CLK RCC_APB2Periph_GPIOA
#define HAL_1B_PIN GPIO_Pin_1
#define HAL_1B_PORT GPIOA
#define HAL_1B_CLK RCC_APB2Periph_GPIOA

// ★ 编码器2（对应电机M2 / L2）→ 互换后使用原编码器3的引脚
//   通道A: GPIOA Pin15 (TIM2 CH1, 重映射)
//   通道B: GPIOB Pin3  (TIM2 CH2, 重映射)
#define HAL_2A_PIN GPIO_Pin_15
#define HAL_2A_PORT GPIOA
#define HAL_2A_CLK RCC_APB2Periph_GPIOA
#define HAL_2B_PIN GPIO_Pin_3
#define HAL_2B_PORT GPIOB
#define HAL_2B_CLK RCC_APB2Periph_GPIOB
// ★ 编码器3（对应电机M1 / L1）→ 互换后使用原编码器2的引脚
//   通道A: GPIOA Pin7  (TIM3 CH2)
//   通道B: GPIOA Pin6  (TIM3 CH1)
#define HAL_3A_PIN GPIO_Pin_7
#define HAL_3A_PORT GPIOA
#define HAL_3A_CLK RCC_APB2Periph_GPIOA
#define HAL_3B_PIN GPIO_Pin_6
#define HAL_3B_PORT GPIOA
#define HAL_3B_CLK RCC_APB2Periph_GPIOA
//L1	R2
#define HAL_4A_PIN GPIO_Pin_7
#define HAL_4A_PORT GPIOB
#define HAL_4A_CLK RCC_APB2Periph_GPIOB
#define HAL_4B_PIN GPIO_Pin_6
#define HAL_4B_PORT GPIOB
#define HAL_4B_CLK RCC_APB2Periph_GPIOB


// 不可大于65535 因为F103的定时器是16位的。
#define ENCODER_TIM_PERIOD       (uint16_t)(65535)


extern int g_Encoder_M1_Now;
extern int g_Encoder_M2_Now;
extern int g_Encoder_M3_Now;
extern int g_Encoder_M4_Now;

void Encoder_Init(void);

int Encoder_Get_Count_Now(uint8_t Motor_id);
void Encoder_Update_Count(void);
void Encoder_Get_ALL(int32_t* Encoder_all);

void Encoder_Send_Count_Now(void);

#endif