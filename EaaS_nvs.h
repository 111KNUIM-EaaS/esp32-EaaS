#ifndef __EaaS_NVS_H__
#define __EaaS_NVS_H__

#include <esp_err.h>
#include <nvs_flash.h>

esp_err_t init_Eaas_nvs     (void);

esp_err_t open_Eaas_nvs     (nvs_handle_t *handle, const char* name);

esp_err_t get_sta_ssid      (nvs_handle_t *handle, char** ssid);
esp_err_t get_sta_password  (nvs_handle_t *handle, char** password);
esp_err_t get_user_password (nvs_handle_t *handle, char** password);
esp_err_t get_nvs_str       (nvs_handle_t *handle, const char* name, char** value);

esp_err_t get_mac           (nvs_handle_t *handle, uint8_t* mac, char** user_name);

esp_err_t close_Eaas_nvs    (nvs_handle_t *handle);

#endif // __EaaS_NVS_H__