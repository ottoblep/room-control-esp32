#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "send_codes.c"
#include "udp_server.c"
#include "gpio_handler.c"

TaskHandle_t state_controller_handle = NULL;
TaskHandle_t transmitter_handle = NULL;
TaskHandle_t udp_server_handle = NULL;

struct State {
    bool room_light_on;
} state;

/**
 * Main controller of the state machine receives switch requests and sends actions
 * This task must be started first to initialize shared resources
 */
static void state_controller_task(void *pvParameters) {
    state.room_light_on = false;
    // Wait for other tasks to initialize
    vTaskDelay(pdMS_TO_TICKS(400));
    // Send disable light signal to be sure its off
    // xTaskNotify( transmitter_handle, 1, eSetValueWithOverwrite );

    uint32_t message;

    while(1) {
        // Wait for Note from Button or UDP to switch
        vTaskDelay(pdMS_TO_TICKS(2000)); // debounce
        ESP_LOGI(TAG, "Controller Standing By.");
        xTaskNotifyStateClear(NULL);
        if (xTaskNotifyWait( ULONG_MAX, ULONG_MAX, &message, portMAX_DELAY )) {
            ESP_LOGI(TAG, "Switch Request received.");
            if(state.room_light_on) message = 1;
            else message = 0;
            xTaskNotify( transmitter_handle, message, eSetValueWithOverwrite );
            state.room_light_on = !state.room_light_on; //Flip
        }
    }
}

/**
 * Sends a code to the 433Mhz transmitter to switch the lights on or off
 *
 * Sleeps until given a direct notification. Then switches to the binary code in the notification.
 */
static void transmitter_task(void *pvParameters) {
    uint32_t note;
    while(1) {
        ESP_LOGI(TAG, "Transmitter Standing By.");
        if (xTaskNotifyWait( ULONG_MAX, 0, &note, portMAX_DELAY )) {
            if (note == 0) { // If light is off switch it on
                ESP_LOGI(TAG, "Sending Light on Signal!");
                chan1on(20);
            } else { // If light is on switch it off
                ESP_LOGI(TAG, "Sending Light off Signal!");
                chan1off(20);
            }
        }
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    setup_button_handler(&state_controller_handle);
    setup_debo_gpio();

    xTaskCreate(state_controller_task, "state_controller", 4096, NULL, 4, &state_controller_handle);
    xTaskCreate(transmitter_task, "transmitter", 4096, NULL, 3, &transmitter_handle);
    xTaskCreate(custom_udp_server_task, "udp_server", 4096, (void *) &state_controller_handle, 2, &udp_server_handle);
}