#include <stdbool.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_sntp.h"
#include <time.h>
#include "defines.h"
#include "wifi.h"
#include "http.h"

#define TAG_PING "PING"
#define TAG_NTP "NTP"

static volatile bool gf_ntp_updated = false;

static bool get_ntp();

void http_ping()
{
    static uint32_t ping_timer;
    if (MILLIS() - ping_timer > PING_INTERVAL && wifi_state_get() == true)
    {
        ping_timer = MILLIS();
        esp_http_client_config_t config = {
            .url = PING_URL,
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);

        esp_err_t err = esp_http_client_perform(client);

        if (err != ESP_OK)
        {
            ESP_LOGE(TAG_PING, "NO-INTERNET");
        }
        else
        {
            ESP_LOGI(TAG_PING, "OK");
            if (gf_ntp_updated == false)
            {
                gf_ntp_updated = get_ntp();
            }
        }

        esp_http_client_cleanup(client);
    }
}

bool get_ntp()
{
    // Initialize SNTP
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org"); // or "time.google.com"
    esp_sntp_init();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    // Set time zone to IST (India Standard Time)
    setenv("TZ", "IST-5:30", 1);
    tzset();

    time(&now);
    localtime_r(&now, &timeinfo);

    ESP_LOGI(TAG_NTP, "Current time: %s", asctime(&timeinfo));

    return true;
}
