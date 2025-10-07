#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "regex.h"

#define DEFAULT_SCAN_LIST_SIZE 1
#define MIN_RSSI -30
#define TAG "WIFI-SCANNING"

static void setup_wifi();
static void wifi_scanner();

void app_main(void)
{
    setup_wifi();

    while (1)
    {
        wifi_scanner();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup_wifi()
{

    // Initialize NVS
    // This is important to be done as wifi uses nvs to store phy layer parameter, user credentials, tx-power level etc,...
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize lightweight tcp/ip stack
    // following task will create a thread to handle  network events such as incoming packets, timers, DHCP, ARP, etc
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_scanner()
{
    esp_wifi_scan_start(NULL, true);

    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    memset(ap_info, 0, sizeof(ap_info));

    uint16_t temp = DEFAULT_SCAN_LIST_SIZE;

    // retrive device
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&temp, ap_info));

    for (int i = 0; i < DEFAULT_SCAN_LIST_SIZE && ap_info[i].rssi > MIN_RSSI; i++)
    {
        ESP_LOGI(TAG, "%s, %d", ap_info[i].ssid, ap_info[i].rssi);
    }
}
