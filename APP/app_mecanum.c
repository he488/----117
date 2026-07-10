#include "app_mecanum.h"

//static float speed_lr = 0;
//static float speed_fb = 0;
//static float speed_spin = 0;

//static int speed_L1_setup = 0;
//static int speed_L2_setup = 0;
//static int speed_R1_setup = 0;
//static int speed_R2_setup = 0;

//static int g_offset_yaw = 0;
uint16_t g_speed_setup = 0;

//// X杞撮€熷害(鍓嶆鍚庤礋锛毬?000)锛孻杞撮€熷害(宸︽鍙宠礋锛毬?000)锛屾棆杞€熷害(宸︽鍙宠礋锛毬?000)
//// X3 PLUS X杞撮€熷害(鍓嶆鍚庤礋锛毬?00)锛孻杞撮€熷害(宸︽鍙宠礋锛毬?00)锛屾棆杞€熷害(宸︽鍙宠礋锛毬?200)
//void Mecanum_Ctrl(int16_t V_x, int16_t V_y, int16_t V_z, uint8_t adjust)
//{
//    float robot_APB = Motion_Get_APB();
//    speed_lr = -V_y;
//    speed_fb = V_x;
//    speed_spin = (-V_z / 1000.0f) * robot_APB;
//    if (V_x == 0 && V_y == 0 && V_z == 0)
//    {
//        Motion_Stop(STOP_BRAKE);
//        return;
//    }

//    speed_L1_setup = speed_fb + speed_lr + speed_spin;
//    speed_L2_setup = speed_fb - speed_lr + speed_spin;
//    speed_R1_setup = speed_fb - speed_lr - speed_spin;
//    speed_R2_setup = speed_fb + speed_lr - speed_spin;

//    if (Motion_Get_Car_Type() == CAR_MECANUM_MAX)
//    {
//        if (speed_L1_setup > CAR_X3_PLUS_MAX_SPEED) speed_L1_setup = CAR_X3_PLUS_MAX_SPEED;
//        if (speed_L1_setup < -CAR_X3_PLUS_MAX_SPEED) speed_L1_setup = -CAR_X3_PLUS_MAX_SPEED;
//        if (speed_L2_setup > CAR_X3_PLUS_MAX_SPEED) speed_L2_setup = CAR_X3_PLUS_MAX_SPEED;
//        if (speed_L2_setup < -CAR_X3_PLUS_MAX_SPEED) speed_L2_setup = -CAR_X3_PLUS_MAX_SPEED;
//        if (speed_R1_setup > CAR_X3_PLUS_MAX_SPEED) speed_R1_setup = CAR_X3_PLUS_MAX_SPEED;
//        if (speed_R1_setup < -CAR_X3_PLUS_MAX_SPEED) speed_R1_setup = -CAR_X3_PLUS_MAX_SPEED;
//        if (speed_R2_setup > CAR_X3_PLUS_MAX_SPEED) speed_R2_setup = CAR_X3_PLUS_MAX_SPEED;
//        if (speed_R2_setup < -CAR_X3_PLUS_MAX_SPEED) speed_R2_setup = -CAR_X3_PLUS_MAX_SPEED;
//    }
//    else
//    {
//        if (speed_L1_setup > 1000) speed_L1_setup = 1000;
//        if (speed_L1_setup < -1000) speed_L1_setup = -1000;
//        if (speed_L2_setup > 1000) speed_L2_setup = 1000;
//        if (speed_L2_setup < -1000) speed_L2_setup = -1000;
//        if (speed_R1_setup > 1000) speed_R1_setup = 1000;
//        if (speed_R1_setup < -1000) speed_R1_setup = -1000;
//        if (speed_R2_setup > 1000) speed_R2_setup = 1000;
//        if (speed_R2_setup < -1000) speed_R2_setup = -1000;
//    }
//    Motion_Set_Speed(speed_L1_setup, speed_L2_setup, speed_R1_setup, speed_R2_setup);
//}



//// 閫氳繃鍋忚埅瑙掕绠楀綋鍓嶇殑鍋忓樊鍊硷紝鏍″噯灏忚溅杩愬姩鏂瑰悜銆?
//void Mecanum_Yaw_Calc(float yaw)
//{
//    float yaw_offset = PID_Yaw_Calc(yaw);
//    g_offset_yaw = yaw_offset * g_speed_setup;

//    // 鐢ㄤ簬璋冭瘯鎵撳嵃鍋忓樊鏁版嵁
//    // static int aaaaaaaaa = 0;
//    // aaaaaaaaa++;
//    // if (aaaaaaaaa > 5)
//    // {
//    //     aaaaaaaaa = 0;
//    //     DEBUG("Yaw Calc:%.5f, %d", yaw_offset, g_offset_yaw);
//    // }

//    int speed_L1 = speed_L1_setup - g_offset_yaw;
//    int speed_L2 = speed_L2_setup - g_offset_yaw;
//    int speed_R1 = speed_R1_setup + g_offset_yaw;
//    int speed_R2 = speed_R2_setup + g_offset_yaw;

//    if (speed_L1 > 1000) speed_L1 = 1000;
//    if (speed_L1 < -1000) speed_L1 = -1000;
//    if (speed_L2 > 1000) speed_L2 = 1000;	
//    if (speed_L2 < -1000) speed_L2 = -1000;
//    if (speed_R1 > 1000) speed_R1 = 1000;
//    if (speed_R1 < -1000) speed_R1 = -1000;
//    if (speed_R2 > 1000) speed_R2 = 1000;
//    if (speed_R2 < -1000) speed_R2 = -1000;
//    Motion_Set_Speed(speed_L1, speed_L2, speed_R1, speed_R2);
//}

//// 鎺у埗楹﹀厠绾冲杞皬杞﹁繍鍔ㄧ姸鎬併€?
//// 閫熷害鎺у埗锛歴peed=0~1000銆?
//// 鍋忚埅瑙掕皟鑺傝繍鍔細adjust=1寮€鍚紝=0涓嶅紑鍚€?
//void Mecanum_State(uint8_t state, uint16_t speed, uint8_t adjust)
//{
//    Motion_Set_Yaw_Adjust(adjust);
//    g_speed_setup = speed;
//    switch (state)
//    {
//    case MOTION_STOP:
//        g_speed_setup = 0;
//        Motion_Stop(speed==0?STOP_FREE:STOP_BRAKE);
//        break;
//    case MOTION_RUN:
//        Mecanum_Ctrl(speed, 0, 0, adjust);
//        break;
//    case MOTION_BACK:
//        Mecanum_Ctrl(-speed, 0, 0, adjust);
//        break;
//    case MOTION_LEFT:
//        Mecanum_Ctrl(0, speed, 0, adjust);
//        break;
//    case MOTION_RIGHT:
//        Mecanum_Ctrl(0, -speed, 0, adjust);
//        break;
//    case MOTION_SPIN_LEFT:
//        Motion_Set_Yaw_Adjust(0);
//        Mecanum_Ctrl(0, 0, speed*5, 0);
//        break;
//    case MOTION_SPIN_RIGHT:
//        Motion_Set_Yaw_Adjust(0);
//        Mecanum_Ctrl(0, 0, -speed*5, 0);
//        break;
//    case MOTION_BRAKE:
//        Motion_Stop(STOP_BRAKE);
//        break;
//    default:
//        break;
//    }
//}


