#include "Hit.h"


/**
 * @brief This object represents a generic hit on target.
 *        creation of this object will initially include 
 *        a score. A generic time field is provided for 
 *        various usage in calculating shot deltas.
 * 
 * @param score 
 */
Hit::Hit(Zone score) :
    score(score)

{

}
