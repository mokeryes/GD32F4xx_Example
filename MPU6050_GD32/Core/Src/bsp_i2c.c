#include "bsp_i2c.h"


void I2C_init()
{
    i2c_deinit(I2CX);

    /* GPIO settings */
    rcu_periph_clock_enable(I2C_RCU_PORT);
    gpio_af_set(I2C_GPIO_PORT, I2C_GPIO_AF, I2C_SCL_PIN);
    gpio_af_set(I2C_GPIO_PORT, I2C_GPIO_AF, I2C_SDA_PIN);

    gpio_mode_set(I2C_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_PIN);
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(I2C_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_PIN);
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);

    /* I2C config */
    rcu_periph_clock_enable(I2C_RCU);
    i2c_clock_config(I2CX, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2CX, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, SLAVE_ADDRESS7);
    i2c_enable(I2CX);
    i2c_ack_config(I2CX, I2C_ACK_ENABLE);
}

void i2c_bus_reset(void)
{
    i2c_deinit(I2CX);
    /* configure SDA/SCL for GPIO */
    GPIO_BC(I2C_GPIO_PORT) |= I2C_SCL_PIN;
    GPIO_BC(I2C_GPIO_PORT) |= I2C_SDA_PIN;
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(I2C_GPIO_PORT) |= I2C_SCL_PIN;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(I2C_GPIO_PORT) |= I2C_SDA_PIN;
    /* connect I2C_SCL_PIN to I2C_SCL */
    /* connect I2C_SDA_PIN to I2C_SDA */
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
    /* configure the I2CX interface */
    I2C_init();
}