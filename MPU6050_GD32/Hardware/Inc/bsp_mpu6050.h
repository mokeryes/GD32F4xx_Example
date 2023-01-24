#ifndef _BSP_MPU6050_H
#define _BSP_MPU6050_H


#include "gd32f4xx.h"
#include "systick.h"
#include "bsp_i2c.h"


#define RAD_TO_DEG              57.295779513082320876798154814105

/**
 * 设备写地址 0xD0
 * 设备读地址 0xD1
 */
#define MPU6050_ADDR            0xD0
#define WHO_AM_I_REG            0x75

/**
 *                       MPU6050寄存器地址表
 *  -------------------------------------------------------------------
 * | Register | Bit7 | Bit6 | Bit 5 | Bit4 | Bit3 | Bit2 | Bit1 | Bit0 |
 *  -------------------------------------------------------------------
 * |    -     |  -   |   -  |   -   |  -   |   -  |   -  |  -   |  -   |
 *  -------------------------------------------------------------------
 */

/**
 * 陀螺仪数据输出频率设置 
 * DLPF(低通滤波)关闭时: Gyroscope Output Rate = 8kHz, DLPF_CFG = 0 or 7
 * DLPF(低通滤波)启动时: Gyroscope Output Rate = 1kHz
 * Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
*/
#define SMPLRT_DIV_REG          0x19
    #define SMPLRT_DIV_0        0x00    // 默认原始频率输出数据

/** 
 * 配置同步引脚输出内容与陀螺仪和加速度传感器低通滤波参数 
 * 同步引脚配置：
 *     EXT_SYNC_SET[2:0] => CONFIG_REG[Bit5:Bit3]
 *     -----------------------------------
 *    | EXT_SYNC_SET | FSYNC Bit Location |
 *    |--------------|--------------------|
 *    |       0      |  input disabled    |
 *    |       1      |  TEMP_OUT_L[0]     |
 *    |       2      |  GYRO_XOUT_L[0]    |
 *    |       3      |  GYRO_YOUT_L[0]    |
 *    |       4      |  GYRO_ZOUT_L[0]    |
 *    |       5      |  ACCEL_XOUT_L[0]   |
 *    |       6      |  ACCEL_YOUT_L[0]   |
 *    |       7      |  ACCEL_ZOUT_L[0]   |
 *     -----------------------------------
 * 低通滤波配置:
 *     DLPF_CFG[2:0] => CONFIG_REG[Bit2:Bit0]
 *     ----------------------------------------------------------------------------
 *    | DLPF_CFG | Accelerometer (Fs = 1kHz) |             Gyroscope               |
 *    |          |-----------------------------------------------------------------|
 *    |          | Bandwidth(Hz) | Delay(ms) | Bandwidth(Hz) | Delay(ms) | Fs(kHz) |
 *    |          |-----------------------------------------------------------------|
 *    |    0     |      260      |    0      |       256     |    0.98   |    8    |
 *    |    1     |      184      |    2.0    |       188     |    1.9    |    1    |
 *    |    2     |       94      |    3.0    |        98     |    2.8    |    1    |
 *    |    3     |       44      |    4.9    |        42     |    4.8    |    1    |
 *    |    4     |       21      |    8.5    |        20     |    8.3    |    1    |
 *    |    5     |       10      |   13.8    |        10     |   13.4    |    1    |
 *    |    6     |        5      |   19.0    |         5     |   18.6    |    1    |
 *    |    7     |            RESERVED       |            RESERVED       |    8    |
 *     ----------------------------------------------------------------------------
 */
#define CONFIG_REG              0x1A
    #define CONFIG_VALUE        0x06 // 同步引脚输出不启用，Accel Bandwidth 5Hz，Delay 19.0ms，Gyro Bandwidth 5Hz，Delay 18.6ms, Fs 1kHz

/**
 * 触发陀螺仪自检以及配置陀螺仪的采样范围
 *     X轴角度自检:
 *         XG_ST => GYRO_CONFIG_REG[Bit7]
 *     Y轴角度自检:
 *         YG_ST => GYRO_CONFIG_REG[Bit6]
 *     Z轴角度自检:
 *         YZ_ST => GYRO_CONFIG_REG[Bit5]
 *     [note]陀螺仪自检:
 *         芯片内部通过计算陀螺仪灵敏度公差是否在±3%之间来判断陀螺仪工作正常与否，
 *         因此在芯片自检过程中应保持姿态稳定。
 *         Self-test response = Sensor output with self-test enabled - Sensor output without self-test enabled
 * 陀螺仪采样范围配置:
 *     FS_SEL => GYRO_CONFIG_REG[Bit4:Bit3]
 *      ---------------------------
 *     | FS_SEL | Full Scale Range |
 *      ---------------------------
 *     |   0    |      ±250°/s     |
 *     |   1    |      ±500°/s     |
 *     |   2    |     ±1000°/s     |
 *     |   3    |     ±2000°/s     |
 *      ---------------------------
 */
#define GYRO_CONFIG_REG         0x1B
    #define GYRO_CONFIG_VALUE   0xE0 // 陀螺仪所有轴自检，采样范围设置为：±250°/s

/**
 * 陀螺仪X、Y、Z轴的高位、低位数据读取寄存器
 */
#define GYRO_XOUT_H_REG         0x43 
#define GYRO_XOUT_L_REG         0x44
#define GYRO_YOUT_H_REG         0x45 
#define GYRO_YOUT_L_REG         0x46
#define GYRO_ZOUT_H_REG         0x47
#define GYRO_ZOUT_L_REG         0x48

/**
 * 触发加速度传感器自检以及配置加速度传感器采样范围，同时可配置高通滤波器
 *     X轴加速度自检：
 *         XA_ST => ACCEL_CONFIG_REG[Bit7]
 *     Y轴加速度自检：
 *         YA_ST => ACCEL_CONFIG_REG[Bit6]
 *     Z轴加速度自检：
 *         ZA_ST => ACCEL_CONFIG_REG[Bit5]
 *     [note]加速度传感器自检：
 *         芯片内部通过计算稳定状态时的加速度值，在300mg~950mg之间则说明加速度传感器工作正常。
 *         因此在芯片自检过程中应保持姿态稳定。
 *         Self-test response = Sensor output with self-test enabled - Sensor output without self-test enabled
 * 加速度传感器采样范围配置：
 *     AFS_SEL => ACCEL_CONFIG_REG[Bit4:Bit3]
 *      --------------------------------------------------
 *     | AFS_SEL  | Full Scale Range |  LSB Sensitivity   |
 *     |-----------------------------|--------------------|
 *     |    0     |        ± 2g      |   16384 LSB/mg     | 
 *     |    1     |        ± 4g      |    8192 LSB/mg     | 
 *     |    2     |        ± 8g      |    4096 LSB/mg     | 
 *     |    3     |       ± 16g      |    2048 LSB/mg     | 
 *      --------------------------------------------------
 * 高通滤波器配置：
 *     ACCEL_HPF => ACCEL_CONFIG_REG[Bit2:Bit0]
 *      ---------------------------------------------
 *     | ACCEL_HPF | Filter Mode | Cut-off Frequency |
 *     |---------------------------------------------|
 *     |     0     |    Reset    |       None        |
 *     |     1     |    On       |       5Hz         |
 *     |     2     |    On       |       2.5Hz       |
 *     |     3     |    On       |       1.25Hz      |
 *     |     4     |    On       |       0.63Hz      |
 *     |     7     |    Hold     |       None        |
 *      ---------------------------------------------
 */
#define ACCEL_CONFIG_REG        0x1C
    #define ACCEL_CONFIG_VALUE  0xE0 // 加速度传感器自检，采样范围±2g，高通滤波器关闭

/**
 * 加速度传感器X、Y、Z轴的高位、低位数据读取寄存器
 */
#define ACCEL_XOUT_H_REG        0x3B
#define ACCEL_XOUT_L_REG        0x3C
#define ACCEL_YOUT_H_REG        0x3D
#define ACCEL_YOUT_L_REG        0x3E
#define ACCEL_ZOUT_H_REG        0x3F
#define ACCEL_ZOUT_L_REG        0x40

/**
 * 自由落体运动检测临界值设置
 * 同时对加速度三个轴的加速度区绝对值，若大于某值，则判定为自由落体运动
 * FF_THR => FF_THR_REG[Bit7:Bit0]
 */
#define FF_THR_REG              0x1D

/**
 * 自由落体运动检测在1ms内的加速度变化
 * 设置三轴加速度绝对值在1ms内的变化临界值，，若超过该临界值，则判定为自由落体运动 
 * FF_DUR => FF_DUR_REG[Bit7:Bit0]
 */
#define FF_DUR_REG              0x1E

/**
 * 配置是否复位、激活、循环复位读取设备，配置是否关闭温度传感器，配置时钟
 * 复位开关寄存器，复位后需要延时一会儿
 *     DEVICE_RESET => PWR_MGMT_1_REG[Bit7]
 * 睡眠模式/待机模式开关寄存器：
 *     SLEEP => PWR_MGMT_1_REG[Bit6]
 * 低功耗单数据读取模式，可在 PWR_MGMT_2_REG 进行具体设置：
 *     CYCLE => PWR_MGMT_1_REG[Bit5]
 * 是否关闭温度传感器：
 *     TEMP_DIS => PWR_MGMT_1_REG[Bit3]
 * 时钟选择：
 *     CLKSEL => PWR_MGMT_1_REG[Bit2:Bit0]
 *      ------------------------------------------------------------------
 *     | CLKSEL |                    Clock Source                         |
 *      ------------------------------------------------------------------
 *     |   0    | Internal 8MHz Oscillator                                |
 *     |   1    | PLL with X axis gyroscope reference                     |
 *     |   2    | PLL with Y axis gyroscope reference                     |
 *     |   3    | PLL with Z axis gyroscope reference                     |
 *     |   4    | PLL with external 32.768kHz reference                   |
 *     |   5    | PLL with external 19.2MHz reference                     |
 *     |   6    | Reserved                                                |
 *     |   7    | Stops the clock and keeps the timing generator in reset |
 *      ------------------------------------------------------------------
 */
#define PWR_MGMT_1_REG          0x6B
   #define PWR_MGMT_1_RESET     0x80
   #define PWR_MGMT_1_WAKEUP    0x00 // 低功耗模式关闭、开启温度传感器、选择内部8MHz时钟作为时钟源

/* MPU6050 数据结构 */
typedef struct {
    uint16_t Accel_X_RAW;
    uint16_t Accel_Y_RAW;
    uint16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;

    uint16_t Gyro_X_RAW;
    uint16_t Gyro_Y_RAW;
    uint16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;

    float Temperature;

    double KalmanAngleX;
    double KalmanAngleY;
} MPU6050_t;

/* 卡尔曼滤波数据结构 */
typedef struct {
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
} Kalman_t;

ErrStatus mpu6050_init(void);
void mpu6050_read_accel(MPU6050_t *data);
void mpu6050_read_gyro(MPU6050_t *data);


#endif
