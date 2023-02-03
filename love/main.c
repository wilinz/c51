/********************************************************************
****************** 实验名称：LED 点阵实验(显示数字)
接线说明：
实验现象：下载程序后，8*8LED 点阵显示数字 0
注意事项：LED 点阵旁的 J24 黄色跳线帽短接到 GND 一端
*********************************************************************
******************/
#include "regx52.h"
#include <intrins.h>
#include "LCD1602.h"

typedef unsigned int u16; // 对系统默认数据类型进行重定义
typedef unsigned char u8;
// 定义 74HC595 控制管脚
sbit SRCLK = P3 ^ 6; // 移位寄存器时钟输入
// sbit RCLK = P3 ^ 5;                                                // 存储寄存器时钟输入
sbit SER = P3 ^ 4;                                                 // 串行数据输入
#define LEDDZ_COL_PORT P0                                          // 点阵列控制端口
u8 gled_row[8] = {0x38, 0x44, 0x42, 0x21, 0x21, 0x42, 0x44, 0x38}; // LED 点阵
// 显示图像的行数据
u8 gled_col[8] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe}; // LED 点阵显示
// 数字 0 的列数据
/********************************************************************
***********
* 函 数 名 : delay_10us
* 函数功能 : 延时函数，ten_us=1 时，大约延时 10us
* 输 入 : ten_us
* 输 出 : 无
*********************************************************************
**********/
void delay_10us(u16 ten_us)
{
    while (ten_us--)
        ;
}

void Delay1ms()
{
    unsigned char i, j;
    _nop_();
    i = 2;
    j = 199;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void delay(int n)
{
    u16 i = 0;
    for (i = 0; i < n; i++)
    {
        Delay1ms();
    }
}

/********************************************************************
***********
* 函 数 名 : hc595_write_data(u8 dat)
* 函数功能 : 向 74HC595 写入一个字节的数据
* 输 入 : dat：数据
* 输 出 : 无
*********************************************************************
**********/
void hc595_write_data(u8 dat)
{
    u8 i = 0;
    for (i = 0; i < 8; i++) // 循环 8 次即可将一个字节写入寄存器中
    {
        SER = dat >> 7; // 优先传输一个字节中的高位
        dat <<= 1;      // 将低位移动到高位
        SRCLK = 0;
        delay_10us(1);
        SRCLK = 1;
        delay_10us(1); // 移位寄存器时钟上升沿将端口数据送入寄存器中
    }
    RCLK = 0;
    delay_10us(1);
    RCLK = 1; // 存储寄存器时钟上升沿将前面写入到寄存器的数据输出
}

void showDotMatrix();
#define SMG_A_DP_PORT P0 // 使用宏定义数码管段码口
// 定义数码管位选信号控制脚
sbit LSA = P2 ^ 2;
sbit LSB = P2 ^ 3;
sbit LSC = P2 ^ 4;
// 共阴极数码管显示 0~F 的段码数据
u8 gsmg_code[18] = {0x30, 0x38, 0x3F, 0x3E, 0x79, 0x16E, 0x3F, 0x3E};
// dpgfedcba
// 000110000 I
// 101101110
void showSmg();
/********************************************************************
***********
* 函 数 名 : main
* 函数功能 : 主函数
* 输 入 : 无
* 输 出 : 无
*********************************************************************
**********/
void main()
{

    u8 flag, op = 0;
    LCD_Init();
    LCD_ShowChar(1, 1, 'A');
    while (1)
    {
        if (P3_1 == 0)
        {
            delay(20);
            while (P3_1 == 0)
                ;
            delay(20);
            flag += 1;
        }
        op = flag % 2;
        if (op == 0)
        {
            showDotMatrix();
        }
        else
        {
            showSmg();
        }
    }
}

void showDotMatrix()
{
    u8 i = 0;
    for (i = 0; i < 8; i++) // 循环 8 次扫描 8 行、列
    {
        LEDDZ_COL_PORT = gled_col[i];  // 传送列选数据
        hc595_write_data(gled_row[i]); // 传送行选数据
        delay_10us(100);
        // SMG_A_DP_PORT = 0x00; // 消影
        hc595_write_data(0x00); // 消影
    }
}

void showSmg()
{
    u8 i = 0;
    for (i = 0; i < 8; i++) // 循环 8 次扫描 8 行、列
    {
        // LEDDZ_COL_PORT = gled_col[i];  // 传送列选数据
        // hc595_write_data(gled_row[i]); // 传送行选数据

        switch (i) // 位选
        {
        case 0:
            LSC = 1;
            LSB = 1;
            LSA = 1;
            break;
        case 1:
            LSC = 1;
            LSB = 1;
            LSA = 0;
            break;
        case 2:
            LSC = 1;
            LSB = 0;
            LSA = 1;
            break;
        case 3:
            LSC = 1;
            LSB = 0;
            LSA = 0;
            break;
        case 4:
            LSC = 0;
            LSB = 1;
            LSA = 1;
            break;
        case 5:
            LSC = 0;
            LSB = 1;
            LSA = 0;
            break;
        case 6:
            LSC = 0;
            LSB = 0;
            LSA = 1;
            break;
        case 7:
            LSC = 0;
            LSB = 0;
            LSA = 0;
            break;
        }

        SMG_A_DP_PORT = gsmg_code[i]; // 传送段选数据

        delay_10us(100);

        SMG_A_DP_PORT = 0x00; // 消影
        // hc595_write_data(0x00); // 消影
    }
}