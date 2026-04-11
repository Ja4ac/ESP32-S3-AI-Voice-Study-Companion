#ifndef __WIFI_H_
#define __WIFI_H_

#include "freertos/FreeRTOS.h"
#include "project_secrets.h"
#include "esp_err.h"

#include <stdbool.h>

#define WIFI_SSID           PROJECT_WIFI_SSID
#define WIFI_PASSWORD       PROJECT_WIFI_PASSWORD

esp_err_t wifi_sta_init(void);
esp_err_t wifi_sta_connect(TickType_t timeout_ticks);
void wifi_sta_deinit(void);
bool wifi_sta_is_connected(void);
bool wifi_sta_is_initialized(void);

#endif
