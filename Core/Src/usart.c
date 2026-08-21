/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
uint8_t Usart2_RxData;
char Usart2_RxPacket[50];
uint8_t Usart2_RxFlag = 0;
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000);

	return (ch);
}
/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (uartHandle->Instance == USART2)
	{
		/* USER CODE BEGIN USART2_MspInit 0 */

		/* USER CODE END USART2_MspInit 0 */
		/* USART2 clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART2 interrupt Init */
		HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		/* USER CODE BEGIN USART2_MspInit 1 */

		/* USER CODE END USART2_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

	if (uartHandle->Instance == USART2)
	{
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);

		/* USART2 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		/* USER CODE BEGIN USART2_MspDeInit 1 */

		/* USER CODE END USART2_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */
void USART2_ReceiveStart(void)
{
	HAL_UART_Receive_IT(&huart2, &Usart2_RxData, 1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;

	if (huart->Instance == USART2)
	{
		if (RxState == 0)
		{
			if (Usart2_RxData == '[' && Usart2_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (Usart2_RxData == ']')
			{
				RxState = 0;
				Usart2_RxPacket[pRxPacket] = '\0';
				Usart2_RxFlag = 1;
			}
			else
			{
				Usart2_RxPacket[pRxPacket] = Usart2_RxData;
				pRxPacket++;
				if (pRxPacket >= 49)
				{
					pRxPacket = 0;
					RxState = 0;
				}
			}
		}
		USART2_ReceiveStart();
	}
}
/* USER CODE END 1 */
