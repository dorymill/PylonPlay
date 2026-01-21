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

/**
 * @brief This method will register a listener for the 
 *        data events to call.
 * 
 * @param listener 
 */
void
DataSource::registerListener (EventListener* listener)
{
    listeners.push_back (listener);
}


/**
 * @brief This method removes the last listener added to 
 *        the listener vector.
 * 
 */
void
DataSource::removeListener ()
{
    listeners.pop_back ();
}