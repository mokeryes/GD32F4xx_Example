#include <stdio.h>
#include "bsp_mpu6050.h"


/**
 * @brief 初始化MPU6050，并检查MPU6050工作是否正常
 * 
 */
void mpu6050_init(void)
{
    i2c_config();

    // 检查MPU6050是否正常
    uint8_t read_buffer[10];
    if (!i2c_buffer_read_timeout(read_buffer, 1, MPU6050_ADDR, WHO_AM_I_REG))
        printf("read success! check value: check = %d\r\n", read_buffer[0]);

    delay_1ms(100);

    uint8_t write_buffer[10];
    if (!i2c_buffer_write_timeout(write_buffer, 1, MPU6050_ADDR, PWR_MGMT_1_REG))
        printf("write success!\r\n");
}