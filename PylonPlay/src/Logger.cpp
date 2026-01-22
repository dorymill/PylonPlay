#include "Logger.h"
#include "Types.h"
#include <chrono>
#include <ctime>
extern "C" {
    #include <string.h>
}


using namespace std::chrono;
using namespace std;

/**
 * @brief This class shall simply print time stamped
 *        log messages to the debug UART for test purposes.
 * 
 */
Logger::Logger ()
{

}


/**
 * @brief This method is responsible for sending
 *        debug statements to the uart.
 * 
 * @param msg 
 */
void
Logger::logMsg (const char* msg)
{

    /* Get a timestamp */
    time_point now  = system_clock::now();
    time_t now_time = system_clock::to_time_t(now);

    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "[%H:%M:%S]", localtime(&now_time));

    /* Append it to the message */
    char output [512] = "";

    int numBytes = sprintf(output, "%s%s\n", timestamp, msg);

    /* Send it out */
    uart_wait_tx_done(DEBUG_UART, 100);
    uart_write_bytes(DEBUG_UART, output, numBytes);

}