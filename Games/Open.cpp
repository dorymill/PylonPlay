#include "Open.h"
#include <iostream>

using namespace std;

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
    nSensors(nSensors), timerRunning(false), timeout(timeout),
    resetFlag(false), startSignal(false), nUnitHitsMax(1), 
    nGlobalHitsMax(nSensors), nuHits(0), ngHits(0)
{


}

/**
 * @brief This method defines the state machine for the
 *        Open run mode.
 * 
 *        RESET - Default state and variables. -> READY
 *        READY - Awaiting start signal from master. -> RUNNING or RESET
 *        RUNNING - Game mode commenced. Executing game logic. -> DONE or RESET
 *        DONE - Game is over. Report data to master or send report
 *               if master. ->RESET
 */
void
Open::openModeStateMachine ()
{

    /* The magic! */
    if(running) {

        /* Check if we need to reset */
        if(resetFlag) {
            state = State::RESET;
        }

        switch(state) {

            case State::RESET:

                /* Set all relevant state variables back
                    to their defaults
                */
               running          = true;
                resetFlag       = false;
                timerRunning    = false;
                startSignal     = false;
                nuHits          = 0;
                ngHits          = 0;

                state = State::READY;

                break;

            case State::READY:

                /* Spin here until we get the green light */
                if(startSignal) {
                    state = State::RUNNING;
                }

                break;

            case State::RUNNING:

                /* Start the internal timer */
                if(!timerRunning) {
                    timerRunning = true;
                    startTime    = high_resolution_clock::now();
                
                } else {

                    /* Check the timeout and bail out if we've hit it */
                    elapsed = duration_cast<seconds>(high_resolution_clock::now() - startTime);

                    /* If we've timed out, move to the done state. */
                    if(elapsed >= timeout) {
                        state = State::DONE;
                    }

                    /* Exit condition checking */
                    if(isMaster) {
    
                        /* Check to see if we've accumualted all 
                        the hits for the entire game. */
                        if(ngHits >= nGlobalHitsMax) {
                            state = State::DONE;
                        }

                    } else {
                        /* Check to see if we've accumulated all
                           the hits for this particular unit. */
                        if(nuHits >= nUnitHitsMax) {
                            state = State::DONE;
                        }
                    }
    
                    /* Open Mode Logic */

                    /* If the event listener triggers a hit,
                       then we simply pop it off the queue
                       and send it to the master, otherwise
                       it's already in the internal queue it
                       needs to be in. */
                    if(hitDetected) {
    
                        if (isMaster) {
                            /* Increment the global hit counter */
                            ngHits++;
                        } else {
                            /* Alert the master of this hit */
                            alertHit ();
                        }

                        /* Increment the unit hit counter */
                        nuHits++;
                        
                        /* Drive the hit flag low */
                        hitDetected = false;
                    }

                }

                break;

            case State::DONE:

                if(isMaster) {
                    /* Send a final report to the user and reset */
                    finalReport ();
                    state = State::RESET;
                } else {
                    /* Otherwise, simply reset. */
                    state = State::RESET;
                }
                break;
        } 
    }
}

/**
 * @brief This method shall alert the master or the internal data queue
 *        of shot detections.
 * 
 */
void 
Open::alertHit ()
{
    if (hitQueue.size() > 0) {
        /* Report and pop elements off the queue */
        while(hitQueue.size() != 0) {

            Hit hit = hitQueue.front ();

            cout << "Hit " << (nuHits + 1) << " | " << static_cast<int>(hit.score) << endl;

            hitQueue.pop();

        }
    }
}

/**
 * @brief This method shall play the appropriate hit sound
 *        upon shot detection.
 * 
 */
void
Open::playSound ()
{
    // TO-DO: Implement
}

/**
 * @brief This method shall set the appropriate LEDs
 *        upon shot detection.
 * 
 */
void
Open::setLED ()
{
    // TO-DO: Implement
}

/**
 * @brief This method shall form a report of the
 *        game results, and report them back to the
 *        user.
 * 
 */
void
Open::finalReport ()
{
    // TO-DO: Implement
}

/**
 * @brief This method shall start the game mode.
 * 
 */
void
Open::start () 
{
    if(state == State::READY) {

        startSignal = true;
    }
}

/**
 * @brief This method shall place the state machine into
 *        the RESET state.
 * 
 *
 */
void
Open::reset ()
{
    resetFlag = true;
}

/**
 * @brief This method shall set the mode timeout.
 * 
 * @param timeout 
 */
void 
Open::setTimeout (seconds timeout)
{
    if(state != State::RUNNING) {
        if(timeout.count() > 0) {
            this->timeout = timeout;
        }
    }
}

/**
 * @brief This method returns the current state of the state
 *        machine.
 * 
 * @return State 
 */
State
Open::getState ()
{
    return state;
}

/** Abstract Class Implementation */
/**
 * @brief This is the implementation of the abstract
 *        EventListener for Open mode, intended to provide the 
 *        Observer-Listener pattern for the shot
 *        detection events, denoted by their score,
 *        and timestamped on receipt.
 * 
 */
class HitListener : public EventListener

{

    public:

        

        explicit HitListener (queue<Hit>* pHitQueue, high_resolution_clock::time_point startTime,
                              bool* phitDetect) :    
            pHitQueue(pHitQueue), startTime(startTime), hitDetect(phitDetect)
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

                /* Alert the system a hit has been detected */
                *hitDetect = true;

            }

        }
    
    private:

        queue<Hit>* pHitQueue;

        bool* hitDetect;

        high_resolution_clock::time_point startTime;

};