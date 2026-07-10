#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "AllHeader.h"

extern volatile uint8_t  flag_1ms;
extern volatile uint8_t  flag_10ms;
extern volatile uint8_t  flag_100ms;
extern volatile uint16_t flag_1s;

void TIM6_Init(void);
void TIM7_Init(void);

#endif
