#include <Arduino.h>
#include <SimpleFOC.h>
#include "dma.h"
#include "usart.h"
#include "messages.h"
#include "string.h"


//PWM Pins (TIM1 or IN1, IN2, IN3)
const PinName driverPins[] = {
    PB_13, //TIM1_1N
    PB_14, //TIM1_2N
    PB_15, //TIM1_3N
    PA_8 //EN
};

const PinName mt6701_spi_pins[] = {
    PA_6,  // MISO
    PA_5,  // SCK
};

BLDCDriver3PWM driver = BLDCDriver3PWM(driverPins[0], driverPins[1], driverPins[2], driverPins[3]);

// MagneticSensorSPI(int cs, float _cpr, int _angle_register, long _clock_speed)
//  cs              - SPI chip select pin 
//  bit_resolution - magnetic sensor resolution
//  angle_register  - (optional) angle read register - default 0x3FFF
//  clock_speed      - (optional) SPI clock speed - default 1MHz
//MagneticSensorSPI sensor = MagneticSensorSPI(mt6701_spi_pins[0], mt6701_spi_pins[1]);

//  BLDCMotor(int pp, (optional R, KV, Lq, Ld))
//  - pp  - pole pair number
//  - R   - phase resistance value [Ohm] - optional
//  - KV  - motor KV rating [rpm/V] - optional
BLDCMotor motor = BLDCMotor(7, 11.2, 140);

void setup() {
  MX_DMA_Init();
  MX_USART1_UART_Init();

  pinMode(PB_2, OUTPUT);
  pinMode(PB_10, OUTPUT);
  pinMode(PB_11, OUTPUT);
  digitalWrite(PB_11, LOW); //Turn on termination 



  digitalWrite(PB_2, LOW); //Turn on RED led
  digitalWrite(PB_10, HIGH); //Turn off WHITE led 
  delay(1000);


  // huart1.pRxBuffPtr = new uint8_t[4];
  // HAL_UART_Receive_DMA(&huart1, huart1.pRxBuffPtr, sizeof(Message));
  digitalWrite(PB_2, HIGH); //Turn off RED led
  digitalWrite(PB_10, LOW); //Turn on WHITE led 
  delay(1000);

  //sensor.spi_mode = SPI_MODE2;
  //sensor.clock_speed = 1000000;
  //sensor.init();

  driver.voltage_power_supply = 12;
  driver.voltage_limit = 12;
  driver.init();

  //motor.linkSensor(&sensor);
  motor.linkDriver(&driver);

  motor.controller =  MotionControlType::velocity_openloop;
  motor.voltage_limit = 12;

  motor.init();
  motor.initFOC();
  huart1.pRxBuffPtr = new uint8_t[4];
}

uint8_t g_n = 0;
uint8_t g_test[] = "This is a test";
void loop() {

  //sensor.update();
  motor.loopFOC();
  //volatile HAL_StatusTypeDef s;
  //s = HAL_UART_Receive(&huart1,huart1.pRxBuffPtr, sizeof(Message), 20000);

  Message m = Request_Hardware_Id_Handler((Message){.raw=0xffffffff});
  HAL_UART_Transmit(&huart1,reinterpret_cast<uint8_t*>(&m), sizeof(m), 25); 
  //HAL_UART_Transmit(&huart1, &g_n, 1, 25); 
  //  g_n--;
  // motor.move(10.0);
}

#ifdef __cplusplus
extern "C" {
#endif

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

const PinName digitalPin[] = {
    PA_0,  PA_1,  PA_2,  PA_3,  PA_4,  PA_5,  PA_6,  PA_7,
    PA_8,  PA_9,  PA_10, PA_11, PA_12, PA_13, PA_14, PA_15,
    PB_0,  PB_1,  PB_2,  PB_3,  PB_4,  PB_5,  PB_6,  PB_7,
    PB_8,  PB_9,  PB_10, PB_11, PB_12, PB_13, PB_14, PB_15,
    PC_13, PC_14, PC_15, PF_0,  PF_1
};

const uint32_t analogInputPin[] = {
    PA_0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7,
    PB_0, PB_1
};

#ifdef __cplusplus
}
#endif