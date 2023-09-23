#include <rom/ets_sys.h>
#include <driver/gpio.h>
#include "esp_log.h"

#define GPIO_DEBO433MHZ_PIN 13


const int delaymicro = 485;
const int switchingtime = 100;

void setup_debo_gpio() {
    gpio_reset_pin(GPIO_DEBO433MHZ_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_DEBO433MHZ_PIN, GPIO_MODE_OUTPUT);
}

void send100(int n) {
    for (int a=1; a<=n; a++) {
        gpio_set_level(GPIO_DEBO433MHZ_PIN,1);
        ets_delay_us(delaymicro-switchingtime);
        gpio_set_level(GPIO_DEBO433MHZ_PIN,0);
        ets_delay_us(delaymicro-switchingtime);
        ets_delay_us(2*delaymicro);
    }
}

void send110100(int n) {
    for (int a=1; a<=n; a++) {
        gpio_set_level(GPIO_DEBO433MHZ_PIN,1);
        ets_delay_us(delaymicro-switchingtime);
        ets_delay_us(2*delaymicro);
        gpio_set_level(GPIO_DEBO433MHZ_PIN,0);
        ets_delay_us(delaymicro-switchingtime);
        gpio_set_level(GPIO_DEBO433MHZ_PIN,1);
        ets_delay_us(delaymicro-switchingtime);
        gpio_set_level(GPIO_DEBO433MHZ_PIN,0);
        ets_delay_us(delaymicro-switchingtime);
        ets_delay_us(2*delaymicro);
    }
}

void chan1off(int n) {
    for (int a=1; a<=n; a++) {
        send100(3);
        send110100(3);
        send100(2);
        send110100(6);
        send100(2);
        ets_delay_us(11000);
    }
}

void chan1on(int n) {
    for (int a=1; a<=n; a++) {
        send100(3);
        send110100(3);
        send100(2);
        send110100(7);
        ets_delay_us(11000);
    }
}