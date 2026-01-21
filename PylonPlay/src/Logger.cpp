#include "Logger.h"
#include "Types.h"
#include <chrono>


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
Logger::logMsg (string msg)
{

    /* Get a timestamp */
    time_point now = std::chrono::system_clock::now();

    string timestamp = format("[{:%H:%M:%S}]", now);

    /* Append it to the message */
    string output = timestamp + msg + "\n";

    /* Send it out */
    uart_wait_tx_done(DEBUG_UART, 100);
    uart_write_bytes(DEBUG_UART, output.c_str(), output.length());

}