#include "mytask.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "semphr.h"

#include "oled.h"
#include "bh1750.h"
#include "dht11.h"
#include "bluetooth.h"

SensorData_t g_sensor_data;
SemaphoreHandle_t g_sensor_mutex;

#define START_TASK_DEPTH 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handler;
void Start_Task(void *param);

#define BH1750_Task_DEPTH 128
#define BH1750_Task_PRIORITY 1
TaskHandle_t BH1750_Task_handler;
void BH1750_Task(void *param);

#define DHT11_Task_DEPTH 128
#define DHT11_Task_PRIORITY 1
TaskHandle_t DHT11_Task_handler;
void DHT11_Task(void *param);

#define BLUETOOTH_Task_DEPTH 128
#define BLUETOOTH_Task_PRIORITY 1
TaskHandle_t BLUETOOTH_Task_handler;
void BLUETOOTH_Task(void *param);

#define OLED_Task_DEPTH 256
#define OLED_Task_PRIORITY 1
TaskHandle_t OLED_Task_handler;
void OLED_Task(void *param);

void FreeRTOS_Start(void)
{
	DHT11_Init();
	BH1750_Init();
	OLED_Init();
	HAL_Delay(1000);

	g_sensor_mutex = xSemaphoreCreateMutex();

	xTaskCreate((TaskFunction_t)Start_Task,
				(char *)"start_task",
				(configSTACK_DEPTH_TYPE)START_TASK_DEPTH,
				NULL,
				(UBaseType_t)START_TASK_PRIORITY,
				(TaskHandle_t *)&start_task_handler);
	vTaskStartScheduler();
}

void Start_Task(void *param)
{

	xTaskCreate((TaskFunction_t)OLED_Task,
				(char *)"OLED_Task",
				(configSTACK_DEPTH_TYPE)OLED_Task_DEPTH,
				NULL,
				(UBaseType_t)OLED_Task_PRIORITY,
				(TaskHandle_t *)&OLED_Task_handler);

	xTaskCreate((TaskFunction_t)BH1750_Task,
				(char *)"BH1750_Task",
				(configSTACK_DEPTH_TYPE)BH1750_Task_DEPTH,
				NULL,
				(UBaseType_t)BH1750_Task_PRIORITY,
				(TaskHandle_t *)&BH1750_Task_handler);

	xTaskCreate((TaskFunction_t)DHT11_Task,
				(char *)"DHT11_Task",
				(configSTACK_DEPTH_TYPE)DHT11_Task_DEPTH,
				NULL,
				(UBaseType_t)DHT11_Task_PRIORITY,
				(TaskHandle_t *)&DHT11_Task_handler);

	xTaskCreate((TaskFunction_t)BLUETOOTH_Task,
				(char *)"BLUETOOTH_Task",
				(configSTACK_DEPTH_TYPE)BLUETOOTH_Task_DEPTH,
				NULL,
				(UBaseType_t)BLUETOOTH_Task_PRIORITY,
				(TaskHandle_t *)&BLUETOOTH_Task_handler);

	vTaskDelete(NULL);
}

void OLED_Task(void *param)
{
	OLED_ShowChinese(32, 0, "环境系统");
	OLED_ShowChinese(0, 16, "当前温度：");
	OLED_ShowChar(91, 16, '.', OLED_8X16);
	OLED_ShowChinese(0, 32, "当前湿度：");
	OLED_ShowChinese(0, 48, "当前光强：");
	OLED_ShowChinese(112, 16, "℃");
	OLED_ShowChar(93, 32, '%', OLED_8X16);
	OLED_ShowString(112, 32, "RH", OLED_8X16);
	OLED_ShowString(102, 48, "lux", OLED_8X16);
	OLED_Update();
	SensorData_t local;
	while (1)
	{
		xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
		local = g_sensor_data;
		xSemaphoreGive(g_sensor_mutex);
		OLED_ShowNum(75, 16, local.temp / 10, 1, OLED_8X16);
		OLED_ShowNum(83, 16, local.temp % 10, 1, OLED_8X16);
		OLED_ShowNum(95, 16, local.temp_0, 1, OLED_8X16);
		OLED_ShowNum(75, 32, local.humi / 10, 1, OLED_8X16);
		OLED_ShowNum(83, 32, local.humi % 10, 1, OLED_8X16);

		OLED_ShowNum(75, 48, local.light, 3, OLED_8X16);

		OLED_Update();

		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

void BH1750_Task(void *param)
{

	while (1)
	{
		uint16_t light = bh_data_read();
		xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
		g_sensor_data.light = light;
		xSemaphoreGive(g_sensor_mutex);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void DHT11_Task(void *param)
{
	uint8_t temp = 0, temp_0 = 0, humi = 0;
	while (1)
	{
		DHT11_Read_Data(&temp, &temp_0, &humi);
		xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
		g_sensor_data.temp = temp;
		g_sensor_data.temp_0 = temp_0;
		g_sensor_data.humi = humi;
		xSemaphoreGive(g_sensor_mutex);
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void BLUETOOTH_Task(void *param)
{
	while (1)
	{
		BlueThooth_Function();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
