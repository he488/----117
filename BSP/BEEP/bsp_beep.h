#ifndef __BSP_BEEP_H__
#define __BSP_BEEP_H__

#include "AllHeader.h"

// ==================== 蜂鸣器引脚定义 ====================
// 蜂鸣器使用 PB5，低电平触发（有源蜂鸣器）
// 接线：蜂鸣器 VCC → 3.3V，蜂鸣器 GND → PB5，蜂鸣器 I/O → PB5
// 注意：有源蜂鸣器直接接PB5即可，无源蜂鸣器需要PWM驱动
#define BEEP_PIN        GPIO_Pin_5      // 蜂鸣器控制引脚 → PB5
#define BEEP_PORT       GPIOB           // 蜂鸣器端口
#define BEEP_CLK        RCC_APB2Periph_GPIOB  // 蜂鸣器时钟

// ==================== 蜂鸣器控制宏 ====================
// 有源蜂鸣器：低电平响，高电平不响
#define BEEP_ON()       GPIO_ResetBits(BEEP_PORT, BEEP_PIN)   // PB5输出低电平 → 蜂鸣器响
#define BEEP_OFF()      GPIO_SetBits(BEEP_PORT, BEEP_PIN)     // PB5输出高电平 → 蜂鸣器停

// ==================== 函数声明 ====================
void Beep_GPIO_Init(void);      // 初始化蜂鸣器GPIO
void Beep_Once(void);           // 蜂鸣器响一声（约200ms）
void Beep_On_Time(uint16_t ms); // 蜂鸣器响指定时长（毫秒）
void Beep_N_Times(uint8_t n);   // 蜂鸣器响N次（每次200ms，间隔100ms）

#endif /* __BSP_BEEP_H__ */