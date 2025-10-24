#ifndef DEFINES_H
#define DEFINES_H

#define MILLIS() pdTICKS_TO_MS(xTaskGetTickCount())

#define PING_INTERVAL 8000
#define PING_URL "http://www.google.com"

#define WIFI_SSID "Samsung"
#define WIFI_PASS "66778899"

#define WIFI_AP_SSID "ESP32_AP"
#define WIFI_AP_PASS "esp@123456789"
#define WIFI_AP_MAX_CLIENTS 1
#define WIFI_AP_CHANNEL 1

#define LOGIN_ID "admin"
#define LOGIN_PASS "admin"

#define NEO_LED_GPIO GPIO_NUM_48

#endif // DEFINES_H