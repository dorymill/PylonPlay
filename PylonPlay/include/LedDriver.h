#ifndef LED_H
#define LED_H

extern "C" {
  #include <led_strip.h>
}

class LedDriver 
{

    public:

        LedDriver ();

        LedDriver (int gpioNum, led_model_t ledModel, led_color_component_format_t ledFormat);

        void setLed(bool state, int color1, int color2, int color3);

        void test (int color1, int color2, int color3);

    private:

        int gpio;

        led_model_t model;

        led_color_component_format_t format;

        led_strip_handle_t ledStrip;
        led_strip_config_t ledConfig;
        led_strip_rmt_config_t rmtConfig;

        led_strip_handle_t ledHandle;


};



#endif