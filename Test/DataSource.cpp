#include "DataSource.h"



DataSource::DataSource ()

{

}

/**
 * @brief This method shall register hits and notify
 *        the event listeners.
 * 
 * @param sccore 
 */
void
DataSource::registerHit(Zone sccore)
{

    for(EventListener& lstnr : listeners) {

    }

}
