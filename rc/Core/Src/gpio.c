/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SI24R1_CSN_GPIO_Port, SI24R1_CSN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SI24R1_CE_GPIO_Port, SI24R1_CE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : KEY_LEFT_X_Pin KEY_RIGHT_X_Pin KEY_UP_Pin KEY_RIGHT_Pin
                           KEY_LEFT_Pin KEY_DOWN_Pin */
  GPIO_InitStruct.Pin = KEY_LEFT_X_Pin|KEY_RIGHT_X_Pin|KEY_UP_Pin|KEY_RIGHT_Pin
                          |KEY_LEFT_Pin|KEY_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IP5305T_KEY_Pin */
  GPIO_InitStruct.Pin = IP5305T_KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(IP5305T_KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SI24R1_CSN_Pin */
  GPIO_InitStruct.Pin = SI24R1_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SI24R1_CSN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SI24R1_CE_Pin */
  GPIO_InitStruct.Pin = SI24R1_CE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SI24R1_CE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SI24R1_IRQ_Pin (PB6, 手动补配)
    原理图 U6(Si24R1) 第6脚 IRQ → PB6，推挽输出+板上10k上拉，故配浮空输入
    注：此段在 USER CODE 区外，CubeMX 重新生成时会被覆盖——但 rc.ioc 已同步补配 PB6，
    重新生成后会自动产生等价的初始化代码，功能不丢 */
  GPIO_InitStruct.Pin = SI24R1_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SI24R1_IRQ_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
