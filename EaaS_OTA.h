#ifndef __EAAS_OTA_H__
#define __EAAS_OTA_H__

#include <Arduino.h>

void init_EaaS_OTA(char *user, char *passwd);
int flash_EaaS_firmware();

#endif // __EAAS_OTA_H__
