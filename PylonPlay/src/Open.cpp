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
Open::Open (bool isMaster, int nSensors, int hitsPerSensor, seconds timeout) :
    hitDetected(false), running(true), state(State::RESET), 
    timeout(timeout), isMaster(isMaster), timerRunning(false),
    resetFlag(false), startSignal(false), masterMaxed(false),
    nuHits(0), ngHits(0), nSensors(nSensors),
    nUnitHitsMax(hitsPerSensor), nGlobalHitsMax(hitsPerSensor*nSensors)
    
    
{

    cout << "[0] Open game mode created with " << nSensors << " sensors,\n"
         << "expecting " << hitsPerSensor << " shots per sensor \n"
         << "within " << timeout.count() << " seconds." << endl;

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
Open::gameStateMachine ()
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
                running         = true;
                resetFlag       = false;
                timerRunning    = false;
                startSignal     = false;
                hitDetected     = false;
                nuHits          = 0;
                ngHits          = 0;

                state = State::READY;
                cout << "[O] Open SM: Reset Complete. Entering Ready." << endl;

                break;

            case State::READY:

                /* Spin here until we get the green light */
                if(startSignal) {
                    state = State::RUNNING;
                    cout << "[O] Open SM: Entering Running." << endl;
                }

                break;

            case State::RUNNING:

                /* Start the internal timer */
                if(!timerRunning) {
                    timerRunning = true;
                    startTime    = high_resolution_clock::now();
                    cout << "[O] Open SM: Timer started." << endl;
                
                } else {

                    /* Check the timeout and bail out if we've hit it */
                    elapsed = duration_cast<seconds>(high_resolution_clock::now() - startTime);

                    /* If we've timed out, move to the done state. */
                    if(elapsed >= timeout) {
                        state = State::DONE;
                        cout << "[O] Open SM: Timeout Elapsed. Entering Done." << endl;
                    }

                    /* Exit condition checking */
                    if(isMaster) {
    
                        /* Check to see if we've accumualted all 
                        the hits for the entire game. */
                        if(ngHits >= nGlobalHitsMax) {
                            state = State::DONE;
                            cout << "[O] Open SM: Max Hits detected as master. Entering Done." << endl;
                        }

                    } else {
                        /* Check to see if we've accumulated all
                           the hits for this particular unit. */
                        if(nuHits >= nUnitHitsMax) {
                            state = State::DONE;
                            cout << "[O] Open SM: Max Hits detected as target. Entering Done." << endl;
                        }
                    }
    
                    /* Open Mode Logic */

                    /* If the event listener triggers a hit,
                       then we simply pop it off the queue
                       and send it to the master, otherwise
                       it's already in the internal queue it
                       needs to be in. 
                       
                       If the master has gotten
                       all of its hits, it simply needs to aggregate 
                       data until the timeout has been hit. Do no shot
                       aggregation thereafter. */
                    if(hitDetected) {
    
                        playSound ();

                        setLED ();

                        /* Increment the unit hit counter */
                        nuHits++;

                        if (isMaster) {
                            /* Increment the global hit counter */
                            ngHits++;
                        } else {
                            /* Alert the master of this hit */
                            alertHit ();
                        }
                        
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

                cout << "[O] Open SM: Final Operations done. Entering Reset." << endl;

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
    /* Replace this logic with serializing and sendint the hit to the master. */
    if (hitQueue.size() > 0) {

            Hit hit = hitQueue.back();

            cout << "[O] Hit " << nuHits << " | " << static_cast<int>(hit.score) << " points at " <<
            static_cast<double>(hit.time.count())/1000.0 << "s." << endl;

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

    /* Replace with final report serialization and sending to user */
    cout << "[O] Issuing Final Report . . ." << endl;
    int hitNum = 0;

    while(hitQueue.size() != 0) {

        Hit hit = hitQueue.front();

        cout << "[OF] Hit " << ++hitNum << " | " << static_cast<int>(hit.score) << " points at " <<
        static_cast<double>(hit.time.count())/1000.0 << "s." << endl;

        hitQueue.pop();

    }
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

/**
 * @brief This method terminates the operation of the game 
 *        statemachine and execution thread.
 * 
 */
void
Open::kill ()
{
    running = false;
}