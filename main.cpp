#include <iostream>
#include <chrono>
#include <thread>
#include "Test/DataSource.h"
#include "Games/Open.h"

using namespace std::chrono;
using namespace std::this_thread;

/**
 * @brief Program entry point
 * 
 * @return int Return code
 */
int main () {

    /* Create the game */
    Open openGame (false, 1, 10, seconds(15));

    /* Create a data source */
    DataSource dataSrc;

    /* Instantaite the hit listener in the game mode */
    openGame.hitListener = new HitListener (&openGame.hitQueue, 
                                            &openGame.startTime,
                                            &openGame.hitDetected);


    /* Register the listener with the data source */
    dataSrc.registerListener(openGame.hitListener);

    /* Launch the game */
    thread openGameThread([&openGame]() { while(openGame.running) {openGame.gameStateMachine(); }});
    

    while (openGame.getState() != State::READY) {
        /* Wait until the state is ready */
        cout << "[T] Sleeping while game preps. . ." << endl;
        sleep_for(milliseconds(100));
    }

    /* Start the game. */
    cout << "[T] Game Start command issued." << endl;
    openGame.start ();

    // cout << "[T] Issuing a 180 ms Alpha-Charlie shot pair every second. . ." << endl;
    // while (openGame.getState () == State::RUNNING) {

    //     /* Trigger a 180 ms pair every second */
    //     sleep_for(milliseconds(1000));
    //     dataSrc.registerHit(Zone::Alpha);
    //     sleep_for(milliseconds(180));
    //     dataSrc.registerHit(Zone::Charlie);

    // }

    
    cout << "[T] Allowing the timeout to elapse after a single shot. . ." << endl;
    do {
        sleep_for(milliseconds(1000));
        dataSrc.registerHit(Zone::Delta);
        sleep_for(seconds(20));

    } while (openGame.getState() == State::RUNNING);

    /* Terminate the game thread gracefully */
    openGame.kill();

    openGameThread.join();
    
    cout << "[T] Test complete." << endl;
    return 0;
}