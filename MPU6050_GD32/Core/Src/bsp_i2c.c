#include <stdio.h>
#include "bsp_i2c.h"


/**
 * @brief 配置I2C
 * 
 */
void i2c_config()
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
    i2c_clock_config(I2CX, I2C_SPEED, I2C_DTCY_2);
    i2c_mode_addr_config(I2CX, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, SLAVE_ADDRESS7);
    i2c_enable(I2CX);
    i2c_ack_config(I2CX, I2C_ACK_DISABLE);
}

/**
 * @brief 从I2C从设备读取数据
 * 
 * @param pBuffer 存放读取到的数据
 * @param size 数据个数
 * @param slave_address 从设备地址
 * @param internal_address 从设备内部地址
 */
FlagStatus i2c_buffer_read_timeout(uint8_t *pBuffer, uint8_t size, uint8_t slave_address, uint8_t internal_address)
{
    uint8_t read_cmd_sent = RESET;
    uint8_t state = I2C_START;
    uint8_t fail_state = state;
    uint8_t timeout = 0;

    i2c_ack_config(I2CX, I2C_ACK_ENABLE);

    while (size >= 0) {
        timeout = 0;

        switch (state) {
        case I2C_START:
            if (read_cmd_sent == RESET) {
                while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if (timeout == I2C_TIME_OUT) {
                    fail_state = state;
                    state = I2C_FAIL;
                    break;
                }
                timeout = 0;
            }

            i2c_start_on_bus(I2CX);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            state = I2C_SEND_ADDRESS;
            break;

        case I2C_SEND_ADDRESS:
            if (read_cmd_sent == RESET) {
                i2c_master_addressing(I2CX, slave_address, I2C_TRANSMITTER);
            } else {
                i2c_master_addressing(I2CX, slave_address, I2C_RECEIVER);
            }
            while ((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            state = I2C_CLEAR_ADDRESS_FLAG;
            break;

        case I2C_CLEAR_ADDRESS_FLAG:
            i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
            if (read_cmd_sent == RESET) {
                while ((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            state = read_cmd_sent ? I2C_RECEIVE_DATA : I2C_TRANSMIT_DATA;
            break;

        case I2C_TRANSMIT_DATA:
            i2c_data_transmit(I2CX, internal_address);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            read_cmd_sent = SET;
            state = I2C_START;
            break;

        case I2C_RECEIVE_DATA:
            while ((!i2c_flag_get(I2CX, I2C_FLAG_RBNE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            if (size == 1) {
                i2c_ack_config(I2CX, I2C_ACK_DISABLE);
            }

            *pBuffer = i2c_data_receive(I2CX);
            pBuffer++;

            size--;

            state = size ? I2C_RECEIVE_DATA : I2C_STOP;
            break;
            
        case I2C_STOP:
            i2c_stop_on_bus(I2CX);
            while ((I2C_CTL0(I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                return SET;
            }
            return RESET;

        case I2C_FAIL:
            switch (fail_state) {
            case I2C_START:
                printf("I2C bus starts error!\r\n");
                return SET;
            case I2C_SEND_ADDRESS:
                printf("I2C bus master addressing error!\r\n");
                return SET;
            case I2C_CLEAR_ADDRESS_FLAG: 
                printf("I2C bus clearing ADDSEND flag error!\r\n");
                return SET;
            case I2C_TRANSMIT_DATA:
                printf("I2C bus transmitting data error!\r\n");
                return SET;
            case I2C_RECEIVE_DATA: 
                printf("I2C bus receiving data error!\r\n");
                return SET;
            case I2C_STOP:
                printf("I2C bus stopping error!\r\n");
                return SET;
            }
        }
    }
}

/**
 * @brief 向I2C设备写入数据
 * 
 * @param pBuffer 携带数据的指针
 * @param size 数据量
 * @param slave_address 从设备地址
 * @param internal_address 从设备内部功能地址
 * @return FlagStatus 
 */
FlagStatus i2c_buffer_write_timeout(uint8_t *pBuffer, uint8_t size, uint8_t slave_address, uint8_t internal_address)
{
    uint8_t write_cmd_sent = RESET;
    uint8_t state = I2C_START;
    uint8_t fail_state = state;
    uint8_t timeout = 0;

    i2c_ack_config(I2CX, I2C_ACK_ENABLE);

    while (size >= 0) {
        timeout = 0;

        switch (state) {
        case I2C_START:
            if (write_cmd_sent == RESET) {
                while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if (timeout == I2C_TIME_OUT) {
                    fail_state = state;
                    state = I2C_FAIL;
                    break;
                }
                timeout = 0;
            }

            i2c_start_on_bus(I2CX);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            state = I2C_SEND_ADDRESS;
            break;

        case I2C_SEND_ADDRESS:
            i2c_master_addressing(I2CX, slave_address, I2C_TRANSMITTER);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            state = I2C_CLEAR_ADDRESS_FLAG;
            break;

        case I2C_CLEAR_ADDRESS_FLAG:
            i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
            if (write_cmd_sent == RESET) {
                while ((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if (timeout == I2C_TIME_OUT) {
                    fail_state = state;
                    state = I2C_FAIL;
                    break;
                }
            }

            state = I2C_TRANSMIT_DATA;
            break;

        case I2C_TRANSMIT_DATA:
            if (write_cmd_sent == RESET) {
                i2c_data_transmit(I2CX, internal_address);
            } else {
                i2c_data_transmit(I2CX, *pBuffer);
                pBuffer++;
                size--;
            }
            while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                break;
            }

            write_cmd_sent = SET;

            state = size ? I2C_TRANSMIT_DATA : I2C_STOP;
            break;

        case I2C_STOP:
            i2c_stop_on_bus(I2CX);
            while (!(I2C_CTL0(I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout == I2C_TIME_OUT) {
                fail_state = state;
                state = I2C_FAIL;
                return SET;
            }
            return RESET;

        case I2C_FAIL:
            switch (fail_state) {
            case I2C_START:
                printf("I2C bus starts error!\r\n");
                return SET;
            case I2C_SEND_ADDRESS:
                printf("I2C bus master addressing error!\r\n");
                return SET;
            case I2C_CLEAR_ADDRESS_FLAG: 
                printf("I2C bus clear ADDSEND flag error!\r\n");
                return SET;
            case I2C_TRANSMIT_DATA:
                printf("I2C bus transmitting data error!\r\n");
                return SET;
            case I2C_RECEIVE_DATA: 
                printf("I2C bus receiving data error!\r\n");
                return SET;
            case I2C_STOP:
                printf("I2C bus stopping with error!\r\n");
                return SET;
            }
            break; 
        }
    }
}