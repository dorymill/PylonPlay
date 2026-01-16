#ifndef IGAME_H
#define IGAME_H

#include <chrono>
#include "States.h"

using namespace std::chrono;


/**
 * @brief This clas represent the generic game interface
 *        enforcing required methods for all games to have
 *        for system abstraction.
 * 
 */
class iGame
{

    public:

        /* Required methods*/
        virtual void start () = 0;
        virtual void reset () = 0;
        virtual void gameStateMachine () = 0;

        virtual State getState () = 0;

        virtual void setTimeout (seconds timeout) = 0;


};



#endif