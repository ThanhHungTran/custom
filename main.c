#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "driver/gpio.h"

static const char *TAG = "APP";

#define LED_GPIO 2

static volatile int wifi_connected = 0;

/* ===================== GPIO LED ===================== */
static void led_init(void)
{
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}

static void led_set(int on)
{
    gpio_set_level(LED_GPIO, on);
}

/* ===================== WIFI EVENT HANDLER ===================== */
static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_connected = 0;
        ESP_LOGW(TAG, "WiFi disconnected, reconnecting...");
        esp_wifi_connect();
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        wifi_connected = 1;
        ESP_LOGI(TAG, "Got IP!");
    }
}

/* ===================== WIFI INIT ===================== */
static void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Nha Chung",
            .password = "tutradi1",
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

/* ===================== TCP CLIENT (LwIP SOCKET) ===================== */
static void tcp_client_task(void *pvParameters)
{
    char rx_buffer[128];

    while (1) {
        if (!wifi_connected) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr("192.168.1.100"); // server IP
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(5000);

        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Socket create failed");
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Connect failed");
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG, "Connected to server");

        while (wifi_connected) {
            const char *msg = "hello from ESP32\n";
            send(sock, msg, strlen(msg), 0);

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len > 0) {
                rx_buffer[len] = 0;
                ESP_LOGI(TAG, "Received: %s", rx_buffer);
            }

            vTaskDelay(pdMS_TO_TICKS(2000));
        }

        close(sock);
    }
}

/* ===================== LED TASK ===================== */
static void led_task(void *pvParameters)
{
    while (1) {
        if (wifi_connected) {
            led_set(1);
            vTaskDelay(pdMS_TO_TICKS(1000));
            led_set(0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            led_set(1);
            vTaskDelay(pdMS_TO_TICKS(200));
            led_set(0);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

/* ===================== app_main ===================== */
void app_main(void)
{
    nvs_flash_init();

    led_init();

    wifi_init();

    xTaskCreate(tcp_client_task, "tcp_client_task", 4096, NULL, 5, NULL);
    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "System started");
}