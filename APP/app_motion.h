#ifndef __APP_MOTION_H__
#define __APP_MOTION_H__

#include "stdint.h"
#include "AllHeader.h"

// 205RPM电机，轮子转一整圈，编码器获得的脉冲数=减速比*码盘线数*编码器脉冲（56*11*4）
#define ENCODER_CIRCLE_205           (2464.0f)

// 330RPM电机，轮子转一整圈，编码器获得的脉冲数=减速比*码盘线数*编码器脉冲（30*11*4）
#define ENCODER_CIRCLE_330           (1320.0f)

// 550RPM电机，轮子转一整圈，编码器获得的脉冲数=减速比*码盘线数*编码器脉冲（19*11*4）
#define ENCODER_CIRCLE_550           (836.0f)

// 轮子转一整圈的位移，单位为米
#define DISTANCE_CIRCLE      (0.204203)


// 停止模式，STOP_FREE表示自由停止，STOP_BRAKE表示刹车。
typedef enum _stop_mode {
    STOP_FREE = 0,
    STOP_BRAKE
} stop_mode_t;


typedef enum _motion_state {
    MOTION_STOP = 0,
    MOTION_RUN,
    MOTION_BACK,
    MOTION_LEFT,
    MOTION_RIGHT,
    MOTION_SPIN_LEFT,
    MOTION_SPIN_RIGHT,
    MOTION_BRAKE,

    MOTION_MAX_STATE
} motion_state_t;


typedef struct _car_data
{
    int16_t Vx;
    int16_t Vy;
    int16_t Vz;
} car_data_t;


typedef enum _car_type
{
    CAR_TYPE_NONE = 0x00,       // 保留
    CAR_MECANUM = 0x01,         // 小架构小麦轮 X3
    CAR_MECANUM_MAX = 0x02,     // 大架构大麦轮 X3 PLUS
    CAR_MECANUM_MINI = 0x03,    // 大架构小麦轮 无
    CAR_FOURWHEEL = 0x04,       // 四轮普通小车 X1
    CAR_ACKERMAN = 0x05,        // 阿克曼小车  R2

    CAR_TYPE_MAX                // 最后一个小车类型，仅作为判断
} car_type_t;


// 电机编号
typedef enum {
    MOTOR_M1 = 0,  // 左前
    MOTOR_M2,      // 左后
    MOTOR_M3,      // 右前
    MOTOR_M4       // 右后
} MotorID;
 
// 任务命令
typedef struct {
    uint8_t command;       // 1=前进, 2=左转, 3=右转, 4=停止
    int16_t pwm;           // 基础 PWM 值
    int32_t encoder_target; // 目标编码器值（脉冲数）
} TaskCommand;
 
// 全局变量声明
extern int32_t	current_encoder_values[4]; 		// M1, M2, M3, M4 的当前编码器值

extern TaskCommand tasks5[];

extern volatile uint8_t currentTaskSequence;          // 当前任务序列索引，0: tasks1, 1: tasks2, ...，2: tasks5
extern volatile uint8_t currentStepIndex;             // 当前步骤索引
 
// 函数声明
//void Motion_Set_Speed(int16_t m1, int16_t m2, int16_t m3, int16_t m4);
//int16_t Update_Motor_PWM(MotorID motor_id, int16_t current_pwm);
//void Car_Control_Task(void);

void Motion_Stop(uint8_t brake);
void Motion_Set_Pwm(int16_t Motor_1, int16_t Motor_2, int16_t Motor_3, int16_t Motor_4);
//void Motion_Ctrl(int16_t V_x, int16_t V_y, int16_t V_z, uint8_t adjust);
//void Motion_Ctrl_State(uint8_t state, uint16_t speed, uint8_t adjust);


void Motion_Get_Encoder(void);
void Motion_Set_Speed(int16_t speed_m1, int16_t speed_m2, int16_t speed_m3, int16_t speed_m4);


void Motion_Handle(void);

void Motion_Get_Speed(car_data_t* car);


void Motion_Set_Yaw_Adjust(uint8_t adjust);
uint8_t Motion_Get_Yaw_Adjust(void);

uint8_t Motion_Get_Car_Type(void);
void Motion_Set_Car_Type(car_type_t car_type);
float Motion_Get_Circle_MM(void);
float Motion_Get_APB(void);

void Motion_Get_Motor_Speed(float* speed);


void Motion_Send_Data(void);
void Motion_Send_Car_Type(void);

void Car_Control_Task(void);

#endif

