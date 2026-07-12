#include "ioi2c.h"



//IO方向设置
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<12;}

//IO操作函数	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //输入SDA 

// 等待引脚的时间，可根据芯片时钟修改，只要符合通讯要求即可。
#define DELAY_FOR_COUNT      50


static void Delay_For_Pin(vu8 nCount) 
{
    uint8_t i = 0;
    for(; nCount != 0; nCount--)
    {
        for (i = 0; i < DELAY_FOR_COUNT; i++); 
    }
}


/**
 * @Brief: 初始化I2C对应的接口引脚
 * @Note:
 * @Parm: void
 * @Retval: void
 */
void IIC_Init(void)
{
    RCC->APB2ENR |= 1 << 3;   //先使能外设IO PORTB时钟
    GPIOB->CRH &= 0XFFFF00FF; //PB 10/11 推挽输出
    GPIOB->CRH |= 0X00003300;
}

/**
 * @Brief: 产生IIC起始信号
 * @Note:
 * @Parm: void
 * @Retval: void
 */
int IIC_Start(void)
{
    SDA_OUT(); //sda线输出
    IIC_SDA = 1;
    if (!READ_SDA)
        return 0;
    IIC_SCL = 1;
    Delay_For_Pin(1);
    IIC_SDA = 0; //START:when CLK is high,DATA change form high to low
    if (READ_SDA)
        return 0;
    Delay_For_Pin(2);
    IIC_SCL = 0; //钳住I2C总线，准备发送或接收数据
    return 1;
}

/**
 * @Brief: 产生IIC停止信号
 * @Note:
 * @Parm: void
 * @Retval: void
 */
void IIC_Stop(void)
{
    SDA_OUT(); //sda线输出
    IIC_SCL = 0;
    IIC_SDA = 0; //STOP:when CLK is high DATA change form low to high
    Delay_For_Pin(2);
    IIC_SCL = 1;
    Delay_For_Pin(1);
    IIC_SDA = 1; //发送I2C总线结束信号
    Delay_For_Pin(2);
}

/**
 * @Brief: 等待应答信号到来
 * @Note:
 * @Parm:
 * @Retval: 1:接收应答失败 | 0:接收应答成功
 */
int IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN(); //SDA设置为输入
    IIC_SDA = 1;
    Delay_For_Pin(1);
    IIC_SCL = 1;
    Delay_For_Pin(1);
    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 50)
        {
            IIC_Stop();
            return 0;
        }
        Delay_For_Pin(1);
    }
    IIC_SCL = 0; //时钟输出0
    return 1;
}

/**
 * @Brief: 产生ACK应答
 * @Note:
 * @Parm: void
 * @Retval: void
 */
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    Delay_For_Pin(1);
    IIC_SCL = 1;
    Delay_For_Pin(1);
    IIC_SCL = 0;
}

/**
 * @Brief: 产生NACK应答
 * @Note:
 * @Parm: void
 * @Retval: void
 */
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    Delay_For_Pin(1);
    IIC_SCL = 1;
    Delay_For_Pin(1);
    IIC_SCL = 0;
}

/**
 * @Brief: IIC发送一个字节
 * @Note:
 * @Parm: void
 * @Retval: void
 */
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL = 0; //拉低时钟开始数据传输
    for (t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        Delay_For_Pin(1);
        IIC_SCL = 1;
        Delay_For_Pin(1);
        IIC_SCL = 0;
        Delay_For_Pin(1);
    }
}

uint8_t g_addr[128] = {0};


void i2c_scanf_addr(void)
{
    uint8_t i2c_count = 0;
    for (int i = 1; i < 128; i++)
    {
        if (!IIC_Start())
        {
            printf("External IIC Start Error:%d\n", i);
            return;
        }
        IIC_Send_Byte(i << 1);
        if (!IIC_Wait_Ack())
        {
            if (i == 127)
            {
                printf("External IIC Scanf End, Count=%d\n", i2c_count);
            }
            continue;
        }
        IIC_Stop();
        i2c_count++;
        printf("External IIC Found address:0x%2x\n", i);
        g_addr[i] = i;
    }
}


/**
 * @Brief: I2C写数据函数
 * @Note:
 * @Parm: addr:I2C地址 | reg:寄存器 | len:数据长度 | data:数据指针
 * @Retval: 0:停止 | 1:连续写
 */

u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data)
{
    u8 count = 0;
    do
    {
        IIC_Start();
        if( IIC_Send_Byte(dev | 0x00) ) //发送设备地址+写信号
        {
            IIC_Stop();
            return 1;
        } 
        if( IIC_Send_Byte(reg) )   //发送寄存器地址
        {
            IIC_Stop();
            return 1;
        }
        for(count = 0; count < length; count++)
        {
            if( IIC_Send_Byte(data[count]) )
            {
                IIC_Stop();
                return 1;
            }
        }   
        IIC_Stop(); 
    }while(0);
    return 0;
}

u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
{
    u8 count = 0;
    do
    {
        IIC_Start();
        if( IIC_Send_Byte(dev | 0x00) )
        {
            IIC_Stop();
            return 1;
        } 
        if( IIC_Send_Byte(reg) )   //发送寄存器地址
        {
            IIC_Stop();
            return 1;
        }
        IIC_Start();
        if( IIC_Send_Byte(dev | 0x01) )
        {
            IIC_Stop();
            return 1;
        } 
        for(count = 0; count < length; count++)
        {
            if(count == length-1)
                data[count] = IIC_Read_Byte(0);
            else
                data[count] = IIC_Read_Byte(1);
        }   
        IIC_Stop(); 
    }while(0);
    return 0;
}


unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
{
    return IICwriteBytes(dev, reg, 1, &data);
}

int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    return IICwriteBytes(addr, reg, len, data);
}
int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    return IICreadBytes(addr, reg, len, buf);
}

/**
 * @Brief: IIC读取一个字节
 * @Note:
 * @Parm: ack:1时，发送ACK，0时，发送nACK
 * @Retval: 接收到的数据
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN(); //SDA设置为输入
    for (i = 0; i < 8; i++)
    {
        IIC_SCL = 0;
        Delay_For_Pin(1);
        IIC_SCL = 1;
        Delay_For_Pin(1);
        receive <<= 1;
        if (READ_SDA)
            receive++;
        Delay_For_Pin(1);
    }
    if (!ack)
        IIC_NAck(); //发送nACK
    else
        IIC_Ack();  //发送ACK
    return receive;
}