#include "bsp_pid.h"

#define PI      (3.1415926f)


pid_t pid_motor[4];

// YAW鍋忚埅瑙?
PID pid_Yaw = {0, 0.4, 0, 0.1, 0, 0, 0};


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

// 鍒濆鍖朠ID鍙傛暟
void PID_Param_Init(void)
{
    /* 閫熷害鐩稿叧鍒濆鍖栧弬鏁?*/
    for (int i = 0; i < MAX_MOTOR; i++)
    {
        pid_motor[i].target_val = 0.0;
        pid_motor[i].pwm_output = 0.0;
        pid_motor[i].err = 0.0;
        pid_motor[i].err_last = 0.0;
        pid_motor[i].err_next = 0.0;
        pid_motor[i].integral = 0.0;

        pid_motor[i].Kp = PID_DEF_KP;
        pid_motor[i].Ki = PID_DEF_KI;
        pid_motor[i].Kd = PID_DEF_KD;
    }

//    pid_Yaw.Proportion = PID_YAW_DEF_KP;
//    pid_Yaw.Integral = PID_YAW_DEF_KI;
//    pid_Yaw.Derivative = PID_YAW_DEF_KD;
    
    #if PID_ASSISTANT_EN
    float pid_temp[3] = {pid_motor[0].Kp, pid_motor[0].Ki, pid_motor[0].Kd};
    set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // 缁欓€氶亾 1 鍙戦€?P I D 鍊?
    #endif
}

// 璁剧疆PID鍙傛暟
void PID_Set_Parm(pid_t *pid, float p, float i, float d)
{
    pid->Kp = p; // 璁剧疆姣斾緥绯绘暟 P
    pid->Ki = i; // 璁剧疆绉垎绯绘暟 I
    pid->Kd = d; // 璁剧疆寰垎绯绘暟 D
}

// 璁剧疆PID鐨勭洰鏍囧€?
void PID_Set_Target(pid_t *pid, float temp_val)
{
    pid->target_val = temp_val; // 璁剧疆褰撳墠鐨勭洰鏍囧€?
}

// 鑾峰彇PID鐩爣鍊?
float PID_Get_Target(pid_t *pid)
{
    return pid->target_val; // 璁剧疆褰撳墠鐨勭洰鏍囧€?
}

// 澧為噺寮廝ID璁＄畻鍏紡
float PID_Incre_Calc(pid_t *pid, float actual_val)
{
    /*璁＄畻鐩爣鍊间笌瀹為檯鍊肩殑璇樊*/
    pid->err = pid->target_val - actual_val;
    /*PID绠楁硶瀹炵幇*/
    pid->pwm_output += pid->Kp * (pid->err - pid->err_next) 
                    + pid->Ki * pid->err 
                    + pid->Kd * (pid->err - 2 * pid->err_next + pid->err_last);
    /*浼犻€掕宸?/
    pid->err_last = pid->err_next;
    pid->err_next = pid->err;
    
    /*杩斿洖PWM杈撳嚭鍊?/
    if (pid->pwm_output > (MOTOR_MAX_PULSE-MOTOR_IGNORE_PULSE))
        pid->pwm_output = (MOTOR_MAX_PULSE-MOTOR_IGNORE_PULSE);
    if (pid->pwm_output < (MOTOR_IGNORE_PULSE-MOTOR_MAX_PULSE))
        pid->pwm_output = (MOTOR_IGNORE_PULSE-MOTOR_MAX_PULSE);
    return pid->pwm_output;

    // // 璁＄畻鍋忓樊
    // pid->err = pid->target_val - actual_val;
    // //澧為噺寮廝I鎺у埗鍣?
    // pid->pwm_output += pid->Kp * (pid->err - pid->err_last) + pid->Ki * pid->err;
                   
    // if (pid->pwm_output > (MOTOR_MAX_PULSE-MOTOR_IGNORE_PULSE))
    //     pid->pwm_output = (MOTOR_MAX_PULSE-MOTOR_IGNORE_PULSE);
    // if (pid->pwm_output < (MOTOR_IGNORE_PULSE-MOTOR_MAX_PULSE))
    //     pid->pwm_output = (MOTOR_IGNORE_PULSE-MOTOR_MAX_PULSE);
    // pid->err_last = pid->err;
    // return pid->pwm_output;
}

// 浣嶇疆寮廝ID璁＄畻鏂瑰紡
float PID_Location_Calc(pid_t *pid, float actual_val)
{
	/*璁＄畻鐩爣鍊间笌瀹為檯鍊肩殑璇樊*/
    pid->err = pid->target_val - actual_val;
  
    /* 闄愬畾闂幆姝诲尯 */
    if((pid->err >= -40) && (pid->err <= 40))
    {
        pid->err = 0;
        pid->integral = 0;
    }
    
    /* 绉垎鍒嗙锛屽亸宸緝澶ф椂鍘绘帀绉垎浣滅敤 */
    if (pid->err > -1500 && pid->err < 1500)
    {
        pid->integral += pid->err;    // 璇樊绱Н

        /* 闄愬畾绉垎鑼冨洿锛岄槻姝㈢Н鍒嗛ケ鍜?*/
        if (pid->integral > 4000)
            pid->integral = 4000;
        else if (pid->integral < -4000)
            pid->integral = -4000;
    }

	/*PID绠楁硶瀹炵幇*/
    pid->output_val = pid->Kp * pid->err + 
                      pid->Ki * pid->integral + 
                      pid->Kd * (pid->err - pid->err_last);

	/*璇樊浼犻€?/
    pid->err_last = pid->err;
    
	/*杩斿洖褰撳墠瀹為檯鍊?/
    return pid->output_val;
}


// PID璁＄畻杈撳嚭鍊?
void PID_Calc_Motor(motor_data_t* motor)
{
    for (int i = 0; i < MAX_MOTOR; i++)
    {
        // 鍙湁褰撶數鏈鸿瀹氶€熷害涓嶄负0鏃舵墠杩涜PID璁＄畻锛堣〃绀虹數鏈烘槸寮€鍚殑锛?
        if (motor->speed_set[i] != 0)
        {
            motor->speed_pwm[i] =  PID_Location_Calc(&pid_motor[i], motor->speed_mm_s[i]);
        }
        else
        {
            motor->speed_pwm[i] = 0;  // 鏈紑鍚數鏈猴紝PWM杈撳嚭璁句负0
        }
    }
}

// PID鍗曠嫭璁＄畻涓€鏉￠€氶亾
float PID_Calc_One_Motor(uint8_t motor_id, float now_speed)
{
    if (motor_id >= MAX_MOTOR) return 0; 
    return PID_Incre_Calc(&pid_motor[motor_id], now_speed);
}

// 璁剧疆PID鍙傛暟锛宮otor_id=4璁剧疆鎵€鏈夛紝=0123璁剧疆瀵瑰簲鐢垫満鐨凱ID鍙傛暟銆?
void PID_Set_Motor_Parm(uint8_t motor_id, float kp, float ki, float kd)
{
    if (motor_id > MAX_MOTOR) return;

    if (motor_id == MAX_MOTOR)
    {
        for (int i = 0; i < MAX_MOTOR; i++)
        {
            pid_motor[i].Kp = kp;
            pid_motor[i].Ki = ki;
            pid_motor[i].Kd = kd;
        }
       // DEBUG("PID Set:%.3f, %.3f, %.3f\n", kp, ki, kd);
    }
    else
    {
        pid_motor[motor_id].Kp = kp;
        pid_motor[motor_id].Ki = ki;
        pid_motor[motor_id].Kd = kd;
       // DEBUG("PID Set M%d:%.3f, %.3f, %.3f\n", motor_id+1, kp, ki, kd);
    }
}

// 娓呴櫎PID鏁版嵁
void PID_Clear_Motor(uint8_t motor_id)
{
    if (motor_id > MAX_MOTOR) return;

    if (motor_id == MAX_MOTOR)
    {
        for (int i = 0; i < MAX_MOTOR; i++)
        {
            pid_motor[i].pwm_output = 0.0;
            pid_motor[i].err = 0.0;
            pid_motor[i].err_last = 0.0;
            pid_motor[i].err_next = 0.0;
            pid_motor[i].integral = 0.0;
        }
    }
    else
    {
        pid_motor[motor_id].pwm_output = 0.0;
        pid_motor[motor_id].err = 0.0;
        pid_motor[motor_id].err_last = 0.0;
        pid_motor[motor_id].err_next = 0.0;
        pid_motor[motor_id].integral = 0.0;
    }
}

// 璁剧疆PID鐩爣閫熷害锛屽崟浣嶄负锛歮m/s
void PID_Set_Motor_Target(uint8_t motor_id, float target)
{
    if (motor_id > MAX_MOTOR) return;

    if (motor_id == MAX_MOTOR)
    {
        for (int i = 0; i < MAX_MOTOR; i++)
        {
            pid_motor[i].target_val = target;
        }
    }
    else
    {
        pid_motor[motor_id].target_val = target;
    }
}

// 杩斿洖PID缁撴瀯浣撴暟缁?
pid_t* Pid_Get_Motor(void)
{
    return pid_motor;
}


//// 閲嶇疆鍋忚埅瑙掔殑鐩爣鍊?
//void PID_Yaw_Reset(float yaw)
//{
//		pid_Yaw.SetPoint = yaw;
//    pid_Yaw.SumError = 0;
//    pid_Yaw.LastError = 0;
//    pid_Yaw.PrevError = 0;
//}

//// 璁＄畻鍋忚埅瑙掔殑杈撳嚭鍊?
//float PID_Yaw_Calc(float NextPoint)
//{
//	float dError, Error;
//	Error = pid_Yaw.SetPoint - NextPoint;			// 鍋忓樊
//	pid_Yaw.SumError += Error;						// 绉垎
//	dError = pid_Yaw.LastError - pid_Yaw.PrevError; // 褰撳墠寰垎
//	pid_Yaw.PrevError = pid_Yaw.LastError;
//	pid_Yaw.LastError = Error;

//	double omega_rad = pid_Yaw.Proportion * Error			 // 姣斾緥椤?
//					   + pid_Yaw.Integral * pid_Yaw.SumError // 绉垎椤?
//					   + pid_Yaw.Derivative * dError;		 // 寰垎椤?

//	if (omega_rad > PI / 6)
//		omega_rad = PI / 6;
//	if (omega_rad < -PI / 6)
//		omega_rad = -PI / 6;
//	return omega_rad;
//}

//// 璁剧疆鍋忚埅瑙扨ID鐨勫弬鏁?
//void PID_Yaw_Set_Parm(float kp, float ki, float kd)
//{
//    pid_Yaw.Proportion = kp;
//    pid_Yaw.Integral = ki;
//    pid_Yaw.Derivative = kd;
//}

