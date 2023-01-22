#include <stdio.h>
#include "main.h"


int main(void)
{
    systick_config();
    usart_gpio_config();
    mpu6050_init();

    MPU6050_t accel_data;

    while (1) {
        mpu6050_read_accel(&accel_data);

        printf("Accel_X_RAW: %lf\r\n", accel_data.Accel_X_RAW);
        printf("Accel_Y_RAW: %lf\r\n", accel_data.Accel_Y_RAW);
        printf("Accel_Z_RAW: %lf\r\n", accel_data.Accel_Z_RAW);

        delay_1ms(500);
    }

    return 0;
}

