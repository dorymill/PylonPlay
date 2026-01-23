#ifndef EVTL_H
#define EVTL_H

#include "Hit.h"

class EventListener {

    public:

        virtual void registerHit(Zone score) = 0;

        virtual void setStartTime (high_resolution_clock::time_point time) = 0;

};

#endif