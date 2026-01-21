#ifndef TYPES_H
#define TYPES_H

extern "C" {
    #include "driver/uart.h"
    #include "driver/ledc.h"
}

#define DEBUG_UART UART_NUM_0

#define UART_BUFF_SIZE (1024 * 2)

#define DEBUG_TX 43
#define DEBUG_RX 44

#define DEBUG_LED 48
#define DEBUG_LED_CHAN LEDC_CHANNEL_1


#endif