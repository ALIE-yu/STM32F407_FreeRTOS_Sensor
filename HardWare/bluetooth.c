#include "stm32f4xx_hal.h" // Device header
#include "bluetooth.h"
#include "mytask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"
void BlueThooth_Function(void)
{
	if (Usart2_RxFlag == 1)
	{
		char *Tag = strtok(Usart2_RxPacket, ",");

		if (strcmp(Tag, "key") == 0) // 判断第一个标志字符是不是key（按键解析）软件选中数据包
		{
			char *Name = strtok(NULL, ","); // name为按键名字
			char *Action = strtok(NULL, ",");

			SensorData_t data;

			// 读取共享数据
			xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);

			data = g_sensor_data;

			xSemaphoreGive(g_sensor_mutex);

			if (strcmp(Name, "温度") == 0 && strcmp(Action, "up") == 0)
			{
				printf("当前温度：%d.%d℃\r\n", data.temp, data.temp_0);
			}

			else if (strcmp(Name, "湿度") == 0 && strcmp(Action, "up") == 0)
			{
				printf("当前湿度：%d%%RH\r\n", data.humi);
			}

			else if (strcmp(Name, "光强") == 0 && strcmp(Action, "up") == 0)
			{
				printf("当前光强：%dLX\r\n", data.light);
			}
		}
		Usart2_RxFlag = 0;
	}
}
