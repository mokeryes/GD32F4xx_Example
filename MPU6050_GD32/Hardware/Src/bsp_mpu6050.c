#include <stdio.h>
#include "bsp_mpu6050.h"


/**
 * @brief 向器件内部功能地址发送数据
 * 
 * @param slave_addr 器件地址
 * @param internel_addr 器件内部功能地址
 * @param data 要发送的数据
 */
static uint8_t mpu6050_single_read(uint8_t internal_addr)
{
    i2c_ack_config(I2CX, I2C_ACK_ENABLE);

    while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY));
    i2c_start_on_bus(I2CX);
    while (!i2c_flag_get(I2CX, I2C_FLAG_SBSEND));

    i2c_master_addressing(I2CX, MPU6050_ADDR, I2C_TRANSMITTER);
    while (!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
    while (!i2c_flag_get(I2CX, I2C_FLAG_TBE));

    i2c_data_transmit(I2CX, internal_addr);
    while (!i2c_flag_get(I2CX, I2C_FLAG_BTC));

    i2c_start_on_bus(I2CX);
    while (!i2c_flag_get(I2CX, I2C_FLAG_SBSEND));

    i2c_master_addressing(I2CX, MPU6050_ADDR, I2C_RECEIVER);
    i2c_ack_config(I2CX, I2C_ACK_DISABLE);
    while (!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
    i2c_stop_on_bus(I2CX);
    while (!i2c_flag_get(I2CX, I2C_FLAG_RBNE));
    uint8_t data = i2c_data_receive(I2CX);
    while (I2C_CTL0(I2CX) & I2C_CTL0_STOP);

    return data;
}


static void MPU6050_single_byte_write(uint8_t internal_addr, uint8_t data)
{
    i2c_ack_config(I2CX, I2C_ACK_DISABLE);

    i2c_ack_config(I2CX, I2C_ACK_ENABLE);
}

/**
 * @brief 从设备中读取单个数据
 * 
 * @param slave_addr 器件地址
 * @param internal_addr 器件内部功能地址
 * @return uint8_t 返回读取的数据
 */
uint8_t mpu6050_buffer_read_timeout(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte)
{
    uint8_t state = I2C_START;
    uint8_t read_cycle = 0;
    uint16_t timeout = 0;
    uint8_t i2c_timeout_flag = 0;

    /* enable acknowledge */
    i2c_ack_config(I2CX, I2C_ACK_ENABLE);
    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            if(RESET == read_cycle) {
                /* i2c master sends start signal only when the bus is idle */
                while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* whether to send ACK or not for the next byte */
                    if(2 == number_of_byte) {
                        i2c_ackpos_config(I2CX, I2C_ACKPOS_NEXT);
                    }
                } else {
                    i2c_bus_reset();
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c bus is busy in READ!\n");
                }
            }
            /* send the start signal */
            i2c_start_on_bus(I2CX);
            timeout = 0;
            state = I2C_SEND_ADDRESS;
            break;
        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                if(RESET == read_cycle) {
                    i2c_master_addressing(I2CX, MPU6050_ADDR, I2C_TRANSMITTER);
                    state = I2C_CLEAR_ADDRESS_FLAG;
                } else {
                    i2c_master_addressing(I2CX, MPU6050_ADDR, I2C_RECEIVER);
                    if(number_of_byte < 3) {
                        /* disable acknowledge */
                        i2c_ack_config(I2CX, I2C_ACK_DISABLE);
                    }
                    state = I2C_CLEAR_ADDRESS_FLAG;
                }
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                printf("i2c master sends start signal timeout in READ!\n");
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG:
            /* address flag set means i2c slave sends ACK */
            while((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
                if((SET == read_cycle) && (1 == number_of_byte)) {
                    /* send a stop condition to I2C bus */
                    i2c_stop_on_bus(I2CX);
                }
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                printf("i2c master clears address flag timeout in READ!\n");
            }
            break;
        case I2C_TRANSMIT_DATA:
            if(RESET == read_cycle) {
                /* wait until the transmit data buffer is empty */
                while((! i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* send the EEPROM's internal address to write to : only one byte address */
                    i2c_data_transmit(I2CX, read_address);
                    timeout = 0;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    printf("i2c master wait data buffer is empty timeout in READ!\n");
                }
                /* wait until BTC bit is set */
                while((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle++;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    printf("i2c master sends EEPROM's internal address timeout in READ!\n");
                }
            } else {
                while(number_of_byte) {
                    timeout++;
                    if(3 == number_of_byte) {
                        /* wait until BTC bit is set */
                        while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
                        /* disable acknowledge */
                        i2c_ack_config(I2CX, I2C_ACK_DISABLE);
                    }
                    if(2 == number_of_byte) {
                        /* wait until BTC bit is set */
                        while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
                        /* send a stop condition to I2C bus */
                        i2c_stop_on_bus(I2CX);
                    }
                    /* wait until RBNE bit is set */
                    if(i2c_flag_get(I2CX, I2C_FLAG_RBNE)) {
                        /* read a byte from the EEPROM */
                        *p_buffer = i2c_data_receive(I2CX);
                        /* point to the next location where the byte read will be saved */
                        p_buffer++;
                        /* decrement the read bytes counter */
                        number_of_byte--;
                        timeout = 0;
                    }
                    if(timeout > I2C_TIME_OUT) {
                        timeout = 0;
                        state = I2C_START;
                        read_cycle = 0;
                        printf("i2c master sends data timeout in READ!\n");
                    }
                }
                timeout = 0;
                state = I2C_STOP;
            }
            break;
        case I2C_STOP:
            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                printf("i2c master sends stop signal timeout in READ!\n");
            }
            break;
        default:
            state = I2C_START;
            read_cycle = 0;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            printf("i2c master sends start signal in READ.\n");
            break;
        }
    }
    return I2C_END;
}



void MPU6050_init(void)
{
    I2C_init();

    uint8_t buffer[10];
    printf("Start read!\r\n");
    // mpu6050_buffer_read_timeout(buffer, WHO_AM_I_REG, 1);
    // printf("mpu6050_buffer_read_timeout: Data read: check = %d\r\n", buffer[0]);
    uint8_t check = mpu6050_single_read(WHO_AM_I_REG);
    printf("mpu6050_single_read: Data read: check = %d\r\n", check);
}