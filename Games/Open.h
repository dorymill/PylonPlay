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

        Open (bool isMaster, int nSensors, seconds timeout);

        void openModeStateMachine ();

        /* Interface methods */
        void start () override;
        void reset () override;

        State getState () override;

        void setTimeout (seconds timeout) override;

    private:

        /* Variables */
        State state;

        queue<Hit> hitQueue;

        EventListener* hitListener;

        high_resolution_clock::time_point startTime;
        high_resolution_clock::time_point time;
        seconds      timeout;
        seconds      elapsed;
        
        bool running;
        bool isMaster;
        bool timerRunning;
        bool hitDetected;
        
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


#endif