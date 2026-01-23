#ifndef OPEN_H
#define OPEN_H

#include "Hit.h"
#include "EventListener.h"
#include "iGame.h"
#include "Logger.h"
#include "LedDriver.h"
#include <chrono>
#include <queue>
#include <atomic>



using namespace std::chrono;
using namespace std;

class Open : public iGame
{

    public:

        Open (bool isMaster, int nSensors, int hitsPerSensor, seconds timeout, Logger* logger, LedDriver* led);
        
        EventListener* hitListener;
        
        queue<Hit> hitQueue;
        
        high_resolution_clock::time_point startTime;
        
        atomic<bool> hitDetected;
        bool running;
        atomic<bool> killed;
        
        /* Interface methods */
        void gameStateMachine () override;

        void start () override;
        void reset () override;
        void kill  () override;

        State getState () override;

        void setTimeout (seconds timeout) override;

    private:

        /* Variables */
        Logger* logger;
        LedDriver* led;
        State state;

        high_resolution_clock::time_point time;
        seconds      timeout;
        seconds      elapsed;
        
        bool isMaster;
        bool timerRunning;
        bool resetFlag;
        bool startSignal;
        bool masterMaxed; //
        
        int nuHits;
        int ngHits;
        int nSensors;
        int nUnitHitsMax;
        int nGlobalHitsMax;

        /* Methods */
        void alertHit ();
        void playSound ();
        void setLED ();
        void finalReport ();

};

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

            HitListener (queue<Hit>* pHitQueue, high_resolution_clock::time_point* startTime,
                              atomic<bool>* phitDetect) :    
            pHitQueue(pHitQueue), hitDetect(phitDetect), startTime(startTime)
        {
            /* Get reference to the mode's hit queue */
        }

        void registerHit(Zone score) override {

            if(pHitQueue) {

                /* Popualte Hit fields */
                Hit* hit = new Hit(score);

                /* Calculate the hit time from the mode provided start time. */
                hit->time = duration_cast<milliseconds>(high_resolution_clock::now() - *startTime);

                /* Push it to the queue*/
                pHitQueue->push(*hit);

                /* Alert the system a hit has been detected */
                *hitDetect = true;

            }

        }
    
    private:

        queue<Hit>* pHitQueue;

        atomic<bool>* hitDetect;

        high_resolution_clock::time_point* startTime;

};


#endif