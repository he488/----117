#ifndef __BSP_K210_H__
#define __BSP_K210_H__

#include "AllHeader.h"

/*============================================================================
 * K210 视觉模块 - 串口通信（USART3，部分重映射）
 *
 * 通信方式：USART3 硬件串口（波特率 115200）
 *
 *   引脚定义：
 *     K210_TX（K210 发送 → STM32 接收）→  GPIOC Pin11  （USART3 RX，部分重映射）
 *     K210_RX（K210 接收 ← STM32 发送）→  GPIOC Pin10  （USART3 TX，部分重映射）
 *
 *   USART3 重映射说明：
 *     STM32F103RC 的 USART3 默认引脚为 PB10（TX）、PB11（RX），
 *     但 PB10/PB11 已被 MPU6050 的 I2C 占用，所以使用部分重映射
 *     （GPIO_PartialRemap_USART3）将 TX/RX 重映射到 PC10/PC11。
 *
 *   接线方式：
 *     K210 模块          STM32F103RC
 *     TX（发送）    →    PC11（USART3 RX）
 *     RX（接收）    ←    PC10（USART3 TX）
 *     VCC           →    5V（或 3.3V，视 K210 模块而定）
 *     GND           →    GND
 *
 *   注意：
 *     PC10 和 PC11 原本被 LCD 彩屏占用（BLK 背光、RES 复位），
 *     但 LCD_SWITCH = 0（LCD 已关闭），所以这两个引脚空闲可用。
 *     如果以后要打开 LCD，需要关闭 K210 或改用其他串口。
 *============================================================================*/

/* USART3 波特率 */
#define K210_BAUDRATE       115200

/* 接收缓冲区大小（字节） */
#define K210_RX_BUF_SIZE    128

/* 发送缓冲区大小（字节） */
#define K210_TX_BUF_SIZE    64

/* 函数声明 */
void K210_Init(void);                           // 初始化 K210 串口
void K210_SendByte(uint8_t ch);                 // 发送一个字节
void K210_SendArray(uint8_t *buf, uint16_t len); // 发送数据数组
uint8_t K210_GetByte(uint8_t *ch);              // 读取一个字节（非阻塞，有数据返回1）
void K210_ClearRxBuf(void);                     // 清空接收缓冲区

/* 外部变量：接收缓冲区 */
extern volatile uint8_t g_k210_rx_buf[K210_RX_BUF_SIZE];
extern volatile uint16_t g_k210_rx_len;

#endif /* __BSP_K210_H__ */