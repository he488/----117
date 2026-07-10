#include "app_motion.h"
#include "bsp_key.h"

// === 全局变量定义 ===
int32_t	current_encoder_values[4] = {0};  // M1, M2, M3, M4 的当前编码器值

volatile uint8_t  currentTaskSequence = 0;          // 当前任务序列索引（0: tasks1, 1: tasks2, ...）
volatile uint8_t  currentStepIndex = 0;             // 当前步骤索引
bool taskRunning = false;                // 任务是否正在执行
  
// === 任务序列定义 ===
TaskCommand tasks1[] = {
			{1, 1500,4750},  // 步骤0：前进125CM，PWM 目标编码器
			{3, 1200,1280},  // 步骤1：右转
  		{1, 1500,1900},  // 步骤2：前进50CM，PWM 目标编码器
//     	{2, 1200,1740},  // 步骤3：左转， PWM 目标编码器
//	  	{1, 1500,3450},  // 步骤4：前进100CM，PWM 目标编码器
//  		{2, 1200,1740},   // 步骤5：左转， PWM 目标编码器
//			{1, 1500,3350},  // 步骤6：前进100CM，PWM 目标编码器
//			{3, 1200,1270},   // 步骤7：右转
//			{1, 1500,1300},  // 步骤8：前进50CM，PWM 目标编码器
			{4, 0, 0}       // 步骤9：停止（结束标记）
};
 
 
TaskCommand tasks2[] = {
		{1, 1500,3150}, // 步骤0：前进75CM，PWM 目标编码器
		{2, 1200,1740},  // 步骤1：左转， PWM 目标编码器
    {1, 1500,1300}, // 步骤2：前进50CM，PWM 目标编码器
		{3, 1200,1200},   // 步骤3：右转
		{1, 1500,1500},  // 步骤4：前进50CM，PWM 目标编码器
   	{3, 1200,1200},   // 步骤5：右转
		{1, 1500,1500},  // 步骤6：前进50CM，PWM 目标编码器
		{2, 1200,1760},   // 步骤7：左转， PWM 目标编码器
		{1, 1500,1500},  // 步骤8：前进50CM，PWM 目标编码器
		{2, 1200,1740},   // 步骤9：左转， PWM 目标编码器
		{1, 1500,1600},  // 步骤10：前进50CM，PWM 目标编码器
		//{3, 1500,512},   // 步骤11：右转
	//	{1, 1500,4090},  // 步骤12：前进100CM，PWM 目标编码器
	  {4, 0, 0}       // 步骤13：停止（结束标记）	
};
 
TaskCommand tasks3[] = {
    {1, 1500,6134},  // 步骤0：前进75CM，PWM 目标编码器
		{3, 1500,512},   // 步骤1：右转
    {1, 1500,2045},  // 步骤2：前进50CM，PWM 目标编码器
    {2, 1500,512},   // 步骤3：左转， PWM 目标编码器
    {1, 1500,6134},  // 步骤4：前进150CM，PWM 目标编码器
    {2, 1500,512},   // 步骤5：左转， PWM 目标编码器
    {1, 1500,4090},  // 步骤6：前进100CM，PWM 目标编码器
		{2, 1500,512},   // 步骤7：左转， PWM 目标编码器
    {1, 1500,2045},  // 步骤8：前进50CM，PWM 目标编码器		
		{2, 1500,512},   // 步骤9：左转， PWM 目标编码器		
    {1, 1500,2045},  // 步骤10：前进50CM，PWM 目标编码器		
		{2, 1500,512},   // 步骤11：左转， PWM 目标编码器		
    {1, 1500,2045},  // 步骤12：前进50CM，PWM 目标编码器	
		{2, 1500,512},   // 步骤13：左转， PWM 目标编码器	
    {1, 1500,4090},  // 步骤14：前进100CM，PWM 目标编码器
		{2, 1500,512},   // 步骤15：左转， PWM 目标编码器			
    {1, 1500,2045},  // 步骤16：前进50CM，PWM 目标编码器		
		{2, 1500,512},   // 步骤17：左转， PWM 目标编码器	
		{1, 1500,2045},  // 步骤18：前进50CM，PWM 目标编码器	
		{2, 1500,512},   // 步骤19：左转， PWM 目标编码器	
	  {1, 1500,4090},  // 步骤20：前进100CM，PWM 目标编码器
	  {4, 0, 0}       // 步骤21：停止（结束标记）
};

TaskCommand tasks4[] = {
    {1, 1500,3069},  // 步骤0：前进75CM， PWM 目标编码器
    {2, 1500,512},   // 步骤1：左转， 		PWM 目标编码器
    {1, 1500,4090},  // 步骤2：前进100CM，PWM 目标编码器
		{3, 1500,512},   // 步骤3：右转
    {1, 1500,4090},  // 步骤4：前进100CM， PWM 目标编码器
		{3, 1500,512},   // 步骤5：右转
    {1, 1500,2045},  // 步骤6：前进50CM，  PWM 目标编码器
    {2, 1500,512},   // 步骤7：左转， 		 PWM 目标编码器
    {1, 1500,4090},  // 步骤8：前进100CM，PWM 目标编码器
    {4, 0, 0}       // 步骤9：停止（结束标记）
};

TaskCommand tasks5[]= 
{
    {1, 1500,3069},  // 步骤0：前进75CM， 进赛道
		{0, 0,0},// 步骤1：
		{0, 0,0},// 步骤2：
		{0, 0,0},// 步骤3：
		{0, 0,0},// 步骤4：
		{0, 0,0},// 步骤5：
		{0, 0,0},// 步骤6：
		{0, 0,0},// 步骤7：
		{0, 0,0},// 步骤8：
		{0, 0,0},// 步骤9：			
};


// 定义一个指向TaskCommand类型的指针数组，用于存储多个任务序列
// 每个元素(tasks1, tasks2, tasks3)应该是一个TaskCommand类型的数组或结构体
TaskCommand* allTaskSequences[] = {tasks1, tasks2, tasks3, tasks4,tasks5};

                // 清除所有电机目标
// 计算任务序列的数量
// sizeof(allTaskSequences) 获取整个数组的字节大小
// sizeof(allTaskSequences[0]) 获取单个指针元素的字节大小
// 两者相除得到数组元素(指针)的数量
const uint8_t taskSequenceCount = sizeof(allTaskSequences) / sizeof(allTaskSequences[0]);

// 定义一个包含4个元素的int32_t类型数组，用于存储编码器目标值
// 初始值全部设为0，表示编码器的初始目标位置
int32_t encoder_targets[4] = {0};  // 数组索引0-3对应4个不同的编码器通道


// 编码器10毫秒前后数据
int g_Encoder_All_Now[MAX_MOTOR] = {0};
int g_Encoder_All_Last[MAX_MOTOR] = {0};

int g_Encoder_All_Offset[MAX_MOTOR] = {0};

uint8_t g_start_ctrl = 0;

car_data_t car_data;
motor_data_t motor_data;

uint8_t g_yaw_adjust = 0;
car_type_t g_car_type = CAR_ACKERMAN; //CAR_MECANUM; ENCODER_CIRCLE_550


static float Motion_Get_Circle_Pulse(void)
{
    float temp = 0;
    switch (g_car_type)
    {
    case CAR_MECANUM:
        temp = ENCODER_CIRCLE_330;
        break;
    case CAR_MECANUM_MAX:
        temp = ENCODER_CIRCLE_205;
        break;
    case CAR_FOURWHEEL:
        temp = ENCODER_CIRCLE_330;
        break;
    case CAR_ACKERMAN:
        temp = ENCODER_CIRCLE_550;
        break;
    default:
        temp = ENCODER_CIRCLE_330;
        break;
    }
    return temp;
}


#if ENABLE_REAL_WHEEL
// 实际轮子转的圈数，单位：XX转/分钟
int real_circle[MAX_MOTOR] = {0};

// 实际轮子的速度，单位：m/s
float real_circle_speed[MAX_MOTOR] = {0};

//// 实际小车角速度
//float real_motion_angular = 0.0;

void* Motion_Real_Circle_Speed(uint8_t index)
{
    if (index == 1) return (int*) real_circle;
    if (index == 2) return (float*) real_circle_speed;
    return NULL;
}
#endif


// 仅用于添加到调试中显示数据。
void* Motion_Get_Data(uint8_t index)
{
    if (index == 1) return (int*)g_Encoder_All_Now;
    if (index == 2) return (int*)g_Encoder_All_Last;
    if (index == 3) return (int*)g_Encoder_All_Offset;
    return NULL;
}

// 获取电机速度
void Motion_Get_Motor_Speed(float* speed)
{
    for (int i = 0; i < 4; i++)
    {
        speed[i] = motor_data.speed_mm_s[i];
        
    }
}

// 控制小车运动，Motor_X=[-3600, 3600]，超过范围则无效。
void Motion_Set_Pwm(int16_t Motor_1, int16_t Motor_2, int16_t Motor_3, int16_t Motor_4)
{
    if (Motor_1 >= -MOTOR_MAX_PULSE && Motor_1 <= MOTOR_MAX_PULSE)//L1  
    {   
        Motor_Set_Pwm(MOTOR_ID_M1, Motor_1);
    }
    if (Motor_2 >= -MOTOR_MAX_PULSE && Motor_2 <= MOTOR_MAX_PULSE)//L2
    {
        Motor_Set_Pwm(MOTOR_ID_M2, Motor_2);
    }
    if (Motor_3 >= -MOTOR_MAX_PULSE && Motor_3 <= MOTOR_MAX_PULSE)//R1
    {
        Motor_Set_Pwm(MOTOR_ID_M3, Motor_3);
    }
    if (Motor_4 >= -MOTOR_MAX_PULSE && Motor_4 <= MOTOR_MAX_PULSE)//R2
    {
        Motor_Set_Pwm(MOTOR_ID_M4, Motor_4);
    }
}

// 小车停止
void Motion_Stop(uint8_t brake)
{
		g_Encoder_M1_Now = 0;
		g_Encoder_M2_Now = 0;
		g_Encoder_M3_Now = 0;
		g_Encoder_M4_Now = 0;	
    Motion_Set_Speed(0, 0, 0, 0);
		PID_Clear_Motor(MAX_MOTOR);
    g_start_ctrl = 0;
    g_yaw_adjust = 0;
    Motor_Stop(brake);
}


void Motion_Set_Speed(int16_t speed_m1, int16_t speed_m2, int16_t speed_m3, int16_t speed_m4)
{
    g_start_ctrl = 1;

    // 单独设置每个电机的目标速度（如果参数为 INT16_MAX，则表示不修改）
    if (speed_m1 != INT16_MAX) motor_data.speed_set[0] = speed_m1;
    if (speed_m2 != INT16_MAX) motor_data.speed_set[1] = speed_m2;
    if (speed_m3 != INT16_MAX) motor_data.speed_set[2] = speed_m3;
    if (speed_m4 != INT16_MAX) motor_data.speed_set[3] = speed_m4;

    // 更新 PID 目标值
    for (uint8_t i = 0; i < MAX_MOTOR; i++)
    {
        PID_Set_Motor_Target(i, (float)motor_data.speed_set[i]);
    }
}

void Motion_Get_Speed(car_data_t* car)
{
    float speed_mm[MAX_MOTOR] = {0};
    float circle_mm = Motion_Get_Circle_MM();  // 轮子周长(mm)
    float circle_pulse = Motion_Get_Circle_Pulse();  // 编码器每转脉冲数
    float wheelbase = ROBOT_WIDTH;  // 轮距
    int active_motor_count = 0;     // 活跃电机计数
    float total_speed = 0.0f;       // 总速度累加

    Motion_Get_Encoder();  // 读取编码器值

    // 计算每个轮子的速度(mm/s)
    for (int i = 0; i < MAX_MOTOR; i++) {
        // 只有当电机设定速度不为0时才计算实际速度（表示电机是开启的）
        if (motor_data.speed_set[i] != 0) {
            speed_mm[i] = (g_Encoder_All_Offset[i]) * 100 * circle_mm / circle_pulse;
            active_motor_count++;
            total_speed += speed_mm[i];
        } else {
            speed_mm[i] = 0.0f;  // 确保未开启电机速度为0
        }
    }

    // 计算纵向速度（活跃轮子的平均速度）
    car->Vx = (active_motor_count > 0) ? (total_speed / active_motor_count) : 0.0f;

    // 普通四轮小车没有横向速度
    car->Vy = 0;

    // 计算旋转速度（左侧轮子 vs 右侧轮子）
    // 只考虑开启的电机
    float left_speed = 0.0f;
    int left_count = 0;
    float right_speed = 0.0f;
    int right_count = 0;
    
    // 假设电机0和1是左侧，2和3是右侧（根据实际布局调整）
    if (motor_data.speed_set[0] != 0 || motor_data.speed_set[1] != 0) {
        left_speed = (motor_data.speed_set[0] != 0) ? speed_mm[0] : 0.0f;
        left_speed += (motor_data.speed_set[1] != 0) ? speed_mm[1] : 0.0f;
        left_count = (motor_data.speed_set[0] != 0) + (motor_data.speed_set[1] != 0);
        if (left_count > 0) left_speed /= left_count;
    }
    
    if (motor_data.speed_set[2] != 0 || motor_data.speed_set[3] != 0) {
        right_speed = (motor_data.speed_set[2] != 0) ? speed_mm[2] : 0.0f;
        right_speed += (motor_data.speed_set[3] != 0) ? speed_mm[3] : 0.0f;
        right_count = (motor_data.speed_set[2] != 0) + (motor_data.speed_set[3] != 0);
        if (right_count > 0) right_speed /= right_count;
    }
    
    // 只有当左右两侧都有活跃电机时才计算旋转速度
    if (left_count > 0 && right_count > 0) {
        car->Vz = (left_speed - right_speed) / wheelbase * 1000;
    } else {
        car->Vz = 0.0f;
    }

    if (g_start_ctrl)
    {
        // 保存轮子速度（只保存开启电机的速度）
        for (int i = 0; i < MAX_MOTOR; i++) {
            if (motor_data.speed_set[i] != 0) {
                motor_data.speed_mm_s[i] = speed_mm[i];
            } else {
                motor_data.speed_mm_s[i] = 0.0f;  // 确保未开启电机速度记录为0
            }
        }
        
        #if ENABLE_YAW_ADJUST
        if (g_yaw_adjust) {
            #if ENABLE_INV_MEMS
            Yaw_Calc(ICM20948_Get_Yaw_Now());
            #elif ENABLE_MPU9250
            Yaw_Calc(MPU_Get_Yaw_Now());
            #endif
        }
        #endif
        
        // 优化后的 PID 计算：只对开启的电机进行 PID 控制
        PID_Calc_Motor(&motor_data);  // 修改 PID_Calc_Motor 函数，使其支持单电机判断

        #if PID_ASSISTANT_EN
        if (start_tool()) {
            int32_t speed_send = car->Vx;
            set_computer_value(SEND_FACT_CMD, CURVES_CH1, &speed_send, 1);
        }
        #endif
    }
}



// 返回当前小车轮子轴间距和的一半
float Motion_Get_APB(void)
{
//    if (g_car_type == CAR_MECANUM) return MECANUM_APB;
//    if (g_car_type == CAR_MECANUM_MAX) return MECANUM_MAX_APB;
//    if (g_car_type == CAR_MECANUM_MINI) return MECANUM_MINI_APB;
//    if (g_car_type == CAR_ACKERMAN) return AKM_WIDTH;
//    if (g_car_type == CAR_FOURWHEEL) return FOURWHEEL_APB;
    return MECANUM_APB;
}

// 返回当前小车轮子转一圈的多少毫米
float Motion_Get_Circle_MM(void)
{
//    if (g_car_type == CAR_MECANUM) return MECANUM_CIRCLE_MM;
//    if (g_car_type == CAR_MECANUM_MAX) return MECANUM_MAX_CIRCLE_MM;
//    if (g_car_type == CAR_MECANUM_MINI) return MECANUM_MINI_CIRCLE_MM;
//    if (g_car_type == CAR_ACKERMAN) return AKM_CIRCLE_MM;
//    if (g_car_type == CAR_FOURWHEEL) return FOURWHEEL_CIRCLE_MM;
    return MECANUM_CIRCLE_MM;
}

// 设置当前控制的小车类型。
void Motion_Set_Car_Type(car_type_t car_type)
{
    if (car_type >= CAR_TYPE_MAX) return;
    if (g_car_type == car_type) return;
    g_car_type = car_type;
    // Flash_Set_CarType(g_car_type);
}

// 返回当前控制的小车类型。
uint8_t Motion_Get_Car_Type(void)
{
    return (uint8_t)g_car_type;
}

// 获取编码器数据，并计算偏差脉冲数
void Motion_Get_Encoder(void)
{
    Encoder_Get_ALL(g_Encoder_All_Now);

    for(uint8_t i = 0; i < MAX_MOTOR; i++)
    {
        // 记录两次测试时间差的脉冲数
        g_Encoder_All_Offset[i] = g_Encoder_All_Now[i] - g_Encoder_All_Last[i];
	    // 记录上次编码器数据
	    g_Encoder_All_Last[i] = g_Encoder_All_Now[i];
    
    #if ENABLE_REAL_WHEEL
        // 计算每分钟转多少圈，10毫秒测到的脉冲数*100变秒*60变分钟/每一圈的脉冲数
        real_circle[i] = g_Encoder_All_Offset[i] * 60 * 100 / Motion_Get_Circle_Pulse();
        // 计算轮子速度，单位m/s。每分钟转的圈数*转一圈运动的距离/60得到每秒的米数
        real_circle_speed[i] = real_circle[i] * (Motion_Get_Circle_MM() / 1000.0) / 60.0;
    #endif
    }
}

// 运动控制句柄，每10ms调用一次，主要处理速度相关的数据
void Motion_Handle(void)
{
    Motion_Get_Speed(&car_data);

    if (g_start_ctrl)
    {
        Motion_Set_Pwm(motor_data.speed_pwm[0], motor_data.speed_pwm[1], motor_data.speed_pwm[2], motor_data.speed_pwm[3]);
    }
}
 

// 检查是否有电机达到目标位置
// 返回值：true表示至少有一个电机到达目标位置，false表示所有电机都未到达
bool Check_Any_Motor_Reached_Target(void) {
    // 遍历所有4个电机
    for (int i = 0; i < 4; i++) {
        // 如果电机有目标位置且当前值已达到目标值（考虑正反转）
        if (encoder_targets[i] > 0 && abs(current_encoder_values[i]) >= encoder_targets[i]) {
            return true; // 只要有一个电机到达，就返回 true
        }
    }
    return false; // 所有电机都未到达目标位置
}

// 更新电机PWM值（如果电机已到达目标位置则停止）
// 参数：motor_id - 电机ID；current_pwm - 当前PWM值
// 返回值：更新后的PWM值（如果已到达目标则返回0）
int16_t Update_Motor_PWM(MotorID motor_id, int16_t current_pwm) {
    // 检查该电机是否有目标位置
    if (encoder_targets[motor_id] > 0) {
        // 如果当前编码器值已达到或超过目标值（考虑正反转）
        if (abs(current_encoder_values[motor_id]) >= encoder_targets[motor_id]) {
            return 0; // 停止电机
        }
    }
    return current_pwm; // 保持当前PWM值
}

#include <math.h>  // for roundf()

// 限制速度在 [-3600, 3600] 范围内
#define MIN_SPEED  -3600
#define MAX_SPEED 3600


const float forward_comp[4] = {1.027f, 1.027f, 0.975f,  0.975f};  // 前进补偿
const float turn_left_comp[4]  = {1.027f, 1.027f, 0.975f, 0.975f};  // 左转补偿
const float turn_right_comp[4] = {1.027f, 1.027f, 0.975f, 0.975f};  // 右转补偿

// 修改后的补偿函数（增加方向参数）
void Apply_State_Compensation(int16_t base_speed, const float* comp, int16_t* pwm_out) {
    for(int i=0; i<4; i++){
        float compensated = base_speed * comp[i];
        // 增加动态限幅（可根据方向调整限幅值）
        compensated = fminf(fmaxf(compensated, base_speed*0.5f), base_speed*1.5f);
        pwm_out[i] = (int16_t)roundf(compensated);
    }
}

bool compensationApplied = false;  // 标记是否已应用补偿

int16_t pwm_m1 = 0;
int16_t pwm_m2 = 0;
int16_t pwm_m3 = 0;
int16_t pwm_m4 = 0;

// 小车控制任务主函数
void Car_Control_Task(void) {
    // 手动切换任务序列（通过KEY2按键）
    if (Key2_is_Press() == KEY2_PRESS) {
        // 切换到下一个任务序列（循环）
        currentTaskSequence = (currentTaskSequence + 1) % taskSequenceCount;
        currentStepIndex = 0; // 重置步骤索引
        taskRunning = false; // 停止当前任务
				Motion_Set_Pwm(0,0,0,0);
				if(currentTaskSequence == 4)//
				{
					KeyManager_Update(g_Encoder_M1_Now,flag_1s);
				}
				delay_ms(500);

    }
    // 启动任务（通过KEY1按键）
    if (!taskRunning && Key1_is_Press() == KEY1_PRESS) {
				Motion_Set_Pwm(0,0,0,0);
        taskRunning = true; // 标记任务为运行状态
        currentStepIndex = 0; // 从第一步开始
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(500);
    }
 
    // 执行任务逻辑
    if (taskRunning) {
        // 获取当前任务序列
        TaskCommand* currentTask = allTaskSequences[currentTaskSequence];
        // 获取当前步骤的命令
        TaskCommand cmd = currentTask[currentStepIndex];
		 
					// 根据命令类型设置电机目标速度
				switch (cmd.command) {
							case 1: { // 前进命令
								int16_t pwm[4];
								Apply_State_Compensation(cmd.pwm, forward_comp, pwm);
								Motion_Set_Pwm(pwm[0], pwm[1], pwm[2], pwm[3]);
								encoder_targets[MOTOR_M1] = cmd.encoder_target;
								encoder_targets[MOTOR_M2] = cmd.encoder_target;
								encoder_targets[MOTOR_M3] = cmd.encoder_target;
								encoder_targets[MOTOR_M4] = cmd.encoder_target;
								break;
						}
						case 2: { // 左转命令
								int16_t pwm[4];
								Apply_State_Compensation(cmd.pwm, turn_left_comp, pwm);
								encoder_targets[MOTOR_M1] = cmd.encoder_target;
								encoder_targets[MOTOR_M2] = cmd.encoder_target;
								encoder_targets[MOTOR_M3] = cmd.encoder_target;
								encoder_targets[MOTOR_M4] = cmd.encoder_target;
								Motion_Set_Pwm(pwm[0], pwm[1], -pwm[2], -pwm[3]);
								break;
						}
						case 3: { // 右转命令
								int16_t pwm[4];
								Apply_State_Compensation(cmd.pwm, turn_right_comp, pwm);
								encoder_targets[MOTOR_M1] = cmd.encoder_target;
								encoder_targets[MOTOR_M2] = cmd.encoder_target;
								encoder_targets[MOTOR_M3] = cmd.encoder_target;
								encoder_targets[MOTOR_M4] = cmd.encoder_target;
								Motion_Set_Pwm(-pwm[0], -pwm[1], pwm[2], pwm[3]);
								break;
						}
						case 4:// 停止命令			
						default:
								for (int i = 0; i < 4; i++) encoder_targets[i] = 0;
								Motor_Stop(1);	
								taskRunning = false; // 停止任务
								break;			
				}				

			 
				// 检查是否完成当前步骤（所有电机到达目标）
				if (Check_Any_Motor_Reached_Target()) 
				{
//						compensationApplied = false;// 重置标志位
						// 获取下一步命令
						TaskCommand nextCmd = currentTask[currentStepIndex + 1];
						// 检查是否是结束命令（command=4且参数为0）
						if (nextCmd.command == 4) 
						{
								taskRunning = false; // 任务完成
								current_encoder_values[0] = 0;
								current_encoder_values[1] = 0;
								current_encoder_values[2] = 0;
								current_encoder_values[3] = 0;
								g_Encoder_M1_Now = 0;
							  g_Encoder_M2_Now = 0;
							  g_Encoder_M3_Now = 0;
							  g_Encoder_M4_Now = 0;
								Motion_Set_Pwm(0,0,0,0);
								Motor_Stop(1);
						} 
						else 
						{
								current_encoder_values[0] = 0;
								current_encoder_values[1] = 0;
								current_encoder_values[2] = 0;
								current_encoder_values[3] = 0;	
								g_Encoder_M1_Now = 0;
							  g_Encoder_M2_Now = 0;
							  g_Encoder_M3_Now = 0;
							  g_Encoder_M4_Now = 0;
							
								Motion_Set_Pwm(0,0,0,0);
								Motor_Stop(1);
								currentStepIndex++; // 移动到下一步
						}
				}						

    }
}

