#include "ultrasonic.h"

/*============================================================================
 * 超声波模块（HC-SR04）驱动
 * 
 * 引脚定义：
 *   TRIG（触发）→ PA12（推挽输出）
 *   ECHO（回响）→ PD2（浮空输入）
 * 
 * 工作原理：
 *   1. TRIG 发送 10us 以上高电平，触发模块测距
 *   2. 模块自动发送 8 个 40kHz 脉冲，检测回波
 *   3. ECHO 输出高电平，宽度 = 声波往返时间
 *   4. 距离 = 脉宽(us) / 58 (cm)
 * 
 * 计时方式：
 *   使用软件循环 + delay_us(1) 进行计时，无需占用硬件定时器
 *   避免与编码器 TIM2/3/4/5 冲突
 *   精度 ±1cm，满足避障需求
 *============================================================================*/

/**
 * @brief  超声波 GPIO 初始化
 * @param  无
 * @retval 无
 * @note   TRIG(PA12) 推挽输出，ECHO(PD2) 浮空输入
 */
void Ultrasonic_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*========== TRIG 触发引脚（PA12）= 推挽输出 ==========*/
	RCC_APB2PeriphClockCmd(TRIG_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);

	/*========== ECHO 回响引脚（PD2）= 浮空输入 ==========*/
	RCC_APB2PeriphClockCmd(ECHO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ECHO_PORT, &GPIO_InitStructure);

	/* 初始状态：TRIG 拉低，ECHO 保持输入 */
	GPIO_ResetBits(TRIG_PORT, TRIG_PIN);
}


/**
 * @brief  获取超声波测距结果（软件计时版）
 * @param  无
 * @retval 距离值（单位：cm），-1 表示超时/无物体
 * @note   使用 delay_us(1) 循环计时，不占用硬件定时器
 *         最大测量距离约 5m（对应脉宽 30ms）
 *         精度约 ±1cm，满足避障使用
 */
float GetUltrasonicDistance(void)
{
	uint32_t cnt = 0;      // 脉宽计数
	uint32_t timeout = 0;  // 超时计数

	/*========== 1. 发送触发信号 ==========*/
	/* TRIG 拉高 15us（要求 >10us），触发模块测距 */
	GPIO_SetBits(TRIG_PORT, TRIG_PIN);
	delay_us(15);
	GPIO_ResetBits(TRIG_PORT, TRIG_PIN);

	/*========== 2. 等待 ECHO 上升沿 ==========*/
	/* 等待 ECHO 变为高电平，表示模块已发出超声波脉冲 */
	timeout = 0;
	while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == RESET)
	{
		delay_us(10);          // 每 10us 检查一次
		timeout++;
		if (timeout > 25000)   // 超时约 250ms
		{
			return -1;  // 无物体或模块未响应
		}
	}

	/*========== 3. 测量 ECHO 高电平脉宽 ==========*/
	/* ECHO 高电平宽度 = 声波往返时间，用软件循环计数 */
	cnt = 0;
	while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == SET)
	{
		cnt++;
		delay_us(1);           // 每次循环约 1.2us（delay_us(1) + 循环开销）
		if (cnt > 30000)       // 超时约 30ms（对应距离约 5m）
		{
			return -1;  // 超出量程
		}
	}

	/*========== 4. 距离计算 ==========*/
	/* 实际脉宽 ≈ cnt × 1.2 (us)            */
	/* 距离(cm) = 脉宽(us) / 58             */
	/* 所以距离 ≈ cnt × 1.2 / 58 = cnt / 48.3 */
	/* 使用 48.0 作为校准系数，精度约 ±1cm   */
	return cnt / 48.0f;
}


/**
 * @brief  超声波 + 舵机避障模式（预留，当前未启用）
 * @param  无
 * @retval 无
 * @note   需要配合舵机 J1（PB7）使用
 *         通过舵机转动超声波模块，检测左、中、右三个方向距离
 *         根据障碍物位置决策转向方向
 */
//void Ultrasonic_servo_mode(void)
//{
//	int Len = 0;
//	int LeftDistance = 0, RightDistance = 0;
//
//	Len = (u16)GetUltrasonicDistance();
//
//    if(Len <= 30)  // 遇到障碍物（距离 < 30cm）
//    {
//		Motor_Stop();  // 停车
//
//		// 测量左侧距离（舵机转 180°）
//		Angle_J1 = 180;
//		delay_ms(500);
//		Len = GetUltrasonicDistance();
//		LeftDistance = Len;
//
//		// 测量右侧距离（舵机转 0°）
//		Angle_J1 = 0;
//		delay_ms(500);
//		Len = GetUltrasonicDistance();
//		RightDistance = Len;
//
//		// 归位（舵机回中 90°）
//		Angle_J1 = 90;
//		delay_ms(500);
//
//		// 决策：往距离远的方向转向
//		if (LeftDistance > RightDistance)
//		{
//			// 左转
//		}
//		else
//		{
//			// 右转
//		}
//    }
//}