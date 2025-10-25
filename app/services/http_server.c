#include <time.h>
#include "http_client.h"
#include "esp_http_server.h"
#include "defines.h"
#include "wifi.h"
#include "neo_led.h"

static const char common_header[] =
    "<style>"
    "body{font-family:Arial,Helvetica,sans-serif;background:#f2f2f2;margin:0;padding:0;}"
    ".topbar{position:fixed;top:0;left:0;width:100%%;height:40px;"
    "background:#fff;box-shadow:0 2px 5px rgba(0,0,0,0.1);display:flex;"
    "justify-content:space-between;align-items:center;padding:0 15px;font-size:14px;}"
    ".status{font-size:13px;color:#555;}"
    ".ok{color:green;}.fail{color:red;}"
    "#clock{font-weight:bold;color:#333;}"
    "main{padding-top:60px;}"
    "</style>"

    "<div class='topbar'>"
    "<div class='status'>Wi-Fi: <span id='wifi-status'>—</span> | Internet: <span id='internet-status'>—</span></div>"
    "<div id='clock'>--:--:--</div>"
    "</div>"

    "<script>"
    "function updateClock(){fetch('/time').then(r=>r.text()).then(t=>document.getElementById('clock').innerText=t);} "
    "setInterval(updateClock,1000);updateClock();"

    "function updateStatus(){fetch('/status').then(r=>r.json()).then(d=>{"
    "document.getElementById('wifi-status').innerText=d.wifi;"
    "document.getElementById('internet-status').innerText=d.internet;"
    "document.getElementById('wifi-status').className=(d.wifi==='connected')?'ok':'fail';"
    "document.getElementById('internet-status').className=(d.internet==='available')?'ok':'fail';"
    "});}"
    "setInterval(updateStatus,3000);updateStatus();"
    "</script>";

static const char login_page[] =
    "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>ESP Login</title></head><body>"
    "%s" // <---- placeholder for common_header
    "<main><div class='box' style='width:320px;margin:80px auto;padding:20px;background:#fff;border-radius:8px;box-shadow:0 2px 8px rgba(0,0,0,0.1)'>"
    "<h3>Login</h3>"
    "<form action='/login' method='post'>"
    "<input type='text' name='username' placeholder='Username' required>"
    "<input type='password' name='password' placeholder='Password' required>"
    "<button type='submit' style='width:100%%;padding:10px;background:#4CAF50;color:#fff;border:0;border-radius:4px;'>Login</button>"
    "</form></div></main></body></html>";

static const char dashboard_page[] =
    "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>ESP Dashboard</title></head><body>"
    "%s" // <---- placeholder for common_header
    "<main><h2>Welcome!</h2><p>Device dashboard content goes here.</p></main>"
    "</body></html>";

static esp_err_t login_get_handler(httpd_req_t *req);
static esp_err_t login_post_handler(httpd_req_t *req);
static esp_err_t time_get_handler(httpd_req_t *req);
static esp_err_t status_get_handler(httpd_req_t *req);

bool http_server_start()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) != ESP_OK)
    {
        return false;
    }

    httpd_uri_t login_get_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = login_get_handler,
        .user_ctx = NULL};

    if (httpd_register_uri_handler(server, &login_get_uri) != ESP_OK)
    {
        httpd_stop(server);
        return false;
    }

    httpd_uri_t login_post_uri = {
        .uri = "/login",
        .method = HTTP_POST,
        .handler = login_post_handler,
        .user_ctx = NULL};

    if (httpd_register_uri_handler(server, &login_post_uri) != ESP_OK)
    {
        httpd_stop(server);
        return false;
    }

    httpd_uri_t time_get_uri = {
        .uri = "/time",
        .method = HTTP_GET,
        .handler = time_get_handler,
        .user_ctx = NULL};

    if (httpd_register_uri_handler(server, &time_get_uri) != ESP_OK)
    {
        httpd_stop(server);
        return false;
    }

    static const httpd_uri_t status_get_uri = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_get_handler,
        .user_ctx = NULL};

    if (httpd_register_uri_handler(server, &status_get_uri) != ESP_OK)
    {
        httpd_stop(server);
        return false;
    }

    return true;
}

esp_err_t login_get_handler(httpd_req_t *req)
{
    char page_buffer[sizeof(login_page) + sizeof(common_header) + 10];

    // For login page:
    snprintf(page_buffer, sizeof(page_buffer), login_page, common_header);

    httpd_resp_set_type(req, "text/html"); // set content type
    httpd_resp_send(req, page_buffer, HTTPD_RESP_USE_STRLEN);
    neo_led_queue_send((neo_led_queue_t){0, 0, 255, 250});
    return ESP_OK;
}

esp_err_t login_post_handler(httpd_req_t *req)
{
    char buf[128];
    int total_len = req->content_len;
    int cur_len = 0;
    int received = 0;

    if (total_len >= sizeof(buf))
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }

    while (cur_len < total_len)
    {
        received = httpd_req_recv(req, buf + cur_len, total_len - cur_len);
        if (received <= 0)
        {
            if (received == HTTPD_SOCK_ERR_TIMEOUT)
                continue;
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[cur_len] = '\0'; // Null-terminate

    char username[32] = {0};
    char password[32] = {0};

    sscanf(buf, "username=%31[^&]&password=%31s", username, password);

    for (int i = 0; username[i]; i++)
    {
        if (username[i] == '+')
        {
            username[i] = ' ';
        }
    }
    for (int i = 0; password[i]; i++)
    {
        if (password[i] == '+')
        {
            password[i] = ' ';
        }
    }

    char page_buffer[sizeof(dashboard_page) + sizeof(common_header) + 10];

    // For login page:
    snprintf(page_buffer, sizeof(page_buffer), dashboard_page, common_header);

    if (strcmp(username, LOGIN_ID) == 0 && strcmp(password, LOGIN_PASS) == 0)
    {
        httpd_resp_send(req, page_buffer, HTTPD_RESP_USE_STRLEN);
    }
    else
    {
        httpd_resp_send(req, "Invalid Credentials!", HTTPD_RESP_USE_STRLEN);
    }

    neo_led_queue_send((neo_led_queue_t){0, 0, 255, 250});

    return ESP_OK;
}

esp_err_t time_get_handler(httpd_req_t *req)
{
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];

    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S, %d-%m-%Y", &timeinfo);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, strftime_buf);

    return ESP_OK;
}

esp_err_t status_get_handler(httpd_req_t *req)
{
    char status_json[128];
    bool wifi_connected = wifi_state_get();
    bool internet_connected = wifi_connected ? http_client_get_internet_status() : false;

    snprintf(status_json, sizeof(status_json),
             "{\"wifi\":\"%s\", \"internet\":\"%s\"}",
             wifi_connected ? "connected" : "disconnected",
             internet_connected ? "available" : "unavailable");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, status_json, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}