#ifndef LOG_H
#define LOG_H

#include "UartDriver.h"
#include <string>

using namespace std;


class Logger 
{


    public:

        Logger ();

        Logger (UartDriver* uart);

        void logMsg (const char*);

    private:

        UartDriver* uart;

};


#endif