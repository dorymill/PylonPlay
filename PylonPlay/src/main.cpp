#include "Types.h"
#include "Logger.h"
#include "DataSource.h"
#include "Open.h"
#include <thread>
#include <chrono>

extern "C" {
  #include <string.h>
}

using namespace std::chrono;
using namespace std::this_thread;

static QueueHandle_t uart_queue;
static uart_port_t uart;
static ledc_timer_config_t ledTimer;
static ledc_channel_config_t ledChannel;

static int startUart (void);
static int startRGB  (Logger*);
int toggleLED (bool);
void ledTest (Logger*);

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
    startUart ();
    
    Logger logger;

    if(startRGB (&logger) != 0) {
        logger.logMsg("[+] Failed to start LED peripheral.");
    } else {
        logger.logMsg("[+] LED Peripheral initialized.");
    }

    /* Create the game */
    logger.logMsg("[+] Creating Open Mode. . .");
    Open openGame (false, 1, 10, seconds(15), &logger);
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
    

    while (openGame.getState() != State::READY) {
        /* Wait until the state is ready */
        logger.logMsg("[T] Sleeping while game preps. . .");
        sleep_for(milliseconds(100));
    }

    /* Start the game. */
    logger.logMsg("[T] Game Start command issued.");
    openGame.start ();

    vTaskDelay(pdMS_TO_TICKS(1000));

    logger.logMsg("[T] Issuing a 180 ms Alpha-Charlie shot pair every second. . .");
    while (openGame.getState () == State::RUNNING) {

        /* Trigger a 180 ms pair every second */
        vTaskDelay(pdMS_TO_TICKS(1000));
        dataSrc.registerHit(Zone::Alpha);
        vTaskDelay(pdMS_TO_TICKS(180));
        dataSrc.registerHit(Zone::Charlie);

    }

    
    // logger.logMsg("[T] Allowing the timeout to elapse after a single shot. . .");
    // do {
    //     sleep_for(milliseconds(1000));
    //     dataSrc.registerHit(Zone::Delta);
    //     sleep_for(seconds(20));

    // } while (openGame.getState() == State::RUNNING);

    /* Terminate the game thread gracefully */
    openGame.kill();

    openGameThread.join();
    
    logger.logMsg("[T] Test complete.");

}

/**
 * @brief This method sets up the uart for communication
 *        for debug and test purposes.
 * 
 * @return int Success
 */
static int 
startUart (void) 
{
    
    int success = 0;
    
    /* Install Drivers (Allocate UART Resources) */
    success = uart_driver_install(DEBUG_UART, UART_BUFF_SIZE, UART_BUFF_SIZE, 10, &uart_queue, 0);

    /* Set Comms Parameters */
    uart = DEBUG_UART;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = UART_HW_FLOWCTRL_DISABLE,
    };

    /* Set Comms Pins */
    success &= uart_param_config(uart, &uart_config);

    /* Start the action */
    success &= uart_set_pin(uart, DEBUG_TX, DEBUG_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    return success;

}

/**
 * @brief This method shall start the RGB LED peripheral.
 * 
 * 
 * @return int success
 */
static int
startRGB (Logger* logger)
{

    int success = 0;

    ledTimer = {

        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = 5000,
        .clk_cfg         = LEDC_AUTO_CLK
    };

    success = ledc_timer_config(&ledTimer);

    logger->logMsg(("[-] RBG TImer Config Error Code: " + to_string(success)).c_str());

    ledChannel = {
        .gpio_num   = DEBUG_LED,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = DEBUG_LED_CHAN,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0,
        .sleep_mode = LEDC_SLEEP_MODE_KEEP_ALIVE
    }; 

    success &= ledc_channel_config(&ledChannel);

    logger->logMsg(("[-] RGB Channel Config Error Code: " + to_string(success)).c_str());

    return success;
}

/**
 * @brief This method shall toggle the onboard LED.
 * 
 * @param state 
 * @return int success
 */
int
toggleLED (bool state)
{
    int success = 0;

    if(state) {
        /* Set to 100% duty cycle */
        success  = ledc_set_duty(LEDC_LOW_SPEED_MODE, DEBUG_LED_CHAN, 255);
        success &= ledc_update_duty(LEDC_LOW_SPEED_MODE, DEBUG_LED_CHAN); 

    } else { // Not working for some reason
        /* Set to 0% duty cycle */
        success  = ledc_set_duty(LEDC_LOW_SPEED_MODE, DEBUG_LED_CHAN, 0);
        success &= ledc_update_duty(LEDC_LOW_SPEED_MODE, DEBUG_LED_CHAN); 
    }

    return success;
}

void
ledTest (Logger* logger)
{
    for(;;){
        /* Do a test write */
      if(toggleLED(true) == 0) {
          logger->logMsg("[+] LED toggled on.");
      } else {
          logger->logMsg("[-] Failed to toggle led.");
      }

      /* Rest */
      sleep(1);

      if(toggleLED(false) == 0){
          logger->logMsg("[+] LED toggled off.");
      } else {
          logger->logMsg("[-] Failed to toggle led.");
      }
      
      /* Rest */
      sleep(1);

    }
}