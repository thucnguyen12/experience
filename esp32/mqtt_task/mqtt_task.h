#ifndef __MQTT_TASK_H
#define __MQTT_TASK_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "http_request.h"
#include "cJSON.h"
#include "test_info.h"
#include "mqtt_client.h"
void mqtt_manager_task (void);
void creat_rtos_resource (void);
#endif