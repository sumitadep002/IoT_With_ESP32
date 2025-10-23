#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "defines.h"
#include "wifi.h"
#include "http_client.h"

#define TAG_MAIN "MAIN"

void app_main(void)
{
    wifi_init_apsta();
    http_client_ping_init();

    while (1)
    {

        ESP_LOGI(TAG_MAIN, "Running...");

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}