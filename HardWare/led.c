#include "led.h"

void LED_R_ON(void)
{
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
}
void LED_G_ON(void)
{
	HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
}
void LED_B_ON(void)
{
	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
}
void LED_R_OFF(void)
{
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
}
void LED_G_OFF(void)
{
	HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
}
void LED_B_OFF(void)
{
	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
}

void LED_Turn(void)
{
	HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
}
