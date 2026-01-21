#ifndef EVTL_H
#define EVTL_H

#include "Hit.h"

class EventListener {

    public:

        virtual void registerHit(Zone score) = 0;

};

#endif