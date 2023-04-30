#include "EaaS.h"
#include "EaaS_nvs.h"
#include "EaaS_network.h"
#include "EaaS_client.h"
#include "EaaS_OTA.h"

#include "driver/gpio.h"

#include <Arduino.h>

#include <esp_log.h>

static uint8_t machine_status = 0;

static char* user_passwd   = NULL;
static char* user_name     = NULL;

static TaskHandle_t EaaS_init_blink_handle;
static TaskHandle_t EaaS_handle;

uint8_t task_status = 0;

void loop_eaas(void *parameter) {
    // set_machine_status(4);

    uint64_t count = 0;
    ESP_LOGI("Loop EaaS", "Loop EaaS task started");

    while(1) {
        // Get machine status
        machine_status = loop_eaas_status();
        ESP_LOGI("Loop EaaS", "Status: %d, count: %d", machine_status, (int)count);
        count+=1;

        switch (machine_status) {
            case 0:
                task_status = 0;
                break;

            case 1:
                task_status = 1;
                set_machine_status(2);
                break;

            case 2:
                task_status = 1;
                break;

            case 3:
                task_status = 0;
                set_machine_status(4);
                break;
            
            case 4:
                task_status = 0;
                break;

            case 5:
                task_status = 2;
                break;
            
            case 6:
                set_machine_status(4);
                task_status = 0;
                break;

            default:
                break;
        }

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void blink_loop(void *parameter) {
    while(1) {
        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void init_EaaS(void) {
    ESP_LOGI("Build Date Time", "%s %s", __DATE__,  __TIME__);
    gpio_config_t io_conf = {
        .pin_bit_mask   = (1ULL << (GPIO_NUM_2)),
        .mode           = GPIO_MODE_OUTPUT,
        .pull_up_en     = GPIO_PULLUP_DISABLE,
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .intr_type      = GPIO_INTR_DISABLE,
    };

    gpio_config(&io_conf);

    gpio_set_level(GPIO_NUM_2, 0);

    xTaskCreatePinnedToCore( blink_loop, "EaaS init blink", 1024, NULL, 1, &EaaS_init_blink_handle, 0 );

    char* sta_ssid      = NULL;
    char* sta_passwd    = NULL;

    uint8_t mac[6] = {0};

    init_Eaas_nvs();

    nvs_handle_t handle;

    open_Eaas_nvs       (&handle, "storage");
    get_sta_ssid        (&handle, &sta_ssid);
    get_sta_password    (&handle, &sta_passwd);
    get_user_password   (&handle, &user_passwd);

    get_mac             (&handle, mac, &user_name);

    close_Eaas_nvs      (&handle);

    init_EaaS_network(sta_ssid, sta_passwd, mac);
    init_client(user_name, user_passwd);
    init_EaaS_OTA(user_name, user_passwd);

    vTaskDelete(EaaS_init_blink_handle);
    gpio_set_level(GPIO_NUM_2, 1);

    // 327680
    // 102400
    xTaskCreatePinnedToCore( loop_eaas, "machine_task", 102400, NULL, 1, &EaaS_handle, 0 );
}

void EaaS_OTA() {
    if(task_status == 2) {
        vTaskDelete(EaaS_handle);
        xTaskCreatePinnedToCore( blink_loop, "EaaS init blink", 1024, NULL, 1, &EaaS_init_blink_handle, 0 );
        
        flash_EaaS_firmware();
        
        vTaskDelete(EaaS_init_blink_handle);
        gpio_set_level(GPIO_NUM_2, 1);
        
        set_machine_status(6);
        xTaskCreatePinnedToCore( loop_eaas, "machine_task", 102400, NULL, 1, &EaaS_handle, 0 );
    }
}
