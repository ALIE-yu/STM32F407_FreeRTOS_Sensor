#include "key.h"

uint8_t Key_Scan(void)
{
	uint8_t key_num = 0;
	if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET)
	{
		HAL_Delay(10);
		if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET)
		{
			while (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET)
				;
			key_num = 1;
		}
	}
	if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET)
	{
		HAL_Delay(10);
		if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET)
		{
			while (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET)
				;
			key_num = 2;
		}
	}
	return key_num;
}
