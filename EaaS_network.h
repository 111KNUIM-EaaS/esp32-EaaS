#ifndef __EAAS_NETWORK_H__
#define __EAAS_NETWORK_H__

#include <esp_err.h>

void init_EaaS_network(const char* ssid, const char* password, const uint8_t* mac);

#endif // __EAAS_NETWORK_H__