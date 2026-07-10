#ifndef __APP_MECANUM_H__
#define __APP_MECANUM_H__

#include "stdint.h"
#include "AllHeader.h"

// 鍥涜疆杞﹀簳鐩橀棿璺?
#define ROBOT_WIDTH                  (169.0f)    // 姣背
#define ROBOT_LENGTH                 (160.11f)   // 姣背

// 鍥涜疆搴曠洏鐢垫満闂磋窛涔嬪拰鐨勪竴鍗?
// #define MECANUM_APB               ((ROBOT_WIDTH + ROBOT_LENGTH)/2.0f)
#define MECANUM_APB                  (164.555f)

// 鍥涜疆杞竴鏁村湀鐨勪綅绉伙紝鍗曚綅涓簃m
#define MECANUM_CIRCLE_MM            (204.203f)
#define MECANUM_MINI_CIRCLE_MM       (204.203f)


// 鍥涜疆搴曠洏鐢垫満闂磋窛涔嬪拰鐨勪竴鍗?
#define MECANUM_MAX_APB              (214.1f)
// 鍥涜疆杞竴鏁村湀鐨勪綅绉伙紝鍗曚綅涓篗M
#define MECANUM_MAX_CIRCLE_MM        (251.327f)

// 鍥涜疆澶у簳鐩樼數鏈洪棿璺濅箣鍜岀殑涓€鍗?
#define MECANUM_MINI_APB             (174.5f)


// X3 PLUS灏忚溅閫熷害闄愬埗
#define CAR_X3_PLUS_MAX_SPEED        (700)




void Mecanum_Ctrl(int16_t V_x, int16_t V_y, int16_t V_z, uint8_t adjust);
void Mecanum_State(uint8_t state, uint16_t speed, uint8_t adjust);

void Mecanum_Yaw_Calc(float yaw);

#endif /* __APP_MECANUM_H__ */

