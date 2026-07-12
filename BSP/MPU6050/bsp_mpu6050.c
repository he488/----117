#include "bsp_mpu6050.h"
#include "app_math.h"

attitude_t g_attitude;
icm_data_t g_mpu6050;

// 微分时间，一般取10毫秒，与调用的时间间隔有关
#define DT (0.01)
// 在此时间前，陀螺仪数据用来检测。单位为：10毫秒。默认值：500
#define MPU_SKIP 500
// 在上面的基础上，前几个数据不稳定直接抛弃掉。单位为：10毫秒。默认值：20
#define MPU_ABANDON 20

#define OLD_YAW_WEIGHT 0.65f
#define NEW_YAW_WEIGHT 0.35f

static quaternion_t NumQ = {1, 0, 0, 0};
float vecxZ, vecyZ, veczZ;
float wz_acc_tmp[2];

int16_t mpu_gyro_x, mpu_gyro_y, mpu_gyro_z;
int16_t mpu_acc_x, mpu_acc_y, mpu_acc_z;

float g_mpu_test_start_yaw = 0;
u16 g_mpu_test = 0;

/* I2C写数据 */
static int mpu_i2c_write(uint8_t reg, uint8_t data)
{
    return IICwriteByte(MPU6050_ADDR, reg, data);
}

/* I2C读数据 */
static int mpu_i2c_read(uint8_t reg, uint8_t *data_buf, uint16_t length)
{
    return IICreadBytes(MPU6050_ADDR, reg, length, data_buf);
}

static void mpu_write_gyro_offset(int16_t *data_buf)
{
    s16 buf[3] = {0};
    int abc = 0;
    for (int i = 0; i < 3; i++)
    {
        buf[i] = *data_buf + i;
        abc += mpu_i2c_write(XG_OFFS_USRH + i * 2, buf[i] & 0xff);
        abc += mpu_i2c_write(XG_OFFS_USRH + i * 2 + 1, (buf[i] << 8) & 0xff);
    }
}

/* 读取陀螺仪X轴原始数据 */
int16_t getRawGyroscopeX(void)
{
    uint8_t val[2] = {0};
    mpu_i2c_read(GYRO_XOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 读取陀螺仪Y轴原始数据 */
int16_t getRawGyroscopeY(void)
{
    uint8_t val[2] = {0};
    mpu_i2c_read(GYRO_YOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 读取陀螺仪Z轴原始数据 */
int16_t getRawGyroscopeZ(void)
{
    uint8_t val[2] = {0};
    mpu_i2c_read(GYRO_ZOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 读取加速度计X轴原始数据 */
int16_t getRawAccelerationX(void)
{
    uint8_t val[2] = {0};
    mpu_i2c_read(ACCEL_XOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 读取加速度计Y轴原始数据 */
int16_t getRawAccelerationY(void)
{
    uint8_t val[2] = {0};
    mpu_i2c_read(ACCEL_YOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/* 读取加速度计Z轴原始数据 */
int16_t getRawAccelerationZ(void)
{
    uint8_t val[2] = {0};
    mpu_i2c_read(ACCEL_ZOUT_H, val, 2);
    return ((int16_t)val[0] << 8) + val[1];
}

/*============================================================================
 * 判断是否是 MPU6050 芯片
 * MPU6050 的 WHO_AM_I 寄存器（0x75）返回值：
 *   - AD0 = GND → 0x68
 *   - AD0 = VCC → 0x69
 * 本配置使用 AD0 = GND，期望值 = 0x68
 *============================================================================*/
void mpu6050_who_am_i(void)
{
    uint8_t val = 0, state = 1;
    do
    {
        mpu_i2c_read(WHO_AM_I, &val, 1); // 读 MPU6050 的 ID

        if ((MPU6050_ID != val) & state) // 当ID不对时，只报一次。
        {
            LCD_ShowString(10, 0, "MPU6050 Detecting!", BLACK, WHITE, 16, 0);
            LCD_ShowString(10, 60, "5s reset to try!", BLACK, WHITE, 16, 0);
            state = 0;
        }

    } while (MPU6050_ID != val);
}

/*
 * MPU6050_GYRO_CONFIG寄存器
 * 设置为0x00 陀螺仪量程为:±250 dps     获取到的陀螺仪数据除以131           可以转化为带物理单位的数据，单位为：°/s
 * 设置为0x08 陀螺仪量程为:±500 dps     获取到的陀螺仪数据除以65.5          可以转化为带物理单位的数据，单位为：°/s
 * 设置为0x10 陀螺仪量程为:±1000dps     获取到的陀螺仪数据除以32.8          可以转化为带物理单位的数据，单位为：°/s
 * 设置为0x18 陀螺仪量程为:±2000dps     获取到的陀螺仪数据除以16.4          可以转化为带物理单位的数据，单位为：°/s

 * MPU6050_ACCEL_CONFIG寄存器
 * 设置为0x00 加速度计量程为:±2g          获取到的加速度计数据 除以16384      可以转化为带物理单位的数据，单位：g(m/s^2)
 * 设置为0x08 加速度计量程为:±4g          获取到的加速度计数据 除以8192       可以转化为带物理单位的数据，单位：g(m/s^2)
 * 设置为0x10 加速度计量程为:±8g          获取到的加速度计数据 除以4096       可以转化为带物理单位的数据，单位：g(m/s^2)
 * 设置为0x18 加速度计量程为:±16g         获取到的加速度计数据 除以2048       可以转化为带物理单位的数据，单位：g(m/s^2)
*/
/* 初始化 MPU6050 芯片 */
void mpu6050_init(void)
{
    uint8_t val = 0x0;
    GPIO_InitTypeDef GPIO_InitStructure;

    IIC_Init(); // 初始化 I2C 通信引脚（PB10-SCL, PB11-SDA）
    delay_ms(10);

    /*========== MPU6050 INT 中断引脚初始化（PB13） ==========*/
    /* MPU6050 数据就绪时，INT 引脚输出高电平脉冲           */
    /* STM32 端配置为浮空输入，读取 INT 引脚电平判断数据状态 */
    RCC_APB2PeriphClockCmd(MPU6050_INT_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = MPU6050_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MPU6050_INT_PORT, &GPIO_InitStructure);
    delay_ms(10);

    mpu_i2c_write(PWR_MGMT_1, 0x80); // 复位设备（写入 0x80 触发复位）
    delay_ms(100);
    mpu6050_who_am_i();              // 检查芯片 ID

    /* 等待复位完成：PWR_MGMT_1 恢复为默认值 0x40（睡眠模式，内部振荡器） */
    do
    {
        mpu_i2c_read(PWR_MGMT_1, &val, 1);
    } while (0x40 != val);

    mpu_i2c_write(PWR_MGMT_1, 0x01); // 退出睡眠，时钟源 = PLL（X轴陀螺参考）
    mpu_i2c_write(PWR_MGMT_2, 0x00); // 开启陀螺仪和加速度计（全部使能）
    mpu_i2c_write(CONFIG, 0x01);     // 数字低通滤波器：176Hz，采样率 1kHz
    mpu_i2c_write(SMPLRT_DIV, 0x07); // 采样率 = 1kHz / (1 + 7) = 125Hz

    mpu_i2c_write(GYRO_CONFIG, 0x18); // 陀螺仪量程：±2000 dps
    //    mpu_i2c_write(GYRO_CONFIG, 0x08);    // 陀螺仪量程：±500 dps

    mpu_i2c_write(ACCEL_CONFIG, 0x10); // 加速度计量程：±8g

    //    mpu_i2c_write(ACCEL_CONFIG, 0x00);     // 加速度计量程：±2g
    mpu_i2c_write(ACCEL_CONFIG_2, 0x23); // 加速度计低通滤波器：44Hz
}

/* 读取陀螺仪的原始数据 */
void mpu_get_gyro(void)
{
    mpu_gyro_x = getRawGyroscopeX();
    mpu_gyro_y = getRawGyroscopeY();
    mpu_gyro_z = getRawGyroscopeZ();
}

/* 读取加速度计的原始数据 */
void mpu_get_acc(void)
{
    mpu_acc_x = getRawAccelerationX();
    mpu_acc_y = getRawAccelerationY();
    mpu_acc_z = getRawAccelerationZ();
}

/* 读取 MPU6050 姿态角 */
void mpu_update_data(void)
{
    mpu_get_gyro();
    mpu_get_acc();
}

int16_t mpu_check_time = 0;
int16_t Deviation_gyro[3] = {0}, Original_gyro[3] = {0}; // 陀螺仪静差 和原始数据
int16_t Deviation_acc[3] = {0};                          // 加速度计静差

// 获取 MPU6050 姿态角（含静差校准）
void get_mpu_attitude(void)
{
    uint8_t buf[20];
    if (mpu_check_time < MPU_SKIP)
    {
        mpu_check_time++;
        mpu_update_data();
#if ENABLE_DEBUG_YAW
        DEBUG("mpu_check:%d, x:%d, y:%d, z:%d\n", mpu_check_time, mpu_gyro_x, mpu_gyro_y, mpu_gyro_z);
#endif
        if (mpu_check_time <= MPU_ABANDON)
            return;
        Deviation_gyro[0] += mpu_gyro_x;
        Deviation_gyro[1] += mpu_gyro_y;
        Deviation_gyro[2] += mpu_gyro_z;
        Deviation_acc[0] += mpu_acc_x;
        Deviation_acc[1] += mpu_acc_y;
        Deviation_acc[2] += mpu_acc_z;

        if (mpu_check_time >= MPU_SKIP)
        {
            // 求平均值（静差校准）
            Deviation_gyro[0] = Deviation_gyro[0] / (MPU_SKIP - MPU_ABANDON);
            Deviation_gyro[1] = Deviation_gyro[1] / (MPU_SKIP - MPU_ABANDON);
            Deviation_gyro[2] = Deviation_gyro[2] / (MPU_SKIP - MPU_ABANDON);
            Deviation_acc[0] = Deviation_acc[0] / (MPU_SKIP - MPU_ABANDON);
            Deviation_acc[1] = Deviation_acc[1] / (MPU_SKIP - MPU_ABANDON);
            Deviation_acc[2] = Deviation_acc[2] / (MPU_SKIP - MPU_ABANDON);
        }
    }
    else
    {
        mpu_update_data();

        // 减去静差，得到真实值
        g_mpu6050.accX = mpu_acc_x - Deviation_acc[0];
        g_mpu6050.accY = mpu_acc_y - Deviation_acc[1];
        g_mpu6050.accZ = mpu_acc_z - Deviation_acc[2];
        g_mpu6050.gyroX = mpu_gyro_x - Deviation_gyro[0];
        g_mpu6050.gyroY = mpu_gyro_y - Deviation_gyro[1];
        g_mpu6050.gyroZ = mpu_gyro_z - Deviation_gyro[2];

        // 转换为物理单位（±2000dps 量程：除以 16.4）
        // +8 是每个 IMU 的个体误差补偿，根据实际情况调整
        g_mpu6050.gyroX = g_mpu6050.gyroX / 16.4 + 8;
        g_mpu6050.gyroY = g_mpu6050.gyroY / 16.4 + 8;
        g_mpu6050.gyroZ = g_mpu6050.gyroZ / 16.4 + 8;

        // 加速度计转换（±8g 量程：除以 4096，再乘以 1000 转为 mg）
        // -66 是加速度计零偏补偿
        g_mpu6050.accX = g_mpu6050.accX / 16.4;
        g_mpu6050.accY = g_mpu6050.accY / 16.4;
        g_mpu6050.accZ = g_mpu6050.accZ / 16.4 - 66;

        // 显示加速度计数据到 LCD
        sprintf(buf, "accX =%d  ", g_mpu6050.accX);
        LCD_ShowString(10, 15, (uint8_t *)buf, BLACK, WHITE, 16, 0);

        sprintf(buf, "accY =%d  ", g_mpu6050.accY);
        LCD_ShowString(10, 30, (uint8_t *)buf, BLACK, WHITE, 16, 0);

        sprintf(buf, "accZ =%d  ", g_mpu6050.accZ);
        LCD_ShowString(10, 45, (uint8_t *)buf, BLACK, WHITE, 16, 0);
    }
}