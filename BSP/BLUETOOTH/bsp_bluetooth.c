#include "bsp_bluetooth.h"

/*============================================================================
 * Bluetooth_Init() - 初始化 USART1 与 HC-06 通信
 *
 * 使用 USART1（PA9-TX，PA10-RX），波特率 9600（HC-06 默认）
 *============================================================================*/
void Bluetooth_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 开启时钟：USART1（APB2）、GPIOA（APB2） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* 配置 PA9 为 TX（推挽复用输出） */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置 PA10 为 RX（浮空输入） */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 参数配置：9600-8-N-1 */
    USART_InitStructure.USART_BaudRate = BLUETOOTH_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* 使能 USART1 接收中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* 配置 USART1 中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 使能 USART1 */
    USART_Cmd(USART1, ENABLE);
}

void Bluetooth_SendByte(uint8_t ch)
{
    USART_SendData(USART1, ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void Bluetooth_SendString(char *str)
{
    while (*str)
    {
        Bluetooth_SendByte((uint8_t)(*str));
        str++;
    }
}

void Bluetooth_SendArray(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        Bluetooth_SendByte(buf[i]);
    }
}

uint8_t Bluetooth_GetByte(uint8_t *ch)
{
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        *ch = USART_ReceiveData(USART1);
        return 1;
    }
    return 0;
}

#if BLUETOOTH_SWITCH && !DEBUG_SWITCH
void USART1_IRQHandler(void)
{
    uint8_t data;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);
        /* 蓝牙接收数据处理回调 */
    }
}
#endif