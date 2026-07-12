#ifndef __BSP_BLUETOOTH_H__
#define __BSP_BLUETOOTH_H__

#include "AllHeader.h"

/*============================================================================
 * HC-06 蓝牙模块 - 串口通信（USART1）
 *
 * 通信方式：USART1 硬件串口（默认波特率 9600，可通过 AT 指令修改）
 *
 *   引脚定义：
 *     HC-06 TX（蓝牙发送 → STM32 接收）→  GPIOA Pin10（USART1 RX）
 *     HC-06 RX（蓝牙接收 ← STM32 发送）→  GPIOA Pin9  （USART1 TX）
 *
 *   注意：
 *     USART1 原本用于调试输出（DEBUG_SWITCH），但 DEBUG_SWITCH=0 已关闭，
 *     所以 USART1 空闲可用。当 DEBUG_SWITCH=1 时不可同时使用蓝牙。
 *============================================================================*/

#define BLUETOOTH_BAUDRATE  9600

void Bluetooth_Init(void);
void Bluetooth_SendByte(uint8_t ch);
void Bluetooth_SendString(char *str);
void Bluetooth_SendArray(uint8_t *buf, uint16_t len);
uint8_t Bluetooth_GetByte(uint8_t *ch);

#endif