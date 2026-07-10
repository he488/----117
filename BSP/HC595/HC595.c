#include "HC595.h"

// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F 
unsigned char number[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf};

/****************************************
函数名称：void HC595_Init(void)
函数参数：无参
函数返回值：无
函数功能：IO的基本配置初始化
****************************************/
 
void HC595_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //完全禁用SW+JTAG，PA13、PA14、PA15、PB3、PB4可用	

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = SER_Pin | SCK_Pin | RCLK_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
}
 
 
/*
8位数据输入595移位寄存器
data595：待输入的数据。
*/
void LED_OUT(unsigned char data595)
{
	unsigned char i;
	for(i = 8;i >= 1;i--)
	{
		if(data595 & 0x80) SER_H;
		else SER_L;
		data595 <<= 1;
		
		SCK_L;
		delay_us(1);
		SCK_H;
	}
}
/*
浮点数四位截取
out_num：要截取的浮点数。
decimal_places：保留的小数位数。
Display_num：输出数据按位存储。
*/
int float_cutting(float out_num,unsigned char decimal_places,unsigned char* Display_num)
{
	if(decimal_places > 4) return 0;
	for(;decimal_places > 0;decimal_places--) out_num *= 10;
	*Display_num++ = (long int)out_num%10;
	*Display_num++ = (long int)out_num/10%10;
	*Display_num++ = (long int)out_num/100%10;
	*Display_num = (long int)out_num/1000%10;
	return 1;
}
/*
熄灭所有位
*/
void Display_off()
{
	LED_OUT(0xff);
	LED_OUT(0xff);
	RCLK_L;
	delay_us(1);
	RCLK_H;
}
/*
显示一位数字
num：要输出的数字。
place：输出数字的位置。
pointYN：是否带小数点，1：带小数点，0：不带小数点。
*/
void LED_Display_1(unsigned char num,unsigned char place,unsigned char pointYN)
{
	if(pointYN == 1)
	{
		LED_OUT(number[num]&0x7f);
		LED_OUT(0x01<<place);
		RCLK_L;
		delay_us(1);
		RCLK_H;
	}
	else
	{
		LED_OUT(number[num]);
		LED_OUT(0x01<<place);
		RCLK_L;
		delay_us(1);
		RCLK_H;
	}
}
/*
显示浮点数
out_num：要显示的数字。
decimal_places：小数位数，取值0~4。
*/
void LED_Display_4(float out_num,unsigned char decimal_places)
{
	unsigned char Display_num[4];
	unsigned char i = 0;
	Display_off();
	float_cutting(out_num,decimal_places,Display_num);
	for(i = 0;i < 4;i++)
	{
		if(i == decimal_places) LED_Display_1(Display_num[i],i,1);
		else LED_Display_1(Display_num[i],i,0);
	}
}

