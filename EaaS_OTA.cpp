#include "EaaS/EaaS_OTA.h"
#include "EaaS/EaaS_header.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <esp_log.h>

static char *_user   = NULL;
static char *_passwd = NULL;

void init_EaaS_OTA(char *user, char *passwd) {
    _user   = user;
    _passwd = passwd;
}

/**
 * @brief Runs the firmware update process using the provided data stream and command.
 * 
 * @param data The data stream containing the firmware update data.
 * @param size The size of the firmware update data.
 * @param command The update command to execute.
 * @return int Returns 0 if the update was successful, and a non-zero error code otherwise.
 */
int runUpdate(Stream& data, size_t size, int command) {
    // Begin the firmware update process
    if(!Update.begin(size, command))
        return 1;

    // Write the firmware update data to the device
    if(Update.writeStream(data) != size)
        return 2;

    // End the firmware update process
    if(!Update.end())
        return 3;

    return 0;
}

/**
 * @brief Attempts to perform a firmware update using the provided URL.
 * 
 * @param url The URL of the firmware update file.
 * @return int Returns the update result code (-1 if the update failed or was not attempted, 0 if the update was successful).
 */
int update_loop(const String& url) {
    int update_num = -1;

    // Initialize an HTTP client with strict redirect handling
    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.begin(url);

    // Attempt to perform an HTTP GET request on the update URL
    int httpCode = http.GET();

    // If the HTTP request was successful, attempt to perform a firmware update
    if(httpCode == HTTP_CODE_OK) {
        ESP_LOGI("Update firmware", "Succese");
        update_num = runUpdate(*(http.getStreamPtr()), http.getSize(), U_FLASH);
    } else {
        ESP_LOGE("Update firmware", "Failed, error code: %d", httpCode);
    }

    ESP_LOGI("Update firmware", "Update number: %d", update_num);
    return update_num;
}

int flash_EaaS_firmware() {
    const char *data = "{\"url\":\"/api/espdev/get/url/\"}";

    // Initialize an HTTP client with strict redirect handling
    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.begin(HOST, PORT, "/api/espdev/get/url/");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("user", _user);
    http.addHeader("token", _passwd);

    // Attempt to perform an HTTP GET request on the update URL
    int httpCode = http.POST(data);

    // If the HTTP request was successful, attempt to perform a firmware update
    if(httpCode == HTTP_CODE_OK) {
        String url = http.getString();
        ESP_LOGI("Get firmware url", "Success message: %s", url.c_str());
        int update_code = update_loop(url);
        if(update_code == 0) {
            ESP_LOGI("Update firmware", "Success");
            http.end();
            return 0;
        } else {
            ESP_LOGE("Update firmware", "Failed, error code: %d", update_code);
            http.end();
            return -1;
        }

    } else {
        ESP_LOGE("Get firmware url", "Failed, error code: %d", httpCode);
        // close the connection:
        http.end();
        return -1;
    }

    return -2;
}