#include "Types.h"
#include "Logger.h"
#include <chrono>

extern "C" {
  #include <string.h>
}


static QueueHandle_t uart_queue;
static uart_port_t uart;
static ledc_timer_config_t ledTimer;
static ledc_channel_config_t ledChannel;

static int startUart (void);
static int startRGB  (Logger*);
static int toggleLED (bool);

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


    /* Main Entry point For the program */
    for (;;) {

        /* Do a test write */
        if(toggleLED(true) == 0) {
            logger.logMsg("[+] LED toggled on.");
        } else {
            logger.logMsg("[-] Failed to toggle led.");
        }

        /* Rest */
        sleep(1);

        if(toggleLED(false) == 0){
            logger.logMsg("[+] LED toggled off.");
        } else {
            logger.logMsg("[-] Failed to toggle led.");
        }
        
        /* Rest */
        sleep(1);
    }

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

    logger->logMsg("[-] RBG TImer Config Error Code: " + to_string(success));

    ledChannel = {
        .gpio_num   = DEBUG_LED,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = DEBUG_LED_CHAN,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    }; 

    success &= ledc_channel_config(&ledChannel);

    logger->logMsg("[-] RGB Channel Config Error Code: " + to_string(success));

    return success;
}

/**
 * @brief This method shall toggle the onboard LED.
 * 
 * @param state 
 * @return int success
 */
static int
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