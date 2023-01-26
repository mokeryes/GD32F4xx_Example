#include <stdio.h>
#include "main.h"


int main(void)
{
    systick_config();
    usart_gpio_config();

    printf("\r\n---------- GD32F4xx -----------\r\n");

    MPU6050_t mpu6050_data;
    ErrStatus mpu6050_state = mpu6050_init();

    while (1) {
        // if (mpu6050_state == SUCCESS) {
        //     mpu6050_read_accel(&mpu6050_data);
        //     mpu6050_read_gyro(&mpu6050_data);

        //     printf(
        //         "Ax: %lf Ay: %lf Az: %lf Gx: %lf Gy: %lf Gz: %lf\r\n", 
        //         mpu6050_data.Ax, mpu6050_data.Ay, mpu6050_data.Az,
        //         mpu6050_data.Gx, mpu6050_data.Gy, mpu6050_data.Gz 
        //     );

        //     delay_1ms(10);
        // }
        uint8_t time_tick = get_systick_ms();
        printf("get systick ms: %lf\r\n", time_tick);
    }

    return 0;
}

