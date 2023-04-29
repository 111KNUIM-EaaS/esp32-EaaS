#include "EaaS/EaaS_network.h"

// include Arduino headers
#include <Arduino.h>
#include <WiFi.h>

// include ESP32 IDF headers
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_netif.h>

// define constants
#define EXAMPLE_ESP_MAXIMUM_RETRY  10

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

// define static variables
static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    const char *WIFI_EVENT_TAG = "wifi event station";
    static uint8_t s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            ESP_LOGW(WIFI_EVENT_TAG, "retry to connect to the AP(%d)", s_retry_num++);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            esp_restart();
        }
        ESP_LOGE(WIFI_EVENT_TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_EVENT_TAG, "Got sta ip address:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void init_EaaS_network(const char* ssid, const char* password, const uint8_t* mac) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    // const char *WIFI_TAG = "wifi station";
    // ESP_LOGI(WIFI_TAG, "ESP_WIFI_MODE_STA");

    // s_wifi_event_group = xEventGroupCreate();

    // ESP_ERROR_CHECK(esp_netif_init());

    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    // esp_netif_create_default_wifi_sta();

    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // // set sta mac
    // esp_wifi_set_mac(WIFI_IF_STA, mac);

    // // Create a network interface instance
    // // esp_netif_t *netif = esp_netif_create_default_wifi_sta();

    // // // Set the DNS server to 8.8.8.8 (Google DNS server)
    // // ip4_addr_t dns_server;
    // // inet_pton(AF_INET, "8.8.8.8", &dns_server);
    // // esp_netif_dns_info_t dns_info = {
    // //     .ip = dns_server
    // // };
    // // esp_err_t err = esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
    // // if (err != ESP_OK) {
    // //     ESP_LOGE(TAG, "Error setting DNS server: %d", err);
    // // }

    // esp_event_handler_instance_t instance_any_id;
    // esp_event_handler_instance_t instance_got_ip;
    // ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    //                                                     ESP_EVENT_ANY_ID,
    //                                                     &event_handler,
    //                                                     NULL,
    //                                                     &instance_any_id));
    // ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
    //                                                     IP_EVENT_STA_GOT_IP,
    //                                                     &event_handler,
    //                                                     NULL,
    //                                                     &instance_got_ip));

    // wifi_config_t wifi_config = {
    //     .sta = {
    //         .ssid = {0},
    //         .password = {0},
    //         .threshold = {
    //             .authmode = WIFI_AUTH_WPA2_PSK,
    //         },
    //     },
    // };
    // memset(wifi_config.sta.ssid, 0, sizeof(wifi_config.sta.ssid));
    // memset(wifi_config.sta.password, 0, sizeof(wifi_config.sta.password));
    // memcpy(wifi_config.sta.ssid, ssid, strlen(ssid) + 1);
    // memcpy(wifi_config.sta.password, password, strlen(password) + 1);

    // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    // ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    // ESP_ERROR_CHECK(esp_wifi_start() );

    // ESP_LOGI(WIFI_TAG, "wifi_init_sta finished.");

    // EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
    //         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
    //         pdFALSE,
    //         pdFALSE,
    //         portMAX_DELAY);

    // if (bits & WIFI_CONNECTED_BIT) {
    //     ESP_LOGI(WIFI_TAG, "connected to ap SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    // } else if (bits & WIFI_FAIL_BIT) {
    //     ESP_LOGI(WIFI_TAG, "Failed to connect to SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    // } else {
    //     ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
    // }
}
