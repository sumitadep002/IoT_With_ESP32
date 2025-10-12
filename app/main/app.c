#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "lwip/dns.h"

#define TAG_MAIN "MAIN"
#define TAG_WIFI "WIFI"
#define TAG_NW_INFO "NETWORK_INFO"

static void wifi_init_sta();
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data);
static void print_network_info(void);

void app_main(void)
{
    wifi_init_sta();

    while (1)
    {
        ESP_LOGI(TAG_MAIN, "Running....");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void wifi_init_sta()
{
    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("wifi_init", ESP_LOG_WARN);
    esp_log_level_set("esp_netif", ESP_LOG_WARN);
    esp_log_level_set("lwip", ESP_LOG_WARN);
    esp_log_level_set("esp_event", ESP_LOG_WARN);
    esp_log_level_set("phy", ESP_LOG_WARN);
    esp_log_level_set("system_api", ESP_LOG_WARN);
    esp_log_level_set("esp_netif_handlers", ESP_LOG_WARN);

    ESP_LOGI(TAG_WIFI, "");

    // Initialize NVS
    // This is important to be done as wifi uses nvs to store phy layer parameter, user credentials, tx-power level etc,...
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_netif_init();                    // Initialize TCP/IP stack
    esp_event_loop_create_default();     // Create event loop
    esp_netif_create_default_wifi_sta(); // Create default station interface, this will be studies later

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg); // Initialize Wi-Fi with default config

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);

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

    ESP_LOGI(TAG_WIFI, "...DONE");
}

// Event handler
void event_handler(void *arg, esp_event_base_t event_base,
                   int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG_WIFI, "Disconnected. Reconnecting...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(TAG_WIFI, "Connected: ");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        print_network_info();
    }
}

void print_network_info(void)
{
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);

    ESP_LOGI(TAG_NW_INFO, "================== Network Info ==================");
    ESP_LOGI(TAG_NW_INFO, "IP Address:    " IPSTR, IP2STR(&ip_info.ip));
    ESP_LOGI(TAG_NW_INFO, "Subnet Mask:   " IPSTR, IP2STR(&ip_info.netmask));
    ESP_LOGI(TAG_NW_INFO, "Gateway:       " IPSTR, IP2STR(&ip_info.gw));

    const ip_addr_t *dns_server = dns_getserver(0);
    ESP_LOGI(TAG_NW_INFO, "Primary DNS:   " IPSTR, IP2STR(&dns_server->u_addr.ip4));

    dns_server = dns_getserver(1);
    ESP_LOGI(TAG_NW_INFO, "Secondary DNS: " IPSTR, IP2STR(&dns_server->u_addr.ip4));
    ESP_LOGI(TAG_NW_INFO, "=================================================");
}