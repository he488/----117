#include "bsp_timer.h"

// 标志位（volatile 防止编译器优化）
volatile uint8_t  flag_1ms = 0;
volatile uint8_t  flag_10ms = 0;
volatile uint8_t  flag_100ms = 0;
volatile uint16_t flag_1s = 0;

/**************************************************************************
函数功能：TIM6初始化，定时1毫秒
入口参数：无
返回  值：无
**************************************************************************/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //使能定时器的时钟
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;			 // 预分频器
	TIM_TimeBaseStructure.TIM_Period = 99;				 //设定计数器自动重装值
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);                //清除TIM的更新标志位
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;			  //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);							  //初始化NVIC寄存器

	TIM_Cmd(TIM6, ENABLE);
}

void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //检查TIM更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //清除TIMx更新中断标志
		Encoder_Update_Count();
		Encoder_Get_ALL(current_encoder_values); 			 // 更新对应电机的编码器值
//		Motion_Handle();
	}
}

/**************************************************************************
函数功能：TIM7初始化，定时1ms
入口参数：无
返回  值：无
**************************************************************************/
void TIM7_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //使能定时器的时钟
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;			 // 预分频器
	TIM_TimeBaseStructure.TIM_Period = 9;				 //设定计数器自动重装值0
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);               //清除TIM的更新标志位
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;			  //TIM中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);							  //初始化NVIC寄存器

	TIM_Cmd(TIM7, ENABLE);
}


// TIM7中断
void TIM7_IRQHandler(void)
{
	 static uint16_t cnt_10ms = 0;
   static uint16_t cnt_100ms = 0;
	 static uint16_t cnt_1s = 0;
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) //检查TIM更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);    //清除TIMx更新中断标志
		flag_1ms = 1;  // 每 1ms 置位

			// 10ms 标志
			if (++cnt_10ms >= 10) {
					cnt_10ms = 0;
					flag_10ms = 1;
			}

			// 100ms 标志
			if (++cnt_100ms >= 100) {
					cnt_100ms = 0;
					flag_100ms = 1;
			}
			// 1s 标志
			if (++cnt_1s >= 1000) {
					cnt_1s = 0;
					flag_1s ++;
					if(flag_1s>=60000)
					{
						flag_1s=0;
					}
			}
	}
}
