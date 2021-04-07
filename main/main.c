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

static const char *TAG = "example";
static bool LED1status = false;
static bool LED2status = false;

#define LED1 GPIO_NUM_18
#define LED2 GPIO_NUM_0

void SendHTML(char *ptr, uint8_t led1stat, uint8_t led2stat)
{
    strcat(ptr, "<!DOCTYPE html> <html>\n");
    strcat(ptr, "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n");
    strcat(ptr, "<title>LED Control</title>\n");
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
    strcat(ptr, "<h1>ESP32 Web Server</h1>\n");

    if (led1stat)
    {
        strcat(ptr, "<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n");
    }
    else
    {
        strcat(ptr, "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n");
    }

    if (led2stat)
    {
        strcat(ptr, "<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n");
    }
    else
    {
        strcat(ptr, "<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n");
    }

    strcat(ptr, "</body>\n");
    strcat(ptr, "</html>\n");
}

static esp_err_t home_get_handler(httpd_req_t *req)
{
    LED1status = false;
    LED2status = false;

    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str, LED1status, LED2status);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    return ESP_OK;
}

static esp_err_t led1on_Handler(httpd_req_t *req)
{
    LED1status = true;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str, true, LED2status);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(LED1, LED1status);

    return ESP_OK;
}

static esp_err_t led1off_Handler(httpd_req_t *req)
{
    LED1status = false;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str, false, LED2status);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(LED1, LED1status);

    return ESP_OK;
}

static esp_err_t led2on_Handler(httpd_req_t *req)
{
    LED2status = true;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str, LED1status, true);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(LED2, LED2status);

    return ESP_OK;
}

static esp_err_t led2off_Handler(httpd_req_t *req)
{
    LED2status = false;
    char *resp_str;
    resp_str = malloc(2048);
    memset(resp_str, 0, 2048);

    SendHTML(resp_str, LED1status, false);

    httpd_resp_send(req, resp_str, strlen(resp_str));

    free(resp_str);

    gpio_set_level(LED2, LED2status);

    return ESP_OK;
}

static const httpd_uri_t led1on = {
    .uri = "/led1on",
    .method = HTTP_GET,
    .handler = led1on_Handler};

static const httpd_uri_t led1off = {
    .uri = "/led1off",
    .method = HTTP_GET,
    .handler = led1off_Handler};

static const httpd_uri_t led2on = {
    .uri = "/led2on",
    .method = HTTP_GET,
    .handler = led2on_Handler};

static const httpd_uri_t led2off = {
    .uri = "/led2off",
    .method = HTTP_GET,
    .handler = led2off_Handler};

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

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &home);
        httpd_register_uri_handler(server, &led1on);
        httpd_register_uri_handler(server, &led1off);
        httpd_register_uri_handler(server, &led2on);
        httpd_register_uri_handler(server, &led2off);
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

void app_main(void)
{
    gpio_pad_select_gpio(LED1);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

    gpio_set_level(LED1, false);
    gpio_set_level(LED2, false);



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
