#ifndef __BSP_PID_H__
#define __BSP_PID_H__

#include "stdint.h"
#include "AllHeader.h"

 #define PID_DEF_KP      (2.0f)
 #define PID_DEF_KI      (0.03f)
 #define PID_DEF_KD      (1.2f)
//#define PID_DEF_KP      (0.8f)
//#define PID_DEF_KI      (0.06f)
//#define PID_DEF_KD      (0.5f)

//#define PID_YAW_DEF_KP  (0.4)
//#define PID_YAW_DEF_KI  (0.0)
//#define PID_YAW_DEF_KD  (0.1)


typedef struct _pid
{
    float target_val;               //鐩爣鍊?
    float output_val;               //杈撳嚭鍊?
    float pwm_output;        		//PWM杈撳嚭鍊?
    float Kp,Ki,Kd;          		//瀹氫箟姣斾緥銆佺Н鍒嗐€佸井鍒嗙郴鏁?
    float err;             			//瀹氫箟鍋忓樊鍊?
    float err_last;          		//瀹氫箟涓婁竴涓亸宸€?

    float err_next;                 //瀹氫箟涓嬩竴涓亸宸€? 澧為噺寮?
    float integral;          		//瀹氫箟绉垎鍊硷紝浣嶇疆寮?
} pid_t;

typedef struct _motor_data_t
{
    float speed_mm_s[4];        // 杈撳叆鍊硷紝缂栫爜鍣ㄨ绠楅€熷害
    float speed_pwm[4];         // 杈撳嚭鍊硷紝PID璁＄畻鍑篜WM鍊?
    int16_t speed_set[4];       // 閫熷害璁剧疆鍊?
} motor_data_t;


typedef struct
{
    float SetPoint;   // 璁惧畾鐩爣Desired value
    float Proportion; // 姣斾緥甯告暟Proportional Const
    float Integral;   // 绉垎甯告暟Integral Const
    float Derivative; // 寰垎甯告暟Derivative Const
    float LastError;  // Error[-1]
    float PrevError;  // Error[-2]
    float SumError;   // Sums of Errors
} PID;



void PID_Param_Init(void);

float PID_Location_Calc(pid_t *pid, float actual_val);
void PID_Calc_Motor(motor_data_t* motor);
float PID_Calc_One_Motor(uint8_t motor_id, float now_speed);
void PID_Set_Motor_Target(uint8_t motor_id, float target);
void PID_Clear_Motor(uint8_t motor_id);
void PID_Set_Motor_Parm(uint8_t motor_id, float kp, float ki, float kd);
void PID_Send_Parm_Active(uint8_t index);



void PID_Yaw_Reset(float yaw);
float PID_Yaw_Calc(float NextPoint);
void PID_Yaw_Set_Parm(float kp, float ki, float kd);

#endif /* __BSP_PID_H__ */
