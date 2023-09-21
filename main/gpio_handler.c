#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"

#define GPIO_BUTTON_PIN 12
#define ESP_INTR_FLAG_DEFAULT 0

TaskHandle_t* state_task = NULL;

void IRAM_ATTR button_isr() {
    ESP_LOGE("isr", "GPIO ISR Triggered");
    xTaskNotifyGive(*state_task);
}

void setup_button_handler(TaskHandle_t* state_task_handle){
    /* Set the GPIO as a push/pull output */
    state_task = state_task_handle;
    gpio_reset_pin(GPIO_BUTTON_PIN);
    gpio_set_direction(GPIO_BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_intr_type(GPIO_BUTTON_PIN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_BUTTON_PIN, button_isr, NULL);
}
