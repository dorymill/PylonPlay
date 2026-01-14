#ifndef OPEN_H
#define OPEN_H

#include <chrono>
#include <queue>
#include "Hit.h"
#include "Utilities/EventListener.h"

using namespace std::chrono;
using namespace std;

enum class State {

    RESET,
    INIT,
    READY,
    RUNNING,
    DONE,

};

class Open
{

    public:

        Open (bool isMaster, int nSensors, seconds timeout);

        void openModeStateMachine ();

    private:

        /* Variables */
        State state;

        queue<Hit> hitQueue;

        EventListener hitListener;

        high_resolution_clock::time_point startTime;
        high_resolution_clock::time_point time;
        seconds      timeout;
        
        bool running;
        bool isMaster;
        bool timerRunning;

        int nSensors;
        int nHits;

        /* Methods */
        void alertHit ();
        void playSound ();
        void setLED ();
        void finalReport ();
};


#endif