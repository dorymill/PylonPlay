#ifndef TSRC_H
#define TSRC_H

#include "Hit.h"
#include <vector>
#include "EventListener.h"

using namespace std;

class DataSource
{

    public:

        DataSource ();

        void registerHit(Zone score);
        
        void registerListener(EventListener* listener);

        void removeListener ();

        
    private:
        
        vector<EventListener*> listeners;
        
};


#endif