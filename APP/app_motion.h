#ifndef __APP_MOTION_H__
#define __APP_MOTION_H__

#include "stdint.h"
#include "AllHeader.h"

// 205RPM鐢垫満锛岃疆瀛愯浆涓€鏁村湀锛岀紪鐮佸櫒鑾峰緱鐨勮剦鍐叉暟=鍑忛€熸瘮*鐮佺洏绾挎暟*缂栫爜鍣ㄨ剦鍐诧紙56*11*4锛?
#define ENCODER_CIRCLE_205           (2464.0f)

// 330RPM鐢垫満锛岃疆瀛愯浆涓€鏁村湀锛岀紪鐮佸櫒鑾峰緱鐨勮剦鍐叉暟=鍑忛€熸瘮*鐮佺洏绾挎暟*缂栫爜鍣ㄨ剦鍐诧紙30*11*4锛?
#define ENCODER_CIRCLE_330           (1320.0f)

// 550RPM鐢垫満锛岃疆瀛愯浆涓€鏁村湀锛岀紪鐮佸櫒鑾峰緱鐨勮剦鍐叉暟=鍑忛€熸瘮*鐮佺洏绾挎暟*缂栫爜鍣ㄨ剦鍐诧紙19*11*4锛?
#define ENCODER_CIRCLE_550           (836.0f)

// 杞瓙杞竴鏁村湀鐨勪綅绉伙紝鍗曚綅涓虹背
#define DISTANCE_CIRCLE      (0.204203)


// 鍋滄妯″紡锛孲TOP_FREE琛ㄧず鑷敱鍋滄锛孲TOP_BRAKE琛ㄧず鍒硅溅銆?
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
    CAR_TYPE_NONE = 0x00,       // 淇濈暀
    CAR_MECANUM = 0x01,         // 灏忔灦鏋勫皬楹﹁疆 X3
    CAR_MECANUM_MAX = 0x02,     // 澶ф灦鏋勫ぇ楹﹁疆 X3 PLUS
    CAR_MECANUM_MINI = 0x03,    // 澶ф灦鏋勫皬楹﹁疆 鏃?
    CAR_FOURWHEEL = 0x04,       // 鍥涜疆鏅€氬皬杞?X1
    CAR_ACKERMAN = 0x05,        // 闃垮厠鏇煎皬杞?  R2

    CAR_TYPE_MAX                // 鏈€鍚庝竴涓皬杞︾被鍨嬶紝浠呬綔涓哄垽鏂?
} car_type_t;


// 鐢垫満缂栧彿
typedef enum {
    MOTOR_M1 = 0,  // 宸﹀墠
    MOTOR_M2,      // 宸﹀悗
    MOTOR_M3,      // 鍙冲墠
    MOTOR_M4       // 鍙冲悗
} MotorID;
 
// 浠诲姟鍛戒护
typedef struct {
    uint8_t command;       // 1=鍓嶈繘, 2=宸﹁浆, 3=鍙宠浆, 4=鍋滄
    int16_t pwm;           // 鍩虹 PWM 鍊?
    int32_t encoder_target; // 鐩爣缂栫爜鍣ㄥ€硷紙鑴夊啿鏁帮級
} TaskCommand;
 
// 鍏ㄥ眬鍙橀噺澹版槑
extern int32_t	current_encoder_values[4]; 		// M1, M2, M3, M4 鐨勫綋鍓嶇紪鐮佸櫒鍊?

extern TaskCommand tasks5[];

extern volatile uint8_t currentTaskSequence;          // 褰撳墠浠诲姟搴忓垪绱㈠紩锛?: tasks1, 1: tasks2, ...锛?
extern volatile uint8_t currentStepIndex;             // 褰撳墠姝ラ绱㈠紩
 
// 鍑芥暟澹版槑
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
