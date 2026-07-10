#include "AllHeader.h"
#include "pic.h"//图片

int main(void)
{	
	//硬件初始化
	BSP_init();
	
	//基本定时器初始化
	TIM6_Init();//10MS
	TIM7_Init();//1MS
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);//开LED
	
	while(1)
	{		
		Car_Control_Task();
		// 1ms 任务（最高优先级）
		if (flag_1ms) {
				flag_1ms = 0;  // 清除标志0
		
			}

		// 10ms 任务
		if (flag_10ms) {       
				flag_10ms = 0;  // 清除标志
			
				OLED_ShowString(0,0,"Task:",8,0);// 当前任务序列索引
				OLED_ShowString(0,8,"Step:",8,0);// 当前步骤索引
				OLED_ShowString(0,16,"Time:",8,0);// 当前计数索引
				OLED_ShowString(0,24,"Count:",8,0);// 当前按键计数索引
				OLED_ShowString(0,32,"Stored quantity:",8,0);// 当前已存储计数索引
				

				OLED_ShowNum(32,0,currentTaskSequence,2,8,1);
				OLED_ShowNum(32,8,currentStepIndex,2,8,1);
				OLED_ShowNum(32,16,flag_1s,2,8,1);
				OLED_ShowNum(32,24,flag_1s-last_store_time,2,8,1);
				OLED_ShowNum(100,32,key_store_count,2,8,1);
			
				OLED_ShowNum(0,40,key_counts[1],1,8,1);//1
				OLED_ShowNum(14,40,key_counts[2],1,8,0);
				OLED_ShowNum(28,40,key_counts[3],1,8,1);
				OLED_ShowNum(42,40,key_counts[4],1,8,0);
				OLED_ShowNum(56,40,key_counts[5],1,8,1);
				OLED_ShowNum(70,40,key_counts[6],1,8,0);
				OLED_ShowNum(84,40,key_counts[7],1,8,1);
				OLED_ShowNum(98,40,key_counts[8],1,8,0);
				OLED_ShowNum(112,40,key_counts[9],1,8,1);
				
				OLED_ShowSignedNum(64,0,(g_Encoder_M1_Now),10,8,1);
				OLED_ShowSignedNum(64,8,(g_Encoder_M2_Now),10,8,1);
				OLED_ShowSignedNum(64,16,(g_Encoder_M3_Now),10,8,1);
				OLED_ShowSignedNum(64,24,(g_Encoder_M4_Now),10,8,1);
			
				OLED_Refresh();
		}

		// 100ms 任务（最低优先级）
		if (flag_100ms) {
				flag_100ms = 0;  // 清除标志
		}		

	}
	
}


