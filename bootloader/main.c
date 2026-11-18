/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "messages.h"
#include "utils/short_hwid.h"
#include "utils/general.h"

//TODO :: FIGURE THE PROPER APP_CODE_START
#define BOOTLOADER_ADDRESS 0x080000000
#define APPLICATION_ADDRESS 0x080080000
#define KILOBYTE_TO_BYTE 1000 
#define FALSE 0
#define TRUE 1


UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static uint8_t rx_message[4];

//TODO :: APPEND GLOBALS WITH g_
// A single frame for reed-solomon
// 255 byes: 223 data bytes + 32 parity bytes
#define FRAME_SIZE 255
#define FRAME_DATA_SIZE 223
#define FRAME_PARITY_SIZE 32

static uint8_t firmware_frame[FRAME_SIZE];
static uint32_t address_offset = 0;
static uint8_t firmware_frame_data_size = FRAME_PARITY_SIZE;

static uint8_t waiting = FALSE;
static uint8_t flash_unlocked = FALSE;




/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void){
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();


  waiting = HAL_UART_Receive(&huart1, rx_message, sizeof(rx_message), 100) == HAL_OK;
  
  // IF we get any error turn on the RED led
  // This can also include Timout exceeding its duration
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, ~waiting);

  // If waiting succeeded we are in write mode
  // So let's blink the white LED 
  while(waiting){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
    HAL_Delay(500);
  }
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

  startApplication();

  // If we got to the end if the first place,
  // Something went wrong, show an error
  Error_Handler();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

  Message message;
  memcpy(&message.raw, rx_message, sizeof(message));

  //Check our CRC 
  int crc = crc8(message.body, sizeof(message.body));

  //We cannot use NodeID here for addressing because bootloader doesnt assign it
  //We will use our HW ID instead
  if(crc != message.bytes[3] && crc ^ ShortHWID() != message.bytes[3]){
    //Our message failed to pass validation, just start the old code
    waiting = FALSE;
    return;
  }

  //We only care about a single message here and that is 30 : Request Firmware Blind Upload
  if(message.id != 30){
    //We got a message that we dont care about in the bootloader
    //Just start the old code
    waiting = FALSE;
    return;
  }

  //We passed our checks lets go into firmware writing mode!!
  flash_unlocked = TRUE;
  HAL_FLASH_Unlock();
  uint32_t startPage = GetPage(APPLICATION_ADDRESS);
  uint32_t total_flash_size = FLASH_BASE * KILOBYTE_TO_BYTE;
  uint32_t endPageAdress = APPLICATION_ADDRESS + total_flash_size + (APPLICATION_ADDRESS - BOOTLOADER_ADDRESS);
  uint32_t endPage = GetPage(endPageAdress);

  FLASH_EraseInitTypeDef EraseInitStruct;
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = startPage;
  EraseInitStruct.NbPages = ((endPage - startPage)/FLASH_PAGE_SIZE) + 1;
  uint32_t PAGEError;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
    /*Error occurred while page erase.*/
    HAL_FLASH_Lock();
    Error_Handler();
  }



  //Lets start listening for the firmware
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, firmware_frame, sizeof(firmware_frame));

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){

  if(Size < FRAME_DATA_SIZE){ // We are done transmitting!!
    waiting = FALSE;
    return;
  }
  // firmware_frame
  uint8_t* corrected = reed_solomon(firmware_frame, FRAME_SIZE);
  uint64_t* packed = pack_64(corrected, FRAME_DATA_SIZE);
  uint32_t packed_size = FRAME_DATA_SIZE / sizeof(uint64_t);
  for(int i = 0; i < packed_size; i++){
    HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, APPLICATION_ADDRESS + address_offset, packed);
    address_offset += sizeof(uint64_t);
  }

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, firmware_frame, sizeof(firmware_frame));
}

void startApplication(){
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);

  HAL_UART_DeInit(&huart1);
  HAL_RCC_DeInit();
  HAL_DeInit();

  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  void (* const jumpFunction)(void) = (void (*)(void))(APPLICATION_ADDRESS + 4ul + 1ul);
  __set_MSP((uint32_t)APPLICATION_ADDRESS);
  jumpFunction();
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200; // Use a more standard Baud
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_RS485Ex_Init(&huart1, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK){
    Error_Handler();
  }
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : PB2 PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
