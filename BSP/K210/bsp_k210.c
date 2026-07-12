#include "bsp_k210.h"

/* 接收缓冲区（中断中填充） */
volatile uint8_t g_k210_rx_buf[K210_RX_BUF_SIZE];
volatile uint16_t g_k210_rx_len = 0;
static uint16_t g_k210_rx_index = 0;

/*============================================================================
 * K210_Init() - 初始化 USART3 与 K210 通信
 *
 * 使用 USART3 部分重映射（GPIO_PartialRemap_USART3）：
 *   TX（发送）→ PC10
 *   RX（接收）→ PC11
 *
 * 配置步骤：
 *   1. 开启 USART3、GPIOC、AFIO 时钟
 *   2. 使能 USART3 部分重映射
 *   3. 配置 PC10 为推挽复用输出（TX）
 *   4. 配置 PC11 为浮空输入（RX）
 *   5. 配置 USART3 参数（波特率、数据位、停止位等）
 *   6. 使能 USART3 接收中断
 *   7. 使能 USART3
 *============================================================================*/
void K210_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 开启时钟：USART3（APB1）、GPIOC（APB2）、AFIO（APB2，用于重映射） */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

    /* USART3 部分重映射：TX→PC10，RX→PC11 */
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);

    /* 配置 PC10 为 TX（推挽复用输出） */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置 PC11 为 RX（浮空输入） */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* USART3 参数配置：115200-8-N-1 */
    USART_InitStructure.USART_BaudRate = K210_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    /* 使能 USART3 接收中断（接收一个字节即触发） */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    /* 配置 USART3 中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 使能 USART3 */
    USART_Cmd(USART3, ENABLE);

    /* 清空接收缓冲区 */
    K210_ClearRxBuf();
}

/*============================================================================
 * K210_SendByte() - 通过 USART3 发送一个字节到 K210
 *============================================================================*/
void K210_SendByte(uint8_t ch)
{
    /* 等待发送数据寄存器空 */
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART3, ch);
}

/*============================================================================
 * K210_SendArray() - 通过 USART3 发送数据数组到 K210
 *============================================================================*/
void K210_SendArray(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        K210_SendByte(buf[i]);
    }
}

/*============================================================================
 * K210_GetByte() - 从接收缓冲区读取一个字节（非阻塞）
 *
 * 返回值：1 = 成功读取到数据，0 = 缓冲区无数据
 *============================================================================*/
uint8_t K210_GetByte(uint8_t *ch)
{
    if (g_k210_rx_len > 0)
    {
        *ch = g_k210_rx_buf[0];
        /* 将后续数据往前移 */
        for (uint16_t i = 0; i < g_k210_rx_len - 1; i++)
        {
            g_k210_rx_buf[i] = g_k210_rx_buf[i + 1];
        }
        g_k210_rx_len--;
        g_k210_rx_index--;
        return 1;
    }
    return 0;
}

/*============================================================================
 * K210_ClearRxBuf() - 清空接收缓冲区
 *============================================================================*/
void K210_ClearRxBuf(void)
{
    g_k210_rx_len = 0;
    g_k210_rx_index = 0;
    for (uint16_t i = 0; i < K210_RX_BUF_SIZE; i++)
    {
        g_k210_rx_buf[i] = 0;
    }
}

/*============================================================================
 * USART3_IRQHandler() - USART3 中断服务函数
 *
 * 当 K210 发送数据到 STM32 时，触发此中断，
 * 将接收到的数据存入环形缓冲区。
 *============================================================================*/
void USART3_IRQHandler(void)
{
    uint8_t data;

    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        /* 读取接收到的数据 */
        data = USART_ReceiveData(USART3);

        /* 存入缓冲区（如果缓冲区未满） */
        if (g_k210_rx_index < K210_RX_BUF_SIZE)
        {
            g_k210_rx_buf[g_k210_rx_index++] = data;
            g_k210_rx_len++;
        }
        /* 如果缓冲区已满，丢弃新数据（防止溢出） */
    }
}