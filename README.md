# KNU Equipment as a Service(EaaS) for ESP32 EaaS Library

## API List
- [EaaS](https://github.com/111KNUIM-EaaS/esp32-EaaS#-eaas)
- [EaaS_OTA](https://github.com/111KNUIM-EaaS/esp32-EaaS#-eaas_ota)
- [EaaS_nvs](https://github.com/111KNUIM-EaaS/esp32-EaaS#%EF%B8%8F-eaas_nvs)
- [EaaS_network](https://github.com/111KNUIM-EaaS/esp32-EaaS#-eaas_network)
- [EaaS_client](https://github.com/111KNUIM-EaaS/esp32-EaaS#-eaas_network)

## Include Library

platformIO

```ini
lib_deps = 
    https://github.com/111KNUIM-EaaS/esp32-EaaS
```

main.cpp

```cpp
#include <Arduino.h>
#include <EaaS.h>

void setup() {
    init_EaaS();
}

void loop() {
    loop_eaas();
}
```

## API (User can not used)

### 📄 EaaS

```cpp
void init_EaaS(void);
```

📄 call this function in setup()

```cpp
void loop_eaas(void *parameter);
```
📄 When OTA is running, this function is ready to Get OTA data.

```cpp
void EaaS_OTA();
```
📄 When OTA is running, loop_eaas will call to this function.


### 💫 EaaS_OTA

```cpp
void init_EaaS_OTA(char *user, char *passwd);
```
💫 This function is save user and password for EaaS_OTA file.

```cpp
int flash_EaaS_firmware();
```
💫 This function is flash EaaS_OTA firmware. <br>
`return` `0` is susses, other is fail.

### 🗃️ EaaS_nvs
```cpp
esp_err_t init_Eaas_nvs     (void);
```
🗃️ This function is init nvs.

```cpp
esp_err_t open_Eaas_nvs     (nvs_handle_t *handle, const char* name);
```
🗃️ This function is open nvs.

```cpp
esp_err_t get_sta_ssid      (nvs_handle_t *handle, char** ssid);
esp_err_t get_sta_password  (nvs_handle_t *handle, char** password);
esp_err_t get_user_password (nvs_handle_t *handle, char** password);
esp_err_t get_nvs_str       (nvs_handle_t *handle, const char* name, char** value);

esp_err_t get_mac           (nvs_handle_t *handle, uint8_t* mac, char** user_name);
```
🗃️ This function is get nvs data.

```cpp
esp_err_t close_Eaas_nvs    (nvs_handle_t *handle);
```
🗃️ This function is close nvs.

### 🚀 EaaS_network
```cpp
void init_EaaS_network(const char* ssid, const char* password, const uint8_t* mac);
```
🚀 This function is init network.

### 🌐 EaaS_client
```cpp
void init_client(char* user, char* passwd);
```
🌐 This function is init client and save user and password.

```cpp
void set_machine_status(const uint8_t status);
```
🌐 This function is set machine status. <br>
`0`: `offline`, `1`: `booting`, `2`: `boot`, `3`: `pausing`, `4`: `pause`, `5`: `otaing`, `6`: `ota`

```cpp
uint8_t loop_eaas_status();
```
🌐 This function is loop eaas, Get status from server.
