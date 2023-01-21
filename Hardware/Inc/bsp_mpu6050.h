#ifndef _BSP_MPU6050_H
#define _BSP_MPU6050_H


#include "gd32f4xx.h"
#include "systick.h"
#include "bsp_i2c.h"


#define I2C_TIME_OUT    5000
#define I2C_OK          1
#define I2C_FAIL        0
#define I2C_END         1
typedef enum {
    I2C_START = 0,
    I2C_SEND_ADDRESS,
    I2C_CLEAR_ADDRESS_FLAG,
    I2C_TRANSMIT_DATA,
    I2C_RECEIVE_DATA,
    I2C_STOP
} i2c_process_enum;


#define RAD_TO_DEG          57.295779513082320876798154814105

#define WHO_AM_I_REG        0x75
#define PWR_MGMT_1_REG      0x6B
#define SMPLRT_DIV_REG      0x19
#define ACCEL_CONFIG_REG    0x1C
#define ACCEL_XOUT_H_REG    0x3B
#define TEMP_OUT_H_REG      0x41
#define GYRO_CONFIG_REG     0x1B
#define GYRO_XOUT_H_REG     0x43
#define MPU6050_ADDR        0xD0

#define W   0
#define R   1


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

void MPU6050_init(void);


#endif