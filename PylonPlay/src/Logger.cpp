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

Logger::Logger (UartDriver* uart) :

    uart(uart)
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

    uart->write(msg);

}