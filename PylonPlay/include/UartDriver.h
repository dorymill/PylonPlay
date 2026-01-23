#ifndef UART_H
#define UART_H

extern "C" {
    #include "driver/uart.h"
}

class UartDriver
{

    public:

        UartDriver ();

        UartDriver (uart_port_t port, int rxtxBuffSize, int queueSize, int intFlags);

        void write (const void* data);

    private:

        QueueHandle_t uartQueue;

        uart_config_t uartConfig;

};


#endif