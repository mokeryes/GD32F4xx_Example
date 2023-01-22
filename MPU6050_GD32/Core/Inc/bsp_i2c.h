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
#define I2C_SPEED               1000
#define I2C_TIME_OUT            5000
#define SLAVE_ADDRESS7          0xD0

void i2c_config(void);


typedef enum {
    I2C_START = 0,
    I2C_SEND_ADDRESS,
    I2C_CLEAR_ADDRESS_FLAG,
    I2C_TRANSMIT_DATA,
    I2C_RECEIVE_DATA,
    I2C_STOP,
    I2C_FAIL
} i2c_state_enum;

FlagStatus i2c_buffer_read_timeout(uint8_t *pBuffer, uint8_t size, uint8_t slave_address, uint8_t internal_address);
FlagStatus i2c_buffer_write_timeout(uint8_t *pBuffer, uint8_t size, uint8_t slave_address, uint8_t internal_address);


#endif