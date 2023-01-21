#include <stdio.h>
#include "main.h"


int main(void)
{
    systick_config();
    usart_gpio_config();

    while (1) {
    }

    return 0;
}

