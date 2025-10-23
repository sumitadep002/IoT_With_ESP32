#include "esp_http_server.h"

static const char login_page[] =
    "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>ESP Login</title><style>"
    "body{font-family:Arial,Helvetica,sans-serif;background:#f2f2f2} .box{width:320px;margin:80px auto;padding:20px;background:#fff;border-radius:8px;box-shadow:0 2px 8px rgba(0,0,0,0.1)}"
    "input{width:100%;padding:10px;margin:8px 0;box-sizing:border-box} button{width:100%;padding:10px;background:#4CAF50;color:#fff;border:0;border-radius:4px}"
    "</style></head><body><div class='box'><h3>Login</h3>"
    "<form action='/login' method='post'>"
    "<input type='text' name='username' placeholder='Username' required>"
    "<input type='password' name='password' placeholder='Password' required>"
    "<button type='submit'>Login</button></form></div></body></html>";

static esp_err_t login_get_handler(httpd_req_t *req);

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

    return true;
}

esp_err_t login_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");                   // set content type
    httpd_resp_send(req, login_page, HTTPD_RESP_USE_STRLEN); // send HTML
    return ESP_OK;
}