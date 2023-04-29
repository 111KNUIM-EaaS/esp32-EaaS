#include <Arduino.h>
#include <EaaS.h>

#include "machine.h"

#include <esp_log.h>
#include <Adafruit_SSD1306.h>

#define EVER ;;

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

static uint16_t machine_task_count = 0;
static uint8_t *display_buffer = NULL;

extern bool task_status;

void setup() {
    // Init EaaS library
    init_EaaS();

    // Init SSD1306
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();

    display_buffer = (uint8_t *)malloc( sizeof(uint8_t) * _3D_MODULE_LENGTH);
    memset(display_buffer, 0, _3D_MODULE_LENGTH);
}

void loop() {
    EaaS_OTA();
    display.clearDisplay();

    // show test
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    if(task_status == 0) {
        display.printf("status: stop");
    } else {
        display.printf("status: run");
    }

    if(machine_task_count >= _3D_MODULE_LENGTH) {
        machine_task_count = 0;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        memset(display_buffer, 0, _3D_MODULE_LENGTH);
        display.clearDisplay();
    }

    if(task_status == 1) {
        *(display_buffer + machine_task_count) |= _3D_Module[machine_task_count];

        if(*(display_buffer + machine_task_count) == 0) {
            vTaskDelay(30 / portTICK_PERIOD_MS);

        } else {
            display.drawBitmap(0, 0, display_buffer, 128, 64, 1);
            display.display();

            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        machine_task_count++;
    } else {
        if(task_status == 0) {
            display.drawBitmap(0, 0, _STOP, 128, 64, 1);
            display.display();
        }
    }

    delay(30);
}
