#include "UartDriver.h"
#include "Types.h"
#include <chrono>
#include <ctime>
extern "C" {
    #include <string.h>
}

using namespace std::chrono;
using namespace std;

/**
 * @brief Construct a new Uart Driver:: Uart Driver object
 * 
 */
UartDriver::UartDriver ()
{

}

UartDriver::UartDriver (uart_port_t port, int rxtxBuffSize, int queueSize, int intFlags)
{

    /* Spin up the debug uart with given settings */
    uart_driver_install(port, UART_BUFF_SIZE, UART_BUFF_SIZE, 10, &uartQueue, 0);

    uartConfig = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = 0
    };

    uart_param_config(port, &uartConfig);

    uart_set_pin(port, DEBUG_TX, DEBUG_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

}

/**
 * @brief Write data to the debug uart.
 * 
 * @param data 
 */
void
UartDriver::write (const void* data)
{
    /* Get a timestamp */
    time_point now  = system_clock::now();
    time_t now_time = system_clock::to_time_t(now);

    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "[%H:%M:%S]", localtime(&now_time));

    /* Append it to the message */
    char output [512] = "";

    int numBytes = sprintf(output, "%s%s\n", timestamp, (char*) data);

    /* Send it out */
    uart_wait_tx_done(DEBUG_UART, 100);
    uart_write_bytes(DEBUG_UART, output, numBytes);

}