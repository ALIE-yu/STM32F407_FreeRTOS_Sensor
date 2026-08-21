#include "dht11.h"
#include "tim.h"

// 复位DHT11
void DHT11_Rst(void)
{
	DHT11_Mode(OUT); // SET OUTPUT
	DHT11_Low;		 // 拉低DQ
	HAL_Delay(20);	 // 主机拉低18~30ms
	DHT11_High;		 // DQ=1
	delay_us(13);	 // 主机拉高10~35us
}

// 等待DHT11的回应
// 返回1:未检测到DHT11的存在
// 返回0:存在
uint8_t DHT11_Check(void)
{
	uint8_t retry = 0;
	DHT11_Mode(IN);															 // SET INPUT
	while (HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100) // DHT11会拉低40~80us
	{
		retry++;
		delay_us(1);
	};
	if (retry >= 100)
		return 1;
	else
		retry = 0;
	while (!HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100) // DHT11拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	};
	if (retry >= 100)
		return 1;
	return 0;
}

// 从DHT11读取一个位
// 返回值：1/0
uint8_t DHT11_Read_Bit(void)
{
	uint8_t retry = 0;
	while (HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100) // 等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry = 0;
	while (!HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100) // 等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40); // 等待40us
	if (HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN))
		return 1;
	else
		return 0;
}

// 从DHT11读取一个字节
// 返回值：读到的数据
uint8_t DHT11_Read_Byte(void)
{
	uint8_t i, dat;
	dat = 0;
	for (i = 0; i < 8; i++)
	{
		dat <<= 1;
		dat |= DHT11_Read_Bit();
	}
	return dat;
}

// 从DHT11读取一次数据
// temp:温度值(范围:0~50°)
// humi:湿度值(范围:20%~90%)
// 返回值：0,正常;1,读取失败
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *temp_0, uint8_t *humi) //
{
	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if (DHT11_Check() == 0)
	{
		for (i = 0; i < 5; i++) // 读取40位数据
		{
			buf[i] = DHT11_Read_Byte();
		}
		if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) //+buf[1]
		{
			*humi = buf[0];
			*temp = buf[2];
			*temp_0 = buf[3];
		}
	}
	else
		return 1;
	return 0;
}

// 初始化DHT11的IO口 DQ 同时检测DHT11的存在
// 返回1:不存在
// 返回0:存在
uint8_t DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStructure.Pin = DHT11_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
	HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);

	DHT11_Rst();		  // 复位DHT11
	return DHT11_Check(); // 等待DHT11的回应
}

void DHT11_Mode(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if (mode)
	{
		GPIO_InitStructure.Pin = DHT11_GPIO_PIN;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		GPIO_InitStructure.Pin = DHT11_GPIO_PIN;
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	}
	HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}
