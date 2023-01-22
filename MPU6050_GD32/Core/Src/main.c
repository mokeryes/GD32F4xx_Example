#include <stdio.h>
#include "main.h"


int main(void)
{
    systick_config();
    usart_gpio_config();

    while (1) {
        mpu6050_init();
        delay_1ms(10);
    }

    return 0;
}

