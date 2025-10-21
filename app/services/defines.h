#ifndef DEFINES_H
#define DEFINES_H

#define MILLIS() pdTICKS_TO_MS(xTaskGetTickCount())

#define PING_INTERVAL 5000
#define TEST_HTTP_INTERVAL 15000
#define TEST_HTTPS_INTERVAL 13000

#define WIFI_SSID "Samsung"
#define WIFI_PASS "66778899"

#define WIFI_AP_SSID "ESP32_AP"
#define WIFI_AP_PASS "esp@123456789"
#define WIFI_AP_MAX_CLIENTS 1
#define WIFI_AP_CHANNEL 1

#endif // DEFINES_H