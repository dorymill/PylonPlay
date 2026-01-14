#ifndef TSRC_H
#define TSRC_H

#include "Games/Hit.h"
#include <vector>
#include "Utilities/EventListener.h"

using namespace std;

class DataSource
{

    public:

        void registerHit(Zone score);
        
        void registerListener(EventListener* listener);

        void removeListener ();

        
    private:
        
        vector<EventListener*> listeners;
        
};


#endif