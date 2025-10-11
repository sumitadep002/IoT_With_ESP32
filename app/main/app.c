#include "esp_log.h"
#include "nvs_flash.h"

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

    ESP_LOGI(tag, "...DONE");
}