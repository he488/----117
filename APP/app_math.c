#include "app_math.h"

//绉佹湁鍙橀噺鍖?
const float MY_PI = 3.1415926535;
const float RtA = 57.2957795f;   //寮у害锛坮adian锛夎浆瑙掑害锛坅ngle锛夌殑甯告暟
const float AtR = 0.0174532925f; //瑙掑害杞姬搴︾殑甯告暟
// 闄€铻轰华缂╂斁姣斾緥ratio锛欸yro_Gr = 1 / ratio / RtA
const float Gyro_G = 0.03051756f * 2;
// const float Gyro_Gr = 0.0005326f * 2; // 瑙掗€熷害鍙樻垚寮у害	姝ゅ弬鏁板搴旈檧铻?000搴︽瘡绉?.0010653f
const float Gyro_Gr = 0.00013323f * 2; // 瑙掗€熷害鍙樻垚寮у害	姝ゅ弬鏁板搴旈檧铻?00搴︽瘡绉?.00026646f
const float PI_2 = 1.570796f;         //PI/2

/* 杩斿洖姝ｅ鸡鍊硷紝x涓鸿搴﹀€?*/
float sine(float x) // (-M_PI , M_PI) ???? 0.0005
{
    float Q = 0.775f;
    float P = 0.225f;
    float B = 1.273239544;
    float tmp_c = -0.405284f;
    float y = B * x + tmp_c * x * fabs(x);
    return (Q * y + P * y * fabs(y));
}

/* 璁＄畻浣欏鸡鍊硷紝x涓鸿搴?/
float cosine(float x)
{
    return sine(x + MY_PI / 2);
}

/* 杩斿洖鍙嶆鍒囧€硷紝杈撳叆鍙嶆鍒囨暟鎹?*/
float arctan(float x) //  (-1 , +1)
{
    float t = x;
    float result = 0;
    float X2 = x * x;
    unsigned char cnt = 1;
    do
    {
        result += t / ((cnt << 1) - 1);
        t = -t;
        t *= X2;
        cnt++;
    } while(cnt <= 6);

    return result;
}

/* 鍙嶆寮︽暟鎹?*/
float arcsin(float x)
{
    float d = 1;
    float t = x;
    unsigned char cnt = 1;
    float result = 0;
    float X2 = x * x;

    if(x >= 1.0f)
    {
        return PI_2;
    }
    if(x <= -1.0f)
    {
        return -PI_2;
    }
    do
    {
        result += t / (d * ((cnt << 1) - 1));
        t *= X2 * ((cnt << 1) - 1); //
        d *= (cnt << 1);            //2 4 6 8 10 ...
        cnt++;
    } while(cnt <= 6);

    return result;
}

/* 璁＄畻1/sqrt(x) */
float q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));
    return y;
}

/* 鏁版嵁闄愬箙 */
float data_limit(float data, float toplimit, float lowerlimit)
{
    if(data > toplimit)
        data = toplimit;
    else if(data < lowerlimit)
        data = lowerlimit;
    return data;
}


int math_abs(int a)
{
	int temp;
	if (a < 0)
		temp = -a;
	else
		temp = a;
	return temp;
}

