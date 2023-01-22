#include <stdio.h>
#include "bsp_mpu6050.h"

const double Accel_Z_corrector = 14418.0;

/**
 * @brief 初始化MPU6050，并检查MPU6050工作是否正常
 * 
 */
void mpu6050_init(void)
{
    i2c_config();

    // 检查MPU6050是否正常
    uint8_t read_buffer[1];
    if (!i2c_buffer_read_timeout(read_buffer, 1, MPU6050_ADDR, WHO_AM_I_REG))
        printf("mpu6050 is ready!\r\n");

    uint8_t cmd[4] = {0, 0x07, 0x00, 0x00};

    if (!i2c_buffer_write_timeout(&cmd[0], 1, MPU6050_ADDR, PWR_MGMT_1_REG))
        printf("write PWR_MGMT_1_REG success!\r\n");

    if (!i2c_buffer_write_timeout(&cmd[1], 1, MPU6050_ADDR, SMPLRT_DIV_REG))
        printf("write SMPLRT_DIV_REG success!\r\n");

    if (!i2c_buffer_write_timeout(&cmd[2], 1, MPU6050_ADDR, ACCEL_CONFIG_REG))
        printf("write ACCEL_CONFIG_REG success!\r\n");

    if (!i2c_buffer_write_timeout(&cmd[3], 1, MPU6050_ADDR, GYRO_CONFIG_REG))
        printf("write GYRO_CONFIG_REG success!\r\n");
}

/**
 * @brief 从MPU6050中读取加速度数据
 * 
 * @param data 存放度渠道的数据
 */
void mpu6050_read_accel(MPU6050_t *data)
{
    uint8_t accel_data[6] = {0};

    i2c_buffer_read_timeout(accel_data, 6, MPU6050_ADDR, ACCEL_XOUT_H_REG);

    for (uint8_t i = 0; i < 6; i++)
        printf("accel_data[%d]: %lf\r\n", i, accel_data[i]);

    data->Accel_X_RAW = (uint16_t)(accel_data[0] << 8 | accel_data[1]);
    data->Accel_X_RAW = (uint16_t)(accel_data[2] << 8 | accel_data[3]);
    data->Accel_X_RAW = (uint16_t)(accel_data[4] << 8 | accel_data[5]);

    data->Ax = data->Accel_X_RAW / 16384.0;
    data->Ay = data->Accel_Y_RAW / 16384.0;
    data->Az = data->Accel_Z_RAW / Accel_Z_corrector;
}