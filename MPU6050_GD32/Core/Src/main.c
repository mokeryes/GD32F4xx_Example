#include <stdio.h>
#include "main.h"


int main(void)
{
    systick_config();
    usart_gpio_config();
    MPU6050_init();

    while (1) {
        MPU6050_init();
    }

    return 0;
}

