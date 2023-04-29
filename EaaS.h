#ifndef __EaaS_H__
#define __EaaS_H__

#include <Arduino.h>

#ifdef EAAS_DEBUG
#define EAAS_LOG(tag, fmt, ...) ESP_LOGI("[%s] " fmt "\n", tag, ##__VA_ARGS__)

#define EAAS_LOGI(tag, fmt, ...) ESP_LOGI("[%s] " fmt "\n", tag, ##__VA_ARGS__)
#define EAAS_LOGW(tag, fmt, ...) ESP_LOGW("[%s] " fmt "\n", tag, ##__VA_ARGS__)
#define EAAS_LOGE(tag, fmt, ...) ESP_LOGE("[%s] " fmt "\n", tag, ##__VA_ARGS__)
#else
#define EAAS_LOG(tag, fmt, ...) ;
#endif

void init_EaaS(void);
void loop_eaas(void *parameter);
void EaaS_OTA();

#endif // __EaaS_H__
