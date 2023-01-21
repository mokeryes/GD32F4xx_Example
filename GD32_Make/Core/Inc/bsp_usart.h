#ifndef _BSP_USART_H
#define _BSP_USART_H


#include "gd32f4xx.h"
#include "systick.h"


#define BSP_USART_RCU       RCU_USART0
#define BSP_USART_TX_RCU    RCU_GPIOA
#define BSP_USART_RX_RCU    RCU_GPIOA

#define BSP_USART_TX_PORT   GPIOA
#define BSP_USART_RX_PORT   GPIOA
#define BSP_USART_AF        GPIO_AF_7
#define BSP_USART_TX_PIN    GPIO_PIN_9
#define BSP_USART_RX_PIN    GPIO_PIN_10

#define BSP_USART           USART0
#define BSP_USART_BAUDRATE  115200

void usart_gpio_config(void);
void usart_send_data(uint32_t data);


#endif
