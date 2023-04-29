#include "EaaS/EaaS_client.h"
#include "EaaS/EaaS_header.h"

#include <Arduino.h>
#include <esp_log.h>
#include <esp_tls.h>
#include <esp_http_client.h>

static uint8_t _machine_status = 0;

static char *_user   = NULL;
static char *_passwd = NULL;

void init_client(char* user, char* passwd) {
    _user = user;
    _passwd = passwd;
}

void data_handler(const char *data, int len) {
    const char *DATA_HANDLER_TAG = "Data handler"; 

    ESP_LOGI(DATA_HANDLER_TAG, "data_handler[%d]: %s", len, data);

    if(strcmp(data, "Unauthorized") == 0) {
        ESP_LOGE(DATA_HANDLER_TAG, "Unauthorized");
        return;
    }

    if(strcmp(data, "Server Error") == 0) {
        ESP_LOGE(DATA_HANDLER_TAG, "Server Error");
        return;
    }

    if(strcmp(data, "Bad Request") == 0) {
        ESP_LOGE(DATA_HANDLER_TAG, "Bad Request");
        return;
    }

    if(strcmp(data, "Success") == 0) {
        ESP_LOGE(DATA_HANDLER_TAG, "Success");
        return;
    }

    // 0: offline, 1: booting, 2: boot, 3: pausing, 4: pause
    if (strcmp(data, "results0") == 0) {
        ESP_LOGI(DATA_HANDLER_TAG, "offline data");
        _machine_status = 0;
    } else if(strcmp(data, "results1") == 0) {
        ESP_LOGI(DATA_HANDLER_TAG, "booting data");
        _machine_status = 1;
    } else if(strcmp(data, "results2") == 0) {
        ESP_LOGI(DATA_HANDLER_TAG, "boot data");
        _machine_status = 2;
    } else if(strcmp(data, "results3") == 0) {
        ESP_LOGI(DATA_HANDLER_TAG, "pausing data");
        _machine_status = 3;
    } else if(strcmp(data, "results4") == 0) {
        ESP_LOGI(DATA_HANDLER_TAG, "pause data");
        _machine_status = 4;
    } else if(strcmp(data, "results5") == 0) {
        ESP_LOGI(DATA_HANDLER_TAG, "otaing data");
        _machine_status = 5;
    } else if(strcmp(data, "results6") == 0) {
        ESP_LOGI(DATA_HANDLER_TAG, "ota data");
        _machine_status = 6;
    } else {
        ESP_LOGI(DATA_HANDLER_TAG, "Unknown data");
        return;
    }
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    const char *SERVER_TAG = "http info loop"; 

    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(SERVER_TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(SERVER_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(SERVER_TAG, "HTTP_EVENT_HEADER_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            // ESP_LOGD(SERVER_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;

        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                int len = esp_http_client_get_content_length(evt->client);
                ESP_LOGI(SERVER_TAG, "HTTP_EVENT_ON_DATA, len=%d", len);
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc( esp_http_client_get_content_length(evt->client) + 1 );
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(SERVER_TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
                output_buffer[len] = '\0';
                data_handler(output_buffer, len);
            }

            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(SERVER_TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
            
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(SERVER_TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(SERVER_TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(SERVER_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;

        // default:
        //     break;
    }
    return ESP_OK;
}

void server_client(const char* path, const char* data) {
    const char* SERVER_CLIENT_TAG = "server client";
    ESP_LOGI(SERVER_CLIENT_TAG, "Starting server client");
    
    esp_http_client_config_t config = {
        .host = HOST,
        .port = PORT,
        .path = path,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "user", _user);
    esp_http_client_set_header(client, "token", _passwd);

    esp_http_client_set_post_field(client, data, strlen(data));

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(SERVER_CLIENT_TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return;
    }

    ESP_LOGI(SERVER_CLIENT_TAG, "HTTP POST Status = %d", esp_http_client_get_status_code(client));

    esp_http_client_cleanup(client);
}

void set_machine_status(const uint8_t status) {
    char *message = NULL;
    message = (char *) malloc( sizeof(char) * 50 );
    sprintf(message, "{\"url\":\"/api/espdev/set/status\",\"status\":%d}", status);
    server_client("/api/espdev/set/status", message);
}

uint8_t loop_eaas_status() {
    server_client("/api/espdev/get/status", "{\"url\":\"/api/espdev/get/status\"}");
    return _machine_status;
}
