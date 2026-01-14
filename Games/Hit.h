#ifndef HIT_H
#define HIT_H

#include <chrono>

using namespace std::chrono;

enum class Zone {

    Delta   = 1,
    Charlie = 3,
    Alpha   = 5,

};

class Hit
{

    public:

        Hit(Zone sccore);

        Zone score;

        milliseconds time;

};


#endif HIT_H