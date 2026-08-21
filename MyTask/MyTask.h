#ifndef __MYTASK_H
#define __MYTASK_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mytask.h"

typedef struct
{
	uint16_t light;
	uint8_t temp;
	uint8_t temp_0;
	uint8_t humi;

} SensorData_t;

extern SensorData_t g_sensor_data;

extern SemaphoreHandle_t g_sensor_mutex;

void FreeRTOS_Start(void);

#endif
