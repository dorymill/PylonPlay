#include "Types.h"
#include "Logger.h"
#include "DataSource.h"
#include "LedDriver.h"
#include "Open.h"
#include <thread>
#include <chrono>

extern "C" {
  #include <string.h>
  #include <driver/gpio.h>
}

/* Namespaces */
using namespace std::chrono;
using namespace std::this_thread;

/* Static Global Variables */
static UartDriver uart (DEBUG_UART, 1024, 10, 0);
static Logger     logger (&uart);
static LedDriver  led (DEBUG_LED, LED_MODEL_WS2812, LED_STRIP_COLOR_COMPONENT_FMT_GRB);

extern "C" {
    void app_main(void);
}

/**
 * @brief This defines the main entry point logic of the program,
 *        namely the main transceiver state-machine logic.
 * 
 * @return int 
 */
void app_main(void) {

    /* Pre main statemachine setup routines */
    // Constructors of peripherals start those up

    /* Create the game */
    logger.logMsg("[+] Creating Open Mode. . .");
    Open openGame (true, 1, 10, seconds(15), &logger, &led);
    logger.logMsg("[+] Done!");

    /* Create a data source */
    logger.logMsg("[+] Creating DataSource. . .");
    DataSource dataSrc;
    logger.logMsg("[+] Done!");

    /* Instantaite the hit listener in the game mode */
    logger.logMsg("[+] Instantiating hit listener. . .");
    openGame.hitListener = new HitListener (&openGame.hitQueue, 
                                            &openGame.startTime,
                                            &openGame.hitDetected);
    logger.logMsg("[+] Done!");


    /* Register the listener with the data source */
    logger.logMsg("[+] Registering hit listener. . .");
    dataSrc.registerListener(openGame.hitListener);
    logger.logMsg("[+] Done!");

    /* Launch the game */
    logger.logMsg("[+] Launching game thread. . .");
    thread openGameThread([&openGame]() { while(!openGame.killed && openGame.running) {openGame.gameStateMachine(); }});
    logger.logMsg("[+] Done!");
    

    /* Wait until the state is ready */
    while (openGame.getState() != State::READY) {
        logger.logMsg("[T] Sleeping while game preps. . .");
        sleep_for(milliseconds(100));
    }

    /* Start the game. */
    logger.logMsg("[T] Game Start command issued.");
    openGame.start ();

    vTaskDelay(pdMS_TO_TICKS(10));

    logger.logMsg("[T] Issuing a 180 ms Alpha-Charlie shot pair every second. . .");
    while (openGame.getState () == State::RUNNING) {

        /* Trigger a 180 ms pair every second */
        vTaskDelay(pdMS_TO_TICKS(1000));
        dataSrc.registerHit(Zone::Alpha);
        vTaskDelay(pdMS_TO_TICKS(180));
        dataSrc.registerHit(Zone::Charlie);

    }

    /* Game timout test */
    vTaskDelay(pdMS_TO_TICKS(5000));
    /* Wait until the state is ready */
    while (openGame.getState() != State::READY) {
        logger.logMsg("[T] Sleeping while game preps. . .");
        sleep_for(milliseconds(100));
    }

    /* Start the game. */
    logger.logMsg("[T] Game Start command issued.");
    openGame.start ();
    
    logger.logMsg("[T] Allowing the timeout to elapse after a single shot. . .");
    do {
        sleep_for(milliseconds(1000));
        dataSrc.registerHit(Zone::Delta);
        sleep_for(seconds(20));

    } while (openGame.getState() == State::RUNNING);

    /* Terminate the game thread gracefully */
    openGame.kill();

    openGameThread.join();
    
    logger.logMsg("[T] Test complete.");

}
