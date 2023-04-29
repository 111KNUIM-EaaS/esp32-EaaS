#ifndef __EAAS_CLIENT_H__
#define __EAAS_CLIENT_H__

#include <esp_err.h>

void init_client(char* user, char* passwd);
void set_machine_status(const uint8_t status);
uint8_t loop_eaas_status();

#endif // __EAAS_CLIENT_H__
