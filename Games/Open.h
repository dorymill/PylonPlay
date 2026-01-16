#ifndef OPEN_H
#define OPEN_H

#include <chrono>
#include <queue>
#include "Hit.h"
#include "Utilities/EventListener.h"
#include "iGame.h"

using namespace std::chrono;
using namespace std;

class Open : public iGame
{

    public:

        Open (bool isMaster, int nSensors, int hitsPerSensor, seconds timeout);
        
        EventListener* hitListener;
        
        queue<Hit> hitQueue;
        
        high_resolution_clock::time_point startTime;
        
        bool hitDetected;
        
        /* Interface methods */
        void gameStateMachine () override;

        void start () override;
        void reset () override;

        State getState () override;

        void setTimeout (seconds timeout) override;

    private:

        /* Variables */
        State state;

        high_resolution_clock::time_point time;
        seconds      timeout;
        seconds      elapsed;
        
        bool running;
        bool isMaster;
        bool timerRunning;
        
        bool resetFlag;
        bool startSignal;
        
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
                              bool* phitDetect) :    
            pHitQueue(pHitQueue), startTime(startTime), hitDetect(phitDetect)
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

        bool* hitDetect;

        high_resolution_clock::time_point* startTime;

};


#endif