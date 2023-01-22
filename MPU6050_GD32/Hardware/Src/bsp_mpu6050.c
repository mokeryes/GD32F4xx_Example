#include <stdio.h>
#include "bsp_mpu6050.h"


/**
 * @brief 向I2C从设备内部指定地址写入数据
 * 
 * @param pBuffer 准备写入的数据指针
 * @param size 数据量
 * @param slave_address I2C从设备地址
 * @param internal_address I2C从设备内部功能地址
 */
static void mpu6050_buffer_write_timeout(uint8_t *pBuffer, uint8_t size, uint8_t slave_address, uint8_t internal_address)
{
    uint8_t state = I2C_START;
    uint8_t timeout = 0;
    uint8_t write_cycle = RESET;
    uint8_t nsize = 0;

    i2c_ack_config(I2CX, I2C_ACK_ENABLE);

    while (size) {
        switch (state) {
        case I2C_START:
            // 首次启动I2C总线需要检查总线是否空闲
            while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C bus is in busy write!\r\n");
                break;
            }

            timeout = 0;

            i2c_start_on_bus(I2CX);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C bus is in busy write!\r\n");
                break;
            }
            
            timeout = 0;
            state = I2C_SEND_ADDRESS;
            break;

        case I2C_SEND_ADDRESS:
            i2c_master_addressing(I2CX, slave_address, I2C_TRANSMITTER);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C send address ERROR!\r\n");
                break;
            }

            timeout = 0;
            state = I2C_CLEAR_ADDRESS_FLAG;
            break;

        case I2C_CLEAR_ADDRESS_FLAG:
            i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C clear ADDSEND flag ERROR!\r\n");
                break;
            }

            timeout = 0;
            state = I2C_TRANSMIT_DATA;
            break;

        case I2C_TRANSMIT_DATA:
            if (write_cycle == RESET) {
                i2c_data_transmit(I2CX, internal_address);
                while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT))
                    timeout++;
                if (timeout > I2C_TIME_OUT) {
                    timeout = 0;
                    state = I2C_TRANSMIT_DATA;
                    printf("I2C internal address transmit ERROR!\r\n");
                    break;
                }

                write_cycle = SET;
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
                break;
            }

            i2c_data_transmit(I2CX, pBuffer[nsize]);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
                printf("I2C data transmit ERROR!\r\n");
                break;
            }
            size--; nsize++;

            if (size == 0) {
                i2c_stop_on_bus(I2CX);
                state = I2C_STOP;
                break;
            }

            timeout = 0;
            state = I2C_TRANSMIT_DATA;
            break;

        case I2C_STOP:
            while (I2C_CTL0(I2CX) & I2C_CTL0_STOP);
            break;
        }
    }
}


/**
 * @brief 从MPU6050中读取多个数据
 * 
 * @param pBuffer 存放数据
 * @param size 数据个数
 * @param slave_address 从设备地址
 * @param internal_address 从设备内部功能地址
 */
static void mpu6050_buffer_read_timeout(uint8_t *pBuffer, uint8_t size, uint8_t slave_address, uint8_t internal_address)
{
    uint8_t state = I2C_START;
    uint8_t timeout = 0;
    uint8_t read_cycle = RESET;
    uint8_t nsize = 0;

    while (size) {
        switch (state) {
        case I2C_START:
            // 首次启动I2C总线需要检查总线是否空闲
            if (read_cycle == RESET) {
                while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT))
                    timeout++;
                if (timeout > I2C_TIME_OUT) {
                    timeout = 0;
                    state = I2C_START;
                    i2c_bus_reset();
                    printf("I2C bus is busy in read!\r\n");
                    break;
                }
            }

            // 总线是否成功启动
            i2c_start_on_bus(I2CX);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C start failed!");
                break;
            }

            timeout = 0;
            state = I2C_SEND_ADDRESS;
            break;

        case I2C_SEND_ADDRESS:
            // 配置从设备地址与读写模式
            if (read_cycle == RESET) {
                i2c_master_addressing(I2CX, slave_address, I2C_TRANSMITTER);
            } else {
                i2c_master_addressing(I2CX, slave_address, I2C_RECEIVER);
            }

            // 判断是否配置成功
            while ((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C config address ERROR!\r\n");
                break;
            } 

            timeout = 0;
            state = I2C_CLEAR_ADDRESS_FLAG;
            break;

        case I2C_CLEAR_ADDRESS_FLAG:
            i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
            if (read_cycle == RESET) {
                while ((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT))
                    timeout++;
            }

            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C clear ADDSEND flag ERROR!\r\n");
                break;
            }

            state = read_cycle ? I2C_RECEIVE_DATA : I2C_TRANSMIT_DATA;
            break;

        case I2C_TRANSMIT_DATA:
            i2c_data_transmit(I2CX, internal_address);
            while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT))
                timeout++;

            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C transmit data ERROR!\r\n");
                break;
            }

            timeout = 0;
            state = I2C_START;
            read_cycle = SET;
            break;

        case I2C_RECEIVE_DATA:
            // 读取到最后一个字节时，发送NACK并且停止总线
            while ((!i2c_flag_get(I2CX, I2C_FLAG_RBNE)) && (timeout < I2C_TIME_OUT))
                timeout++;
            if (timeout > I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("I2C receiving data ERROR!\r\n");
                break;
            }
            pBuffer[nsize] = i2c_data_receive(I2CX);
            size--; nsize++;

            timeout = 0;
            state = I2C_STOP;
            break;

        case I2C_STOP:
            i2c_stop_on_bus(I2CX);
            while (I2C_CTL0(I2CX) & I2C_CTL0_STOP);
            break;
        }
    }
}


/**
 * @brief 初始化MPU6050，并检查MPU6050工作是否正常
 * 
 */
void MPU6050_init(void)
{
    I2C_init();

    uint8_t write_buffer[1] = {0};
    mpu6050_buffer_write_timeout(write_buffer, 1, MPU6050_ADDR, PWR_MGMT_1_REG);

    // 检查MPU6050是否正常
    uint8_t pBuffer[1];
    mpu6050_buffer_read_timeout(pBuffer, 1, MPU6050_ADDR, WHO_AM_I_REG);
    if (pBuffer[0] == 104) {
        printf("MPU6050 is ready!\r\n");
    } else {
        printf("MPU6050 is not ready!\r\n");
    }
}