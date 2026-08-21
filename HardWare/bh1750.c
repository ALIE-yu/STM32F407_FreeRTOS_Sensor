#include "bh1750.h"
void bh_data_send(uint8_t command)
{
	HAL_I2C_Master_Transmit(&hi2c1, BHAddWrite, &command, 1, 1000);
}

uint16_t bh_data_read(void)
{
	uint8_t data[2];
	uint16_t light_value;
	HAL_I2C_Master_Receive(&hi2c1, BHAddRead, data, 2, 1000);
	light_value = ((uint16_t)data[0] << 8) | data[1];
	light_value = light_value / 1.2;
	return light_value;
}

void BH1750_Init(void)
{
	bh_data_send(BHPowOn);	// BH1750上电
	bh_data_send(BHReset);	// BH1750复位
	bh_data_send(BHModeH2); // BH1750写入测量模式
	HAL_Delay(200);
}
