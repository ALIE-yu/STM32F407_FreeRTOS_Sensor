#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f4xx_hal.h" // Device header
#include "tim.h"

// DHT11引脚宏定义
#define DHT11_GPIO_PORT GPIOC
#define DHT11_GPIO_PIN GPIO_PIN_4
/*********************END**********************/

// 输出状态定义
#define OUT 1
#define IN 0

// 控制DHT11引脚输出高低电平
#define DHT11_Low HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_RESET)
#define DHT11_High HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET)

uint8_t DHT11_Init(void); // 初始化DHT11
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *temp_0, uint8_t *humi);
uint8_t DHT11_Read_Byte(void); // 读取一个字节的数据
uint8_t DHT11_Read_Bit(void);  // 读取一位的数据
void DHT11_Mode(uint8_t mode); // DHT11引脚输出模式控制
uint8_t DHT11_Check(void);	   // 检测DHT11
void DHT11_Rst(void);		   // 复位DHT11

#endif
