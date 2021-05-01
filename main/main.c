/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "driver/gpio.h"
#include "protocol_examples_common.h"

#include <esp_http_server.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "garden";
static bool SOL1status = false;
static bool SOL2status = false;
static bool SOL3status = false;
static bool SOL4status = false;

#define SOL1 GPIO_NUM_14
#define SOL2 GPIO_NUM_27
#define SOL3 GPIO_NUM_16
#define SOL4 GPIO_NUM_17

void SendHTML(char *ptr)
{
    uint8_t sol1Stat = SOL1status;
    uint8_t sol2Stat = SOL2status;
    uint8_t sol3Stat = SOL3status;
    uint8_t sol4Stat = SOL4status;

    strcat(ptr, "<!DOCTYPE html> <html>\n");
    strcat(ptr, "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n");
    strcat(ptr, "<title>SOL Control</title>\n");
    strcat(ptr, "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n");
    strcat(ptr, "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n");
    strcat(ptr, ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n");
    strcat(ptr, ".button-on {background-color: #3498db;}\n");
    strcat(ptr, ".button-on:active {background-color: #2980b9;}\n");
    strcat(ptr, ".button-off {background-color: #34495e;}\n");
    strcat(ptr, ".button-off:active {background-color: #2c3e50;}\n");
    strcat(ptr, "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n");
    strcat(ptr, "</style>\n");
    strcat(ptr, "</head>\n");
    strcat(ptr, "<body>\n");
    strcat(ptr, "<h1>Fern Garden Controller</h1>\n");

    strcat(ptr, "<h2>Drippers</h2>\n");
    if (sol1Stat)
    {
        strcat(ptr, "<p>Tap 1 Status: ON</p><a class=\"button button-off\" href=\"/sol1off\">OFF</a>\n");
    }
    else
    {
        strcat(ptr, "<p>Tap 1 Status: OFF</p><a class=\"button button-on\" href=\"/sol1on\">ON</a>\n");
    }

    if (sol2Stat)
    {
        strcat(ptr, "<p>Tap 2 Status: ON</p><a class=\"button button-off\" href=\"/sol2off\">OFF</a>\n");
    }
    else
    {
        strcat(ptr, "<p>Tap 2 Status: OFF</p><a class=\"button button-on\" href=\"/sol2on\">ON</a>\n");
    }

    if (sol3Stat)
    {
        strcat(ptr, "<p>Tap 3 Status: ON</p><a class=\"button button-off\" href=\"/sol3off\">OFF</a>\n");
    }
    else
    {
        strcat(ptr, "<p>Tap 3 Status: OFF</p><a class=\"button button-on\" href=\"/sol3on\">ON</a>\n");
    }

    strcat(ptr, "<h2>Misters</h2>\n");
    if (sol4Stat)
    {
        strcat(ptr, "<p>Tap 4 Status: ON</p><a class=\"button button-off\" href=\"/sol4off\">OFF</a>\n");
    }
    else
    {
        strcat(ptr, "<p>Tap 4 Status: OFF</p><a class=\"button button-on\" href=\"/sol4on\">ON</a>\n");
    }


    strcat(ptr, "</body>\n");
    strcat(ptr, "</html>\n");
}

static esp_err_t home_get_handler(httpd_req_t *req)
{
    SOL1status = false;
    SOL2status = false;

    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    return ESP_OK;
}

static esp_err_t sol1on_Handler(httpd_req_t *req)
{
    SOL1status = true;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL1, SOL1status);

    return ESP_OK;
}

static esp_err_t sol1off_Handler(httpd_req_t *req)
{
    SOL1status = false;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL1, SOL1status);

    return ESP_OK;
}

static esp_err_t sol2on_Handler(httpd_req_t *req)
{
    SOL2status = true;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL2, SOL2status);

    return ESP_OK;
}

static esp_err_t sol2off_Handler(httpd_req_t *req)
{
    SOL2status = false;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL2, SOL2status);

    return ESP_OK;
}

static esp_err_t sol3on_Handler(httpd_req_t *req)
{
    SOL3status = true;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL1, SOL1status);

    return ESP_OK;
}

static esp_err_t sol3off_Handler(httpd_req_t *req)
{
    SOL3status = false;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL1, SOL1status);

    return ESP_OK;
}

static esp_err_t sol4on_Handler(httpd_req_t *req)
{
    SOL4status = true;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL2, SOL2status);

    return ESP_OK;
}

static esp_err_t sol4off_Handler(httpd_req_t *req)
{
    SOL4status = false;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(SOL2, SOL2status);

    return ESP_OK;
}


static const httpd_uri_t sol1on = {
    .uri = "/sol1on",
    .method = HTTP_GET,
    .handler = sol1on_Handler};

static const httpd_uri_t sol1off = {
    .uri = "/sol1off",
    .method = HTTP_GET,
    .handler = sol1off_Handler};

static const httpd_uri_t sol2on = {
    .uri = "/sol2on",
    .method = HTTP_GET,
    .handler = sol2on_Handler};

static const httpd_uri_t sol2off = {
    .uri = "/sol2off",
    .method = HTTP_GET,
    .handler = sol2off_Handler};

static const httpd_uri_t sol3on = {
    .uri = "/sol3on",
    .method = HTTP_GET,
    .handler = sol3on_Handler};

static const httpd_uri_t sol3off = {
    .uri = "/sol3off",
    .method = HTTP_GET,
    .handler = sol3off_Handler};

static const httpd_uri_t sol4on = {
    .uri = "/sol4on",
    .method = HTTP_GET,
    .handler = sol4on_Handler};

static const httpd_uri_t sol4off = {
    .uri = "/sol4off",
    .method = HTTP_GET,
    .handler = sol4off_Handler};

static const httpd_uri_t home = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = home_get_handler,
    .user_ctx = "home handler"};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &home);
        httpd_register_uri_handler(server, &sol1on);
        httpd_register_uri_handler(server, &sol1off);
        httpd_register_uri_handler(server, &sol2on);
        httpd_register_uri_handler(server, &sol2off);
        httpd_register_uri_handler(server, &sol3on);
        httpd_register_uri_handler(server, &sol3off);
        httpd_register_uri_handler(server, &sol4on);
        httpd_register_uri_handler(server, &sol4off);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server)
    {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void init_gpio(void)
{
    gpio_pad_select_gpio(SOL1);
    gpio_set_direction(SOL1, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(SOL2);
    gpio_set_direction(SOL2, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(SOL3);
    gpio_set_direction(SOL3, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(SOL4);
    gpio_set_direction(SOL4, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    init_gpio();

    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    /* Start the server for the first time */
    server = start_webserver();
}
