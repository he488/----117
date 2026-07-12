#ifndef __AllHeader_H
#define __AllHeader_H

#define DEBUG_USARTx USART1



//功能开关  0:关闭 1:打开
#define LCD_SWITCH    0
#define OLED_SWITCH		1
#define HC595_SWITCH  0   // 数码管（关闭，释放PA12给超声波TRIG使用）
#define IMU_SWITCH 		0 
#define FLASH_SWITCH  0 
#define RGB_SWITCH		0
#define MOTOR_SWITCH  1
#define DEBUG_SWITCH  0

#define ELE_SWITCH 		0
#define CCD_SWITCH    0

#define  ENABLE_REAL_WHEEL 1

#define Ultrasonic_SWITCH  1
#define IRAvoid_SWITCH  0

#define three_LineWalking_SWITCH  0  // 三路巡线传感器（关闭，与四路巡线互斥）
#define LineWalking_SWITCH  1    // 四路巡线传感器（打开！使用PA2/PA3/PA4/PA5）
#define PS2_SWITCH 0           // PS2手柄遥控（关闭，与四路巡线互斥）
#define BEEP_SWITCH 0          // 蜂鸣器（关闭，需要时设为1打开）

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "stdint.h"
#include "stm32f10x.h"

//#include "stm32f10x_gpio.h"
//#include "stm32f10x_pwr.h"
//#include "stm32f10x_bkp.h"

#include "bsp.h"
#include "bsp_common.h"
#include "delay.h"

#include "bsp_key.h"

#include "bsp_mpu6050.h"  // MPU6050 六轴传感器
#include "IOI2C.h"

#include "lcd.h"
#include "lcd_init.h"

#include "bsp_spi.h"
#include "bsp_w25q64.h"

#include "bsp_RGB.h"
#include "bsp_usart.h"

#include "bsp_adc.h"
#include "bsp_CCD.h"

#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "app_motion.h"
#include "bsp_pid.h"
#include "app_mecanum.h"

#include "linewalking.h"
#include "iravoid.h"
#include "three_linewalking.h"
#include "ps2_control.h"

#include "oled.h"
#include "bsp_timer.h"
#include "bsp_beep.h"

//APP文件下的函数
/********外部flash相关*********/
u8 detcet_flash(void);
void flash_test(void);
void show_flash(void);

/**********IMU相关************/
void imu_test(void);



#endif