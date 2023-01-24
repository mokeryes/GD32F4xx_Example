#include <stdio.h>
#include "bsp_mpu6050.h"

const double Accel_Z_corrector = 14418.0;

/**
 * @brief 初始化MPU6050，并检查MPU6050工作是否正常
 * 
 */
ErrStatus mpu6050_init(void)
{
    i2c_config();

    // 获取MPU6050状态
    uint8_t read_buffer[1] = {0};
    i2c_buffer_read_timeout(read_buffer, 1, MPU6050_ADDR, WHO_AM_I_REG);
    if (read_buffer[0] == 104) {
        printf("MPU6050 is ready!\r\n");

        // uint8_t data[6] = {
        //     PWR_MGMT_1_RESET,
        //     PWR_MGMT_1_WAKEUP,
        //     CONFIG_VALUE,
        //     GYRO_CONFIG_VALUE,
        //     ACCEL_CONFIG_VALUE,
        //     SMPLRT_DIV_0
        // };
        uint8_t data[6] = {
            0x80,
            0x00,
            0x00,
            0x00,
            0x00,
            0x07
        };

        // 复位设备并延时100ms以等待设备复位完成
        i2c_buffer_write_timeout(&data[0], 1, MPU6050_ADDR, PWR_MGMT_1_REG);
        delay_1ms(100);

        i2c_buffer_write_timeout(&data[1], 1, MPU6050_ADDR, PWR_MGMT_1_REG);

        i2c_buffer_write_timeout(&data[2], 1, MPU6050_ADDR, CONFIG_REG);

        i2c_buffer_write_timeout(&data[3], 1, MPU6050_ADDR, GYRO_CONFIG_REG);

        i2c_buffer_write_timeout(&data[4], 1, MPU6050_ADDR, ACCEL_CONFIG_REG);

        i2c_buffer_write_timeout(&data[5], 1, MPU6050_ADDR, SMPLRT_DIV_REG);

    } else {
        printf("MPU6050 is not ready!\r\n");
        return ERROR;
    }

    return SUCCESS;
}

/**
 * @brief 从MPU6050中读取三轴加速度数据
 * 
 * @param data 存放读取到的数据
 */
void mpu6050_read_accel(MPU6050_t *data)
{
    uint8_t accel_data[6] = {0};

    i2c_buffer_read_timeout(accel_data, 6, MPU6050_ADDR, ACCEL_XOUT_H_REG);
    // i2c_buffer_read_timeout(&accel_data[0], 2, MPU6050_ADDR, ACCEL_XOUT_H_REG);
    // i2c_buffer_read_timeout(&accel_data[2], 2, MPU6050_ADDR, ACCEL_YOUT_H_REG);
    // i2c_buffer_read_timeout(&accel_data[4], 2, MPU6050_ADDR, ACCEL_ZOUT_H_REG);

    data->Accel_X_RAW = (uint16_t)(accel_data[0] << 8 | accel_data[1]);
    data->Accel_Y_RAW = (uint16_t)(accel_data[2] << 8 | accel_data[3]);
    data->Accel_Z_RAW = (uint16_t)(accel_data[4] << 8 | accel_data[5]);

    data->Ax = data->Accel_X_RAW / 16384.0;
    data->Ay = data->Accel_Y_RAW / 16384.0;
    data->Az = data->Accel_Z_RAW / Accel_Z_corrector;
}

/**
 * @brief 从MPU6050读取三轴陀螺仪数据
 * 
 * @param data 读取到的数据
 */
void mpu6050_read_gyro(MPU6050_t *data)
{
    uint8_t gyro_data[6] = {0};

    i2c_buffer_read_timeout(gyro_data, 6, MPU6050_ADDR, GYRO_XOUT_H_REG);
    // i2c_buffer_read_timeout(&gyro_data[0], 2, MPU6050_ADDR, GYRO_XOUT_H_REG);
    // i2c_buffer_read_timeout(&gyro_data[2], 2, MPU6050_ADDR, GYRO_YOUT_H_REG);
    // i2c_buffer_read_timeout(&gyro_data[4], 2, MPU6050_ADDR, GYRO_ZOUT_H_REG);

    data->Gyro_X_RAW = (uint16_t)(gyro_data[0] << 8 | gyro_data[1]);
    data->Gyro_Y_RAW = (uint16_t)(gyro_data[2] << 8 | gyro_data[3]);
    data->Gyro_Z_RAW = (uint16_t)(gyro_data[4] << 8 | gyro_data[5]);

    data->Gx = data->Gyro_X_RAW / 131.0;
    data->Gy = data->Gyro_Y_RAW / 131.0;
    data->Gz = data->Gyro_Z_RAW / 131.0;
}