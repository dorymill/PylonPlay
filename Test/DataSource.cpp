#include "DataSource.h"


/**
 * @brief This class shall serve as a test data
 *        source to trigger the action of the 
 *        event listener and interact with the
 *        current operational mode state machine.
 * 
 */
DataSource::DataSource ()

{

}

/**
 * @brief This method shall register hits and notify
 *        the event listeners.
 * 
 * @param score 
 */
void
DataSource::registerHit(Zone score)
{

    for(EventListener* lstnr : listeners) {

        lstnr->registerHit(score);
    }

}
