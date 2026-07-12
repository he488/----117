#ifndef __LINEWALKING_H__
#define __LINEWALKING_H__	

/*

	从车身后面往前看： 左侧到右边巡线传感器顺序为  L1 L2 |黑线| R1  R2	  对应原理图 IN2 IN1 |黑线| IN3 IN4

*/


// 使能4路巡线传感器，用于控制小车循迹行驶
#define USE_LINE_L1
#define USE_LINE_L2
#define USE_LINE_R1
#define USE_LINE_R2

// 巡线传感器引脚时钟使能（PA2~PA5都挂在GPIOA总线上）
#define LineWalk_L1_RCC		RCC_APB2Periph_GPIOA
#define LineWalk_L2_RCC		RCC_APB2Periph_GPIOA
#define LineWalk_R1_RCC    	RCC_APB2Periph_GPIOA
#define LineWalk_R2_RCC		RCC_APB2Periph_GPIOA

// 巡线传感器引脚号定义：从左到右依次为 L1 L2 |黑线| R1 R2
// 对应原理图：IN2 IN1 |黑线| IN3 IN4
#define LineWalk_L1_PIN		GPIO_Pin_2   // 左侧第1个巡线传感器 → PA2（红外检测输入）
#define LineWalk_L2_PIN		GPIO_Pin_3   // 左侧第2个巡线传感器 → PA3（红外检测输入）
#define LineWalk_R1_PIN		GPIO_Pin_4   // 右侧第1个巡线传感器 → PA4（红外检测输入）
#define LineWalk_R2_PIN		GPIO_Pin_5   // 右侧第2个巡线传感器 → PA5（红外检测输入）

// 巡线传感器端口定义（全部使用GPIOA端口）
#define LineWalk_L1_PORT	GPIOA
#define LineWalk_L2_PORT	GPIOA
#define LineWalk_R1_PORT	GPIOA
#define LineWalk_R2_PORT	GPIOA


void LineWalking_GPIO_Init(void);
void GetLineWalking(int *p_iL1, int *p_iL2, int *p_iR1, int *p_iR2);
void LineWalking(void);




#endif