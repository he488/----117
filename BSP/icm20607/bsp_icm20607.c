#include "bsp_icm20607.h"
#include "app_math.h"




attitude_t g_attitude;
icm_data_t g_icm20607;



// 寰垎鏃堕棿锛屼竴鑸彇10姣锛屼笌璋冪敤鐨勬椂闂撮棿闅旀湁鍏?#define DT                  (0.01)
// 鍦ㄦ鏃堕棿鍓嶏紝闄€铻轰华鏁版嵁鐢ㄦ潵妫€娴嬨€傚崟浣嶄负锛?0姣銆傞粯璁ゅ€硷細500
#define ICM_SKIP            500
// 鍦ㄤ笂闈㈢殑鍩虹涓婏紝鍓嶅嚑涓暟鎹笉绋冲畾鐩存帴鎶涘純鎺夈€傚崟浣嶄负锛?0姣銆傞粯璁ゅ€硷細20
#define ICM_ABANDON         20


#define OLD_YAW_WEIGHT       0.65f
#define NEW_YAW_WEIGHT       0.35f


static quaternion_t NumQ = {1, 0, 0, 0};
float vecxZ, vecyZ, veczZ;
float wz_acc_tmp[2];


int16_t icm_gyro_x, icm_gyro_y, icm_gyro_z;
int16_t icm_acc_x, icm_acc_y, icm_acc_z;

float g_icm_test_start_yaw = 0;
u16 g_icm_test = 0;


/* I2C鍐欐暟鎹?*/
static int icm_i2c_write(uint8_t reg, uint8_t data)
{
    return IICwriteByte(ICM_ADDRESS, reg, data);
    //I2C_ICM_ByteWrite(reg, data);
}

/* I2C璇绘暟鎹?*/
static int icm_i2c_read(uint8_t reg, uint8_t *data_buf, uint16_t length)
{
    return IICreadBytes(ICM_ADDRESS, reg, length, data_buf);
    //I2C_ICM_BufferRead(reg, data_buf, length);
}

static void icm_write_gyro_offset(int16_t* data_buf)
{
    s16 buf[3] = {0};
    int abc = 0;
    for (int i = 0; i < 3; i++)
    {
        buf[i] = *data_buf + i;
        abc += icm_i2c_write(XG_OFFS_USRH + i*2, buf[i]&0xff);
        abc += icm_i2c_write(XG_OFFS_USRH + i*2 + 1, (buf[i]<<8)&0xff);
    }
    //DEBUG("ABC=%d", abc);
}

/* 璇诲彇闄€铻轰华X杞村師濮嬫暟鎹?*/
int16_t getRawGyroscopeX(void)
{
    uint8_t val[2] = {0};
    icm_i2c_read(GYRO_XOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 璇诲彇闄€铻轰华Y杞村師濮嬫暟鎹?*/
int16_t getRawGyroscopeY(void)
{
    uint8_t val[2] = {0};
    icm_i2c_read(GYRO_YOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 璇诲彇闄€铻轰华Z杞村師濮嬫暟鎹?*/
int16_t getRawGyroscopeZ(void)
{
    uint8_t val[2] = {0};
    icm_i2c_read(GYRO_ZOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 璇诲彇鍔犻€熷害璁杞村師濮嬫暟鎹?*/
int16_t getRawAccelerationX(void)
{
    uint8_t val[2] = {0};
    icm_i2c_read(ACCEL_XOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 璇诲彇鍔犻€熷害璁杞村師濮嬫暟鎹?*/
int16_t getRawAccelerationY(void)
{
    uint8_t val[2] = {0};
    icm_i2c_read(ACCEL_YOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 璇诲彇鍔犻€熷害璁杞村師濮嬫暟鎹?*/
int16_t getRawAccelerationZ(void)
{
    uint8_t val[2] = {0};
    icm_i2c_read(ACCEL_ZOUT_H, val, 2);//  ACCEL_ZOUT_H
    return ((int16_t)val[0] << 8) + val[1];
}

/* 鍒ゆ柇鏄惁鏄痠cm20607鑺墖 */
void icm20607_who_am_i(void)
{
    uint8_t val = 0, state = 1;
    do
    {
        icm_i2c_read(WHO_AM_I, &val, 1); // 璇籌CM20607鐨処D

				
        if ((ICM20607_ID != val) & state) // 褰揑D涓嶅鏃讹紝鍙姤涓€娆°€?        {
					LCD_ShowString(10,0,"IMU is Detecting!",BLACK,WHITE,16,0);
					LCD_ShowString(10,60,"5s reset to try!",BLACK,WHITE,16,0);
            //DEBUG("IMU ID error! WHO_AM_I=0x%02x\n", val);
            //DEBUG("Please press the reset key to reboot\n");
            state = 0;	
        }
					
    } while (ICM20607_ID != val);
    //DEBUG("WHO_AM_I=0x%02x\n", val);
}

/*
 * ICM20602_GYRO_CONFIG瀵勫瓨鍣? * 璁剧疆涓?0x00 闄€铻轰华閲忕▼涓?卤250 dps     鑾峰彇鍒扮殑闄€铻轰华鏁版嵁闄や互131           鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅涓猴細掳/s
 * 璁剧疆涓?0x08 闄€铻轰华閲忕▼涓?卤500 dps     鑾峰彇鍒扮殑闄€铻轰华鏁版嵁闄や互65.5          鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅涓猴細掳/s
 * 璁剧疆涓?0x10 闄€铻轰华閲忕▼涓?卤1000dps     鑾峰彇鍒扮殑闄€铻轰华鏁版嵁闄や互32.8          鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅涓猴細掳/s
 * 璁剧疆涓?0x18 闄€铻轰华閲忕▼涓?卤2000dps     鑾峰彇鍒扮殑闄€铻轰华鏁版嵁闄や互16.4          鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅涓猴細掳/s

 * ICM20602_ACCEL_CONFIG瀵勫瓨鍣? * 璁剧疆涓?0x00 鍔犻€熷害璁￠噺绋嬩负:卤2g          鑾峰彇鍒扮殑鍔犻€熷害璁℃暟鎹?闄や互16384      鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅锛歡(m/s^2)
 * 璁剧疆涓?0x08 鍔犻€熷害璁￠噺绋嬩负:卤4g          鑾峰彇鍒扮殑鍔犻€熷害璁℃暟鎹?闄や互8192       鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅锛歡(m/s^2)
 * 璁剧疆涓?0x10 鍔犻€熷害璁￠噺绋嬩负:卤8g          鑾峰彇鍒扮殑鍔犻€熷害璁℃暟鎹?闄や互4096       鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅锛歡(m/s^2)
 * 璁剧疆涓?0x18 鍔犻€熷害璁￠噺绋嬩负:卤16g         鑾峰彇鍒扮殑鍔犻€熷害璁℃暟鎹?闄や互2048       鍙互杞寲涓哄甫鐗╃悊鍗曚綅鐨勬暟鎹紝鍗曚綅锛歡(m/s^2)
*/
/* 鍒濆鍖杋cm20607鑺墖 */
void icm20607_init(void)
{
    uint8_t val = 0x0;
    IIC_Init(); // 鍒濆鍖?    delay_ms(10);

    icm_i2c_write(PWR_MGMT_1, 0x80); //澶嶄綅璁惧
    delay_ms(100);
    icm20607_who_am_i();
		
    do
    { //绛夊緟澶嶄綅鎴愬姛
        icm_i2c_read(PWR_MGMT_1, &val, 1);
    } while(0x41 != val);
    icm_i2c_write(PWR_MGMT_1, 0x01);     //鏃堕挓璁剧疆
    icm_i2c_write(PWR_MGMT_2, 0x00);     //寮€鍚檧铻轰华鍜屽姞閫熷害璁?
    icm_i2c_write(CONFIG, 0x01);         //176HZ 1KHZ  0x01
    icm_i2c_write(SMPLRT_DIV, 0x07);     //閲囨牱閫熺巼 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)  0X07
    
     icm_i2c_write(GYRO_CONFIG, 0x18);    //卤2000 dps
//    icm_i2c_write(GYRO_CONFIG, 0x08);    //卤500 dps

     icm_i2c_write(ACCEL_CONFIG, 0x10);   //卤8g

//    icm_i2c_write(ACCEL_CONFIG, 0x00);     // 卤2
		   icm_i2c_write(ACCEL_CONFIG_2, 0x23);
		
}

/* 璇诲彇闄€铻轰华鐨勫師濮嬫暟鎹?*/
void icm_get_gyro(void)
{
//    uint8_t dat[6];

//    icm_i2c_read(GYRO_XOUT_H, dat, 6);
//    icm_gyro_x = (int16_t)(((uint16_t)dat[0] << 8 | dat[1]));
//    icm_gyro_y = (int16_t)(((uint16_t)dat[2] << 8 | dat[3]));
//    icm_gyro_z = (int16_t)(((uint16_t)dat[4] << 8 | dat[5]));
			icm_gyro_x =	getRawGyroscopeX();
			icm_gyro_y = getRawGyroscopeY();
			icm_gyro_z = getRawGyroscopeZ();
		
}

/* 璇诲彇鍔犻€熷害璁＄殑鍘熷鏁版嵁 */
void icm_get_acc(void)
{
//    uint8_t dat[6];

//    icm_i2c_read(ACCEL_XOUT_H, dat, 6);
//    icm_acc_x = (int16_t)(((uint16_t)dat[0] << 8 | dat[1]));
//    icm_acc_y = (int16_t)(((uint16_t)dat[2] << 8 | dat[3]));
//    icm_acc_z = (int16_t)(((uint16_t)dat[4] << 8 | dat[5]));
	
		icm_acc_x = getRawAccelerationX();
		icm_acc_y = getRawAccelerationY();
		icm_acc_z = getRawAccelerationZ();
		
}

/* 璇诲彇ICM濮挎€佽 */
void icm_update_data(void)
{
    icm_get_gyro();
    icm_get_acc();
}

int16_t icm_check_time = 0;
int16_t Deviation_gyro[3] = {0}, Original_gyro[3] = {0};    //闄€铻轰华闈欏樊 鍜屽師濮嬫暟鎹?int16_t Deviation_acc[3] = {0};    // 鍔犻€熷害璁￠潤宸?

// 鑾峰彇闄€铻轰华濮挎€佽
void get_icm_attitude(void)
{
	char bnb[20];
    if (icm_check_time < ICM_SKIP)
    {
        icm_check_time++;
        icm_update_data();
        #if ENABLE_DEBUG_YAW
        DEBUG("icm_check:%d, x:%d, y:%d, z:%d\n", icm_check_time, icm_gyro_x, icm_gyro_y, icm_gyro_z);
        #endif
        if (icm_check_time <= ICM_ABANDON) return;
        Deviation_gyro[0] += icm_gyro_x;
        Deviation_gyro[1] += icm_gyro_y;
        Deviation_gyro[2] += icm_gyro_z;
        Deviation_acc[0] += icm_acc_x;
        Deviation_acc[1] += icm_acc_y;
        Deviation_acc[2] += icm_acc_z;

        if (icm_check_time >= ICM_SKIP)
        {
            // 姹傚钩鍧囧€?            Deviation_gyro[0] = Deviation_gyro[0] / (ICM_SKIP - ICM_ABANDON);
            Deviation_gyro[1] = Deviation_gyro[1] / (ICM_SKIP - ICM_ABANDON);
            Deviation_gyro[2] = Deviation_gyro[2] / (ICM_SKIP - ICM_ABANDON);
            Deviation_acc[0] = Deviation_acc[0] / (ICM_SKIP - ICM_ABANDON);
            Deviation_acc[1] = Deviation_acc[1] / (ICM_SKIP - ICM_ABANDON);
            Deviation_acc[2] = Deviation_acc[2] / (ICM_SKIP - ICM_ABANDON);
            //DEBUG("deviation gyro:x:%d, y:%d, z:%d\n", Deviation_gyro[0], Deviation_gyro[1], Deviation_gyro[2]);
            //DEBUG("deviation acc:x:%d, y:%d, z:%d\n", Deviation_acc[0], Deviation_acc[1], Deviation_acc[2]);
            //Beep_On_Time(30);
            ////icm_check_calibrate();
        }
    }
    else
    {
        icm_update_data();

        g_icm20607.accX = icm_acc_x - Deviation_acc[0];
        g_icm20607.accY = icm_acc_y - Deviation_acc[1];
        g_icm20607.accZ = icm_acc_z - Deviation_acc[2];
        // g_icm20607.accX = icm_acc_x;
        // g_icm20607.accY = icm_acc_y;
        // g_icm20607.accZ = icm_acc_z;
        g_icm20607.gyroX = icm_gyro_x - Deviation_gyro[0];
        g_icm20607.gyroY = icm_gyro_y - Deviation_gyro[1];
        g_icm20607.gyroZ = icm_gyro_z - Deviation_gyro[2];
			
				g_icm20607.gyroX = g_icm20607.gyroX /16.4 + 8; 
        g_icm20607.gyroY = g_icm20607.gyroY /16.4 + 8;
        g_icm20607.gyroZ = g_icm20607.gyroZ /16.4 + 8; //8:鏄瘡涓狪MU鐨勮宸紝闇€瑕佹牴鎹疄闄呮儏鍐佃皟
			
				g_icm20607.accX = g_icm20607.accX/16.4;
				g_icm20607.accY = g_icm20607.accY/16.4;
				g_icm20607.accZ = g_icm20607.accZ/16.4 -66; 

        //get_attitude_angle(&g_icm20607, &g_attitude, DT); // 鍥涘厓绱犵畻娉?//			if (g_icm_test > 0) //涓嶈繘鍘?//        {
//            if (g_icm_test == 1)
//            {
//                icm_test_stop();
//            }
//            g_icm_test--;
//        }

//				//鏄剧ず闄€铻轰华鏁版嵁
//				sprintf(bnb,"gyroX =%d  ",g_icm20607.gyroX);
//        LCD_ShowString(10,15,(u8*)bnb,BLACK,WHITE,16,0);
//				
//				sprintf(bnb,"gyroY =%d  ",g_icm20607.gyroY);
//        LCD_ShowString(10,30,(u8*)bnb,BLACK,WHITE,16,0);
//				
//				sprintf(bnb,"gyroZ =%d  ",g_icm20607.gyroZ);
//        LCD_ShowString(10,45,(u8*)bnb,BLACK,WHITE,16,0);
//				
				//鏄剧ず鍔犻€熷害璁℃暟鎹?				sprintf(bnb,"accX =%d  ",g_icm20607.accX);
        LCD_ShowString(10,15,(u8*)bnb,BLACK,WHITE,16,0);
				
				sprintf(bnb,"accY =%d  ",g_icm20607.accY);
        LCD_ShowString(10,30,(u8*)bnb,BLACK,WHITE,16,0);
				
				sprintf(bnb,"accZ =%d  ",g_icm20607.accZ);
        LCD_ShowString(10,45,(u8*)bnb,BLACK,WHITE,16,0);
//				


				memset(bnb,0,sizeof(bnb));
				delay_ms(100);
    }
}


void icm_test_start(void)
{
    g_icm_test = 500;
}


void icm_test_stop(void)
{
//    int offset = PID_Get_Offset();
//    Motion_Stop();
//    if (offset > -3 && offset < 3)
//    {
//        //Beep_On_Time(50);
//    }
//    //DEBUG("icm_test:%d\n", offset);
}




void icm_check_calibrate(void)
{
    // LED_ON();
    // Deviation_gyro[0];
    u8 dat[6] = {0};
    s16 offset_gyro[3] = {0};
    icm_i2c_read(XG_OFFS_USRH, dat, 6);
    offset_gyro[0] = (int16_t)(((uint16_t)dat[0] << 8 | dat[1]));
    offset_gyro[1] = (int16_t)(((uint16_t)dat[2] << 8 | dat[3]));
    offset_gyro[2] = (int16_t)(((uint16_t)dat[4] << 8 | dat[5]));
    //DEBUG("offset read gyro:%d, %d, %d\n", offset_gyro[0], offset_gyro[1], offset_gyro[2]);

    for (int i = 0; i < 3; i++)
    {
        offset_gyro[i] -= Deviation_gyro[i];
    }
    //DEBUG("offset write gyro:%d, %d, %d\n", offset_gyro[0], offset_gyro[1], offset_gyro[2]);
    icm_write_gyro_offset(offset_gyro);
}


/* 鍥涘厓绱犺幏鍙? dt锛?0MS */
void get_attitude_angle(icm_data_t *p_icm, attitude_t *p_angle, float dt)
{
    vector_t Gravity, Acc, Gyro, AccGravity;
    static vector_t GyroIntegError = {0};
    static float KpDef = 0.8f;
    static float KiDef = 0.0003f;
    float q0_t, q1_t, q2_t, q3_t;
    float NormQuat;
    float HalfTime = dt * 0.5f;

    Gravity.x = 2 * (NumQ.q1 * NumQ.q3 - NumQ.q0 * NumQ.q2);
    Gravity.y = 2 * (NumQ.q0 * NumQ.q1 + NumQ.q2 * NumQ.q3);
    Gravity.z = 1 - 2 * (NumQ.q1 * NumQ.q1 + NumQ.q2 * NumQ.q2);
    // 鍔犻€熷害褰掍竴鍖栵紝
    NormQuat = q_rsqrt(squa(p_icm->accX)+ squa(p_icm->accY) +squa(p_icm->accZ)); 

    //褰掍竴鍚庡彲鍖栦负鍗曚綅鍚戦噺涓嬫柟鍚戝垎閲?    Acc.x = p_icm->accX * NormQuat;
    Acc.y = p_icm->accY * NormQuat;
    Acc.z = p_icm->accZ * NormQuat;

    //鍚戦噺鍙変箻寰楀嚭鐨勫€硷紝鍙変箻鍚庡彲浠ュ緱鍒版棆杞煩闃电殑閲嶅姏鍒嗛噺鍦ㄦ柊鐨勫姞閫熷害鍒嗛噺涓婄殑鍋忓樊
    AccGravity.x = (Acc.y * Gravity.z - Acc.z * Gravity.y);
    AccGravity.y = (Acc.z * Gravity.x - Acc.x * Gravity.z);
    AccGravity.z = (Acc.x * Gravity.y - Acc.y * Gravity.x);

    GyroIntegError.x += AccGravity.x * KiDef;
    GyroIntegError.y += AccGravity.y * KiDef;
    GyroIntegError.z += AccGravity.z * KiDef;

    //瑙掗€熷害铻嶅悎鍔犻€熷害姣斾緥琛ュ伩鍊硷紝涓庝笂闈笁鍙ュ叡鍚屽舰鎴愪簡PI琛ュ伩锛屽緱鍒扮煫姝ｅ悗鐨勮閫熷害鍊?    Gyro.x = p_icm->gyroX * Gyro_Gr + KpDef * AccGravity.x + GyroIntegError.x; //寮у害鍒讹紝姝ゅ琛ュ伩鐨勬槸瑙掗€熷害鐨勬紓绉?    Gyro.y = p_icm->gyroY * Gyro_Gr + KpDef * AccGravity.y + GyroIntegError.y;
    Gyro.z = p_icm->gyroZ * Gyro_Gr + KpDef * AccGravity.z + GyroIntegError.z;
    // 涓€闃堕緳鏍煎簱濉旀硶, 鏇存柊鍥涘厓鏁?    //鐭鍚庣殑瑙掗€熷害鍊肩Н鍒嗭紝寰楀埌涓ゆ濮挎€佽В绠椾腑鍥涘厓鏁颁竴涓疄閮≦0锛屼笁涓櫄閮≦1~3鐨勫€肩殑鍙樺寲
    q0_t = (-NumQ.q1 * Gyro.x - NumQ.q2 * Gyro.y - NumQ.q3 * Gyro.z) * HalfTime;
    q1_t = (NumQ.q0 * Gyro.x - NumQ.q3 * Gyro.y + NumQ.q2 * Gyro.z) * HalfTime;
    q2_t = (NumQ.q3 * Gyro.x + NumQ.q0 * Gyro.y - NumQ.q1 * Gyro.z) * HalfTime;
    q3_t = (-NumQ.q2 * Gyro.x + NumQ.q1 * Gyro.y + NumQ.q0 * Gyro.z) * HalfTime;

    //绉垎鍚庣殑鍊肩疮鍔犲埌涓婃鐨勫洓鍏冩暟涓紝鍗虫柊鐨勫洓鍏冩暟
    NumQ.q0 += q0_t; 
    NumQ.q1 += q1_t;
    NumQ.q2 += q2_t;
    NumQ.q3 += q3_t;

    // 閲嶆柊鍥涘厓鏁板綊涓€鍖栵紝寰楀埌鍗曚綅鍚戦噺涓?    NormQuat = q_rsqrt(squa(NumQ.q0) + squa(NumQ.q1) + squa(NumQ.q2) + squa(NumQ.q3)); //寰楀埌鍥涘厓鏁扮殑妯￠暱
    NumQ.q0 *= NormQuat;                                                               //妯￠暱鏇存柊鍥涘厓鏁板€?    NumQ.q1 *= NormQuat;
    NumQ.q2 *= NormQuat;
    NumQ.q3 *= NormQuat;

    /* 璁＄畻濮挎€佽 */
    // get_angle(p_angle);
    vecxZ = 2 * NumQ.q0 * NumQ.q2 - 2 * NumQ.q1 * NumQ.q3; /*鐭╅樀(3,1)椤?/                                 //鍦扮悊鍧愭爣绯讳笅鐨刋杞寸殑閲嶅姏鍒嗛噺
    vecyZ = 2 * NumQ.q2 * NumQ.q3 + 2 * NumQ.q0 * NumQ.q1; /*鐭╅樀(3,2)椤?/                                 //鍦扮悊鍧愭爣绯讳笅鐨刌杞寸殑閲嶅姏鍒嗛噺
    veczZ = NumQ.q0 * NumQ.q0 - NumQ.q1 * NumQ.q1 - NumQ.q2 * NumQ.q2 + NumQ.q3 * NumQ.q3; /*鐭╅樀(3,3)椤?/ //鍦扮悊鍧愭爣绯讳笅鐨刏杞寸殑閲嶅姏鍒嗛噺

    // p_angle->yaw = atan2f((2 * NumQ.q1 * NumQ.q2 + 2 * NumQ.q0 * NumQ.q3), 
    //     (1 - 2 * (NumQ.q2 * NumQ.q2 + NumQ.q3 * NumQ.q3))) * RtA; //鍋忚埅瑙?    /* 寮у害杞垚瑙掑害 */
    // p_angle->yaw = atan2(2 * (NumQ.q1 * NumQ.q2 + NumQ.q0 * NumQ.q3), 
    //     NumQ.q0 * NumQ.q0 + NumQ.q1 * NumQ.q1 - NumQ.q2 * NumQ.q2 - NumQ.q3 * NumQ.q3) * RtA; //yaw
    // p_angle->pitch = asin(vecxZ) * RtA;             //淇话瑙?    // p_angle->roll = atan2f(vecyZ, veczZ) * RtA;     //妯粴瑙?    
    #if ENABLE_ROLL_PITCH
    p_angle->pitch = asin(vecxZ);             //淇话瑙?    p_angle->roll = atan2f(vecyZ, veczZ);     //妯粴瑙?    #endif

	
	
    p_angle->yaw = atan2(2 * (NumQ.q1 * NumQ.q2 + NumQ.q0 * NumQ.q3), 
        NumQ.q0 * NumQ.q0 + NumQ.q1 * NumQ.q1 - NumQ.q2 * NumQ.q2 - NumQ.q3 * NumQ.q3); //鍋忚埅瑙?

    static float old_yaw = 0;
    p_angle->yaw = OLD_YAW_WEIGHT * old_yaw + NEW_YAW_WEIGHT * p_angle->yaw;
    old_yaw = p_angle->yaw;
}
