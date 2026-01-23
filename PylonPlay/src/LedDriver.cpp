#include "LedDriver.h"

LedDriver::LedDriver ()
{

}

/**
 * @brief Construct a new Led Driver:: Led Driver object
 * 
 * @param gpioN 
 * @param ledMod 
 * @param colorFormat 
 */
LedDriver::LedDriver (int gpioN, led_model_t ledMod, led_color_component_format_t colorFormat) :

    gpio(gpioN), model(ledMod), format(colorFormat)

{

    /* Initialize the device */
    ledConfig = {
        .strip_gpio_num = gpio,
        .max_leds = 1,
        .led_model = model,
        .color_component_format = format,
        .flags = 0,
    };

    rmtConfig = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .mem_block_symbols = 0,
        .flags = 0,
    };

    led_strip_new_rmt_device(&ledConfig, &rmtConfig, &ledHandle);
}

/**
 * @brief This method shall toggle the onboard LED either on with the 
 *        given color intensities, or off.
 * 
 * @param color1 First color in format
 * @param color2 Second color in format
 * @param color3 Third color in format
 */
void
LedDriver::setLed(bool state, int color1, int color2, int color3)
{

    if(state) {

        led_strip_set_pixel(ledHandle, 0, color1, color2, color3); // {id, R, G, B}
        led_strip_refresh(ledHandle);

    } else { 

        led_strip_clear(ledHandle);
    }

}

/**
 * @brief This method simply pulses the LED with the
 *        given color settings every second 100 times.
 * 
 */
void
LedDriver::test (int color1, int color2, int color3)
{
    for(int idx = 0; idx < 100; idx++) {

      setLed(true, color1, color2, color3);

      /* Rest */
      vTaskDelay(pdMS_TO_TICKS(1000));

      setLed(false, color1, color2, color3);
      
      /* Rest */
      vTaskDelay(pdMS_TO_TICKS(1000));

    }
}