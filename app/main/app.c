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

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg); // Initialize Wi-Fi with default config

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA); // Set Wi-Fi to station mode
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(tag, "...DONE");
}