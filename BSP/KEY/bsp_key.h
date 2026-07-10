#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "AllHeader.h"

//  引脚定义
#define KEY_GPIO_PORT GPIOB
#define KEY_GPIO_CLK  RCC_APB2Periph_GPIOB

#define KEY1_GPIO_PORT GPIOB
#define KEY1_GPIO_PIN  GPIO_Pin_2
#define KEY1_GPIO_CLK  RCC_APB2Periph_GPIOB

#define KEY2_GPIO_PORT GPIOB
#define KEY2_GPIO_PIN  GPIO_Pin_4
#define KEY2_GPIO_CLK  RCC_APB2Periph_GPIOB


// 按键状态，与实际电平相反。
#define KEY_PRESS      1

#define KEY1_PRESS     1
#define KEY2_PRESS     2
#define KEY_RELEASE    0

#define KEY_MODE_ONE_TIME   1
#define KEY_MODE_ALWAYS     0

#define NUM_KEYS 10  // 10个按键

extern uint32_t key_counts[NUM_KEYS];  // 存储按键值
extern uint8_t  current_key;             // 当前按键索引
extern uint32_t last_store_time;        // 上次存储时间
extern uint8_t key_store_count;         // 已存储的按键次数

//extern TaskCommand tasks5;

uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void Key_GPIO_Init(void);
uint8_t Key1_State(uint8_t mode);
uint8_t Key1_Long_Press(uint16_t timeout);

uint8_t Key2_State(uint8_t mode);
uint8_t Key2_Long_Press(uint16_t timeout);

uint8_t Key1_is_Press(void);
uint8_t Key2_is_Press(void);

//Test
uint8_t key_test(void);

// 初始化按键管理器
void KeyManager_ResetCounts(void);
 
// 更新按键状态（由主循环调用）
// encoder_value: 编码器输入值
// cnt_s: 每秒更新的计数器（外部输入）
void KeyManager_Update(int encoder_value, uint32_t cnt_s);
 


#endif

