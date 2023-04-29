#include "EaaS/EaaS_nvs.h"

#include <Arduino.h>

#include <string.h>
#include <esp_log.h>

esp_err_t init_Eaas_nvs(void) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW("NVS", "Erasing NVS flash partition: %s", esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if(err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) initializing NVS!", esp_err_to_name(err));
        ESP_ERROR_CHECK( err );
    }

    ESP_LOGI("NVS", "NVS initialized");
    return ESP_OK;
}

esp_err_t open_Eaas_nvs(nvs_handle_t *handle, const char* name) {
    esp_err_t err = nvs_open(name, NVS_READWRITE, handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        ESP_LOGI("NVS", "NVS handle opened");
    }

    ESP_ERROR_CHECK( err );
    return err;
}

esp_err_t get_sta_ssid(nvs_handle_t *handle, char** ssid) {
    const char* GET_SSID_TAG = "GET ssid";
    const char* nvs_key = "sta_ssid";

    esp_err_t err = get_nvs_str(handle, nvs_key, ssid);
    if(err != ESP_OK) {
        ESP_LOGE(GET_SSID_TAG, "Error (%s) reading %s!", esp_err_to_name(err), nvs_key);
        free(*ssid);
    } else {
        ESP_LOGI(GET_SSID_TAG, "%s(%d): %s", nvs_key, strlen(*ssid), *ssid);
    }

    return err;
}

esp_err_t get_sta_password(nvs_handle_t *handle, char** password) {
    const char* GET_PASSWD_TAG = "GET password";
    const char* nvs_key = "sta_passwd";

    esp_err_t err = get_nvs_str(handle, nvs_key, password);
    if(err != ESP_OK) {
        ESP_LOGE(GET_PASSWD_TAG, "Error (%s) reading %s!", esp_err_to_name(err), nvs_key);
        free(*password);
    } else {
        ESP_LOGI(GET_PASSWD_TAG, "%s(%d): %s", nvs_key, strlen(*password), *password);
    }

    return err;
}

esp_err_t get_user_password(nvs_handle_t *handle, char** password) {
    const char* GET_PASSWD_TAG = "GET user password";
    const char* nvs_key = "user_passwd";

    esp_err_t err = get_nvs_str(handle, nvs_key, password);
    if(err != ESP_OK) {
        ESP_LOGE(GET_PASSWD_TAG, "Error (%s) reading %s!", esp_err_to_name(err), nvs_key);
        free(*password);
    } else {
        ESP_LOGI(GET_PASSWD_TAG, "%s(%d): %s", nvs_key, strlen(*password), *password);
    }

    return err;
}

esp_err_t get_nvs_str(nvs_handle_t *handle, const char* nvs_key, char** value) {
    const char* GET_NVS_STR_TAG = "GET nvs str";

    // read data
    size_t required_size = 0;
    esp_err_t err = nvs_get_str(*handle, nvs_key, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(GET_NVS_STR_TAG, "Error (%s) reading %s!", esp_err_to_name(err), nvs_key);
    } else {
        *value = (char *)malloc( sizeof(char) * required_size );
        err = nvs_get_str(*handle, nvs_key, *value, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(GET_NVS_STR_TAG, "Error (%s) reading %s!", esp_err_to_name(err), nvs_key);
        } else {
            *(*value + required_size) = 0;
            ESP_LOGI(GET_NVS_STR_TAG, "%s(%d): %s", nvs_key, strlen(*value), *value);
        }
    }

    return err;
}

esp_err_t get_mac(nvs_handle_t *handle, uint8_t* mac, char** user_name) {
    const char* GET_NVS_MAC_TAG = "GET nvs mac";

    uint64_t mac_buffer = 0;
    esp_err_t err = nvs_get_u64(*handle, "sta_mac", &mac_buffer);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(GET_NVS_MAC_TAG, "Error (%s) reading mac_buffer!", esp_err_to_name(err));
    } else {
        ESP_LOGI(GET_NVS_MAC_TAG, "mac_buffer: %llu", mac_buffer);
        for(int i = 0; i < 6; i++) {
            mac[i] = (mac_buffer >> (i * 8)) & 0xFF;
        }
        ESP_LOGI(GET_NVS_MAC_TAG, "mac: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        *user_name = (char *)malloc( sizeof(char) * 13 );
        sprintf(*user_name, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        ESP_LOGI(GET_NVS_MAC_TAG, "user_name(%d): %s", strlen(*user_name), *user_name);
    }

    return err;
}

esp_err_t close_Eaas_nvs(nvs_handle_t *handle) {
    // Close
    nvs_close(*handle);

    return ESP_OK;
}
