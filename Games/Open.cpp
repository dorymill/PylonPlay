#include "Open.h"

/**
 * @brief This game mode is designed to operate with at least 
 *        one target and up to N targets. If the unit is not
 *        the master, it will simply spin in the running state
 *        waiting for a hit to register. Once a hit is 
 *        registered, the unit shall play the apporpriate sound,
 *        set the LED state accordingly, and notify the master
 *        unit. Once complete, the unit shall transition to the
 *        done state, awaiting further commands.
 * 
 *        If the unit is the master, entrance into the running
 *        state will trigger the start of a timer. In addition to
 *        waiting for hit detection, it shall check the internal
 *        hit queue for data from the rest of the targets,
 *        noting the time of receipt from the queue. Once data
 *        for all N sensors has been acquired or a timeout has been
 *        hit, the master will provide a final report.
 * 
 * @param isMaster 
 */
Open::Open (bool isMaster, int nSensors, seconds timeout) :
    isMaster(isMaster), state(State::INIT), running(true),
    nSensors(nSensors), timerRunning(false), timeout(timeout)
{


}

class HitListener : public EventListener

{

    public:

        explicit HitListener (queue<Hit>* pHitQueue, high_resolution_clock::time_point startTime) :
            pHitQueue(pHitQueue), startTime(startTime)
        {
            /* Get reference to the mode's hit queue */
        }

        void registerHit(Zone score) override {

            if(pHitQueue) {

                /* Popualte Hit fields */
                Hit hit = Hit(score);

                /* Calculate the hit time from the mode provided start time. */
                hit.time = duration_cast<seconds>(high_resolution_clock::now()-startTime);

                /* Push it to the queue*/
                pHitQueue->push(hit);

            }

        }
    
    private:

        queue<Hit>* pHitQueue;

        high_resolution_clock::time_point startTime;

};