#ifndef _BSP_I2C_H
#define _BSP_I2C_H


#include "gd32f4xx.h"


#define I2CX                    I2C0
#define I2C_RCU_PORT            RCU_GPIOB
#define I2C_RCU                 RCU_I2C0
#define I2C_GPIO_AF             GPIO_AF_4
#define I2C_GPIO_PORT           GPIOB
#define I2C_SCL_PIN             GPIO_PIN_6
#define I2C_SDA_PIN             GPIO_PIN_7
#define SLAVE_ADDRESS7          0xD0


void I2C_init(void);
void i2c_bus_reset(void);

#endif