#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"

void wifi_init_sta();

void app_main(void)
{
    wifi_init_sta();

    while (1)
    {
    }
}

void wifi_init_sta()
{
    char *tag = "WIFI-INIT";

    ESP_LOGI(tag, "");

    // Initialize NVS
    // This is important to be done as wifi uses nvs to store phy layer parameter, user credentials, tx-power level etc,...
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_netif_init();                    // Initialize TCP/IP stack
    esp_event_loop_create_default();     // Create event loop
    esp_netif_create_default_wifi_sta(); // Create default station interface, this will be studies later

    ESP_LOGI(tag, "...DONE");
}