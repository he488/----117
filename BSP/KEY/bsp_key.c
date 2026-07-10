#include "bsp_key.h"
uint16_t g_key1_long_press = 0;

// 全局变量
uint32_t key_counts[NUM_KEYS] = {0};  // 存储按键值
uint8_t current_key = 0;             // 当前按键索引
uint32_t last_store_time = 0;        // 上次存储时间
uint8_t key_store_count = 0;         // 已存储的按键次数

uint8_t key_test(void) //Test
{
	if (!GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN))
	{
		return KEY1_PRESS; // 如果按键1被按下，则返回KEY1_PRESS
	}
	else if(!GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN))
	{
		return KEY2_PRESS;  // 如果按键2被按下，则返回KEY2_PRESS
	}
	return KEY_RELEASE;   // 如果按键是松开状态，则返回KEY_RELEASE

}




// 判断按键是否被按下，按下返回KEY_PRESS，松开返回KEY_RELEASE
uint8_t Key1_is_Press(void)
{
	if (!GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN))
	{
		return KEY1_PRESS; // 如果按键被按下，则返回KEY_PRESS
	}
	return KEY_RELEASE;   // 如果按键是松开状态，则返回KEY_RELEASE
}

uint8_t Key2_is_Press(void)
{
	if (!GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN))
	{
		return KEY2_PRESS; // 如果按键被按下，则返回KEY_PRESS
	}
	return KEY_RELEASE;   // 如果按键是松开状态，则返回KEY_RELEASE
}

// 阻塞式扫描按键，按下返回1，松开返回0
uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	/*检测是否有按键按下 */
	if (!GPIO_ReadInputDataBit(GPIOx, GPIO_Pin))
	{
		/*等待按键释放 */
		while (!GPIO_ReadInputDataBit(GPIOx, GPIO_Pin))
			;
		return KEY1_PRESS;
	}
	else
		return KEY_RELEASE;
}

// 读取按键K1的长按状态，累计达到长按时间返回1，未达到返回0.
// timeout为设置时间长度，单位为秒
uint8_t Key1_Long_Press(uint16_t timeout)
{
	if (g_key1_long_press > 0)
	{
		if (g_key1_long_press < timeout * 100 + 2)
		{
			g_key1_long_press++;
			if (g_key1_long_press == timeout * 100 + 2)
			{
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

// 读取按键K1的状态，按下返回1，松开返回0.
// mode:设置模式，0：按下一直返回1；1：按下只返回一次1
uint8_t Key1_State(uint8_t mode)
{
	static uint16_t key1_state = 0;

	if (Key1_is_Press() == KEY1_PRESS)
	{
		if (key1_state < (mode + 1) * 2)
		{
			key1_state++;
		}
	}
	else
	{
		key1_state = 0;
		g_key1_long_press = 0;
	}
	if (key1_state == 2)
	{
		g_key1_long_press = 1;
		return KEY1_PRESS;
	}
	return KEY_RELEASE;
}

// 初始化按键
void Key_GPIO_Init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启按键端口的时钟*/
	RCC_APB2PeriphClockCmd(KEY_GPIO_CLK, ENABLE);
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN|KEY2_GPIO_PIN;
	//设置按键的引脚为上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//使用结构体初始化按键
	GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}



 
// 编码器值转按键值
static int EncoderToKeyValue(int encoder_value) {
    if (encoder_value > 4000) return 3;
    if (encoder_value > 1500) return 2;
    return 1;
}

 
// 预定义的配置映射（按键值1/2/3对应不同的配置）
static const TaskCommand config_map[] = {
    {1, 100, 4090},   // 按键1 → {1, 100, 4090}
    {2, 100, 512},    // 按键2 → {2, 100, 512}
    {3, 500, 512},    // 按键3 → {3, 500, 512}
};
 
// 复位所有按键计数
void KeyManager_ResetCounts() {
    for (int i = 0; i < NUM_KEYS; i++) {
        key_counts[i] = 0;
    }
    key_store_count = 0;
}
 
// 更新按键管理器		g_Encoder_M1_Now		flag_1s
void KeyManager_Update(int encoder_value, uint32_t cnt_s) {
    int key_value = EncoderToKeyValue(encoder_value);
 
    // 每3秒存储一次按键值，并切换到下一个按键
    if ((cnt_s - last_store_time) >= 3) {
        key_counts[current_key] += key_value;  // 存储当前按键值
        current_key = (current_key + 1) % NUM_KEYS;  // 切换到下一个按键
				flag_1s=0;
        last_store_time = cnt_s;  // 更新存储时间
        key_store_count++;        // 增加存储计数
				g_Encoder_M1_Now = 0;
				g_Encoder_M2_Now = 0;
				g_Encoder_M3_Now = 0;
				g_Encoder_M4_Now = 0;
    }
 
    // 在 if 之外独立判断是否完成第10次存储
    if (key_store_count >= NUM_KEYS) {
        // 遍历所有按键值（从 key_counts[1] 到 key_counts[9]）
        for (int i = 1; i < NUM_KEYS; i++) {
            uint8_t stored_key_value = key_counts[i];  // 获取存储的按键值（1/2/3）
 
            // 根据按键值查找对应的配置
            for (int j = 0; j < sizeof(config_map) / sizeof(config_map[0]); j++) {
                if (config_map[j].command == stored_key_value) {
                    // 存储到二维数组（索引从1开始）
                    tasks5[i] = config_map[j];
                    break;
                }
            }
        }
				g_Encoder_M1_Now = 0;
				g_Encoder_M2_Now = 0;
				g_Encoder_M3_Now = 0;
				g_Encoder_M4_Now = 0;
    }
}

/*********************************************END OF FILE**********************/
