#include "bsp_usart.h"
#include <stdio.h>


void usart_gpio_config(void)
{
    /* 配置USART-GPIOA的时钟 */
    rcu_periph_clock_enable(BSP_USART_RCU);
    rcu_periph_clock_enable(BSP_USART_TX_RCU);
    rcu_periph_clock_enable(BSP_USART_RX_RCU);

    /* 配置GPIO复用功能 */
    gpio_af_set(BSP_USART_TX_PORT, BSP_USART_AF, BSP_USART_TX_PIN);
    gpio_af_set(BSP_USART_RX_PORT, BSP_USART_AF, BSP_USART_RX_PIN);

    /* 配置GPIO的模式 上拉模式 */
    gpio_mode_set(BSP_USART_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BSP_USART_TX_PIN);
    gpio_mode_set(BSP_USART_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BSP_USART_RX_PIN);

    /* 配置GPIO输出 推挽输出 */
    gpio_output_options_set(BSP_USART_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_USART_TX_PIN);
    gpio_output_options_set(BSP_USART_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_USART_RX_PIN);


    /* 配置USART */
    usart_deinit(BSP_USART);
    usart_baudrate_set(BSP_USART, BSP_USART_BAUDRATE);
    usart_parity_config(BSP_USART, USART_PM_NONE);
    usart_word_length_set(BSP_USART, USART_WL_8BIT);
    usart_stop_bit_set(BSP_USART, USART_STB_1BIT);
    usart_enable(BSP_USART);

    /* 配置USART发送开启 */
    usart_transmit_config(BSP_USART, USART_TRANSMIT_ENABLE);
    /* 配置USART接收开启 */
    usart_receive_config(BSP_USART, USART_RECEIVE_ENABLE);
}

void usart_send_data(uint32_t data)
{
    usart_data_transmit(BSP_USART, data);
    /* 发送后等待缓冲区为空 */
    while (usart_flag_get(BSP_USART, USART_FLAG_TBE) == RESET);
}

/* 重写标准库的printf函数 */
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        usart_data_transmit(BSP_USART, ptr[i]);
        while(RESET == usart_flag_get(BSP_USART, USART_FLAG_TBE));
    }
    return len;
}