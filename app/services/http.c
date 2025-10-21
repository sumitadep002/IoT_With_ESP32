#include <stdbool.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_sntp.h"
#include <time.h>
#include "defines.h"
#include "wifi.h"

#define TAG_PING "PING"
#define TAG_NTP "NTP"
#define TAG_TEST_HTTP "TEST_HTTP"
#define TAG_TEST_HTTPS "TEST_HTTPS"

static volatile bool gf_ntp_updated = false;

extern const uint8_t httpbin_root_cert_pem_start[] asm("_binary_httpbin_root_cert_pem_start");
extern const uint8_t httpbin_root_cert_pem_end[] asm("_binary_httpbin_root_cert_pem_end");

void ping()
{
    static uint32_t ping_timer;
    if (MILLIS() - ping_timer > PING_INTERVAL && wifi_state_get() == true)
    {
        ping_timer = MILLIS();
        esp_http_client_config_t config = {
            .url = "http://www.google.com", // You can replace with any public website
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

void test_http()
{
    static uint32_t timer;
    if (MILLIS() - timer > TEST_HTTP_INTERVAL)
    {
        timer = MILLIS();

        esp_http_client_config_t config = {
            .url = "http://httpbin.org/get",
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG_TEST_HTTP, "HTTP GET Status = %d, content_length = %lld",
                     esp_http_client_get_status_code(client),
                     esp_http_client_get_content_length(client));
        }
        else
        {
            ESP_LOGE(TAG_TEST_HTTP, "HTTP GET request failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
    }
}

void test_https()
{
    static uint32_t timer;
    if (MILLIS() - timer > TEST_HTTP_INTERVAL)
    {
        timer = MILLIS();

        esp_http_client_config_t config = {
            .url = "https://httpbin.org/get",
            .cert_pem = (const char *)httpbin_root_cert_pem_start,
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG_TEST_HTTPS, "HTTPS GET Status = %d, content_length = %lld",
                     esp_http_client_get_status_code(client),
                     esp_http_client_get_content_length(client));

            char buffer[256];
            int read_len = esp_http_client_read_response(client, buffer, sizeof(buffer) - 1);
            if (read_len > 0)
            {
                buffer[read_len] = 0;
                ESP_LOGI(TAG_TEST_HTTPS, "Response:\n%s", buffer);
            }
        }
        else
        {
            ESP_LOGE(TAG_TEST_HTTPS, "HTTPS GET request failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
    }
}