#include <time.h>
#include "http_client.h"
#include "esp_http_server.h"
#include "defines.h"
#include "wifi.h"
#include "neo_led.h"

static const char common_header[] =
    "<style>"
    "body{font-family:Arial,Helvetica,sans-serif;background:#f7f7f8;margin:0;padding:0;color:#222;}"
    ".topbar{position:fixed;top:0;left:0;width:100%;height:64px;display:flex;align-items:center;"
    "justify-content:center;z-index:1000;background:rgba(255,255,255,0.95);backdrop-filter:blur(4px);"
    "box-shadow:0 1px 4px rgba(0,0,0,0.06);text-align:center;}"
    "#datetime{display:flex;flex-direction:column;align-items:center;line-height:1;}"
    "#date{font-size:13px;color:#666;margin-bottom:4px;}"
    "#clock{font-size:18px;font-weight:600;color:#111;}"
    "main{padding-top:84px;}"
    "@media (max-width:420px){"
    "  #clock{font-size:16px;} #date{font-size:12px;}"
    "  .topbar{height:56px;} main{padding-top:72px;}"
    "}"
    "</style>"

    "<div class='topbar'>"
    "  <div id='datetime'>"
    "    <div id='date'>-- --- ----</div>"
    "    <div id='clock'>--:--:--</div>"
    "  </div>"
    "</div>"

    "<script>"
    "function updateClock(){"
    "  fetch('/time').then(r=>r.text()).then(t=>{"
    "    const parts = t.split(',');"
    "    const clock = (parts[0]||'--:--:--').trim();"
    "    const date = (parts[1]||'').trim();"
    "    const clockEl = document.getElementById('clock');"
    "    const dateEl = document.getElementById('date');"
    "    if(clockEl) clockEl.innerText = clock;"
    "    if(dateEl) dateEl.innerText = date;"
    "  }).catch(()=>{});"
    "}"
    "setInterval(updateClock, 1000); updateClock();"
    "</script>";

/* Login page: placeholder %s for common_header */
static const char login_page[] =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>ESP Login</title></head><body>"
    "%s" /* common_header goes here */
    "<main>"
    "<div class='box' style='width:340px;max-width:90%%;margin:90px auto 40px auto;padding:20px;background:#fff;"
    "border-radius:10px;box-shadow:0 6px 18px rgba(0,0,0,0.06);text-align:left;'>"
    "<h3 style='margin:0 0 12px 0;font-weight:600;color:#111;'>Login</h3>"
    "<form action='/login' method='post' style='display:flex;flex-direction:column;gap:10px;'>"
    "<input type='text' name='username' placeholder='Username' required "
    "style='padding:10px;border:1px solid #e0e0e0;border-radius:6px;font-size:14px;'>"
    "<input type='password' name='password' placeholder='Password' required "
    "style='padding:10px;border:1px solid #e0e0e0;border-radius:6px;font-size:14px;'>"
    "<button type='submit' style='padding:10px;background:#1976d2;color:#fff;border:0;border-radius:6px;cursor:pointer;"
    "font-weight:600;'>Login</button>"
    "</form></div></main></body></html>";

/* Dashboard page: placeholder %s for common_header */
static const char dashboard_page[] =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>ESP Dashboard</title></head><body>"
    "%s" /* common_header goes here */
    "<main style='max-width:980px;margin:0 auto;padding:20px;text-align:center;'>"
    "<h2 style='margin-top:6px;color:#111;'>Device Dashboard</h2>"
    "<button onclick=\"window.location.href='/colorboard'\" "
    "style='padding:10px 20px;background:#1976d2;color:#fff;border:0;border-radius:6px;"
    "cursor:pointer;font-weight:600;margin-top:20px;'>Open Colorboard</button>"
    "</main></body></html>";

static esp_err_t login_get_handler(httpd_req_t *req);
static esp_err_t login_post_handler(httpd_req_t *req);
static esp_err_t time_get_handler(httpd_req_t *req);
static esp_err_t status_get_handler(httpd_req_t *req);

bool http_server_start()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = (1024 * 5);

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