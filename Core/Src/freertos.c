/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <MAX7219_GPIO.h>

#include "max7219_SPI.h"
#include "max7219_matrix_SPI.h"

#include "animacion.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern RNG_HandleTypeDef hrng;

const uint32_t redTimeout = 10000;
const uint32_t yellowTimeout = 2000;
const uint32_t greenTimeout = 10000;
const uint32_t updateTrafficFlowTimeout = 400;
const uint32_t displayTrafficLightsTimeout = 1;
const uint32_t displayTrafficFlowTimeout = 1;

typedef enum{
  Red,
  Yellow,
  Green
} lightState;

typedef enum{
  NS,
  SN,
  WE,
  EW
} orientation;

typedef struct{
  orientation ori;
  lightState curState;
  lightState prevState;
} traffic;

typedef struct{
  int pos;
  int timeout;
} carGenerator;

// Shared resources
traffic NST;
traffic EWT;
traffic SNT;
traffic WET;
uint8_t carPositions[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Globals
carGenerator randomCarGenerator;
uint32_t myRandomNum;
/* USER CODE END Variables */
/* Definitions for UpdateTLNSState */
osThreadId_t UpdateTLNSStateHandle;
const osThreadAttr_t UpdateTLNSState_attributes = {
  .name = "UpdateTLNSState",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UpdateTLEWState */
osThreadId_t UpdateTLEWStateHandle;
const osThreadAttr_t UpdateTLEWState_attributes = {
  .name = "UpdateTLEWState",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DisplayTL */
osThreadId_t DisplayTLHandle;
const osThreadAttr_t DisplayTL_attributes = {
  .name = "DisplayTL",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UpdateTF */
osThreadId_t UpdateTFHandle;
const osThreadAttr_t UpdateTF_attributes = {
  .name = "UpdateTF",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DisplayTF */
osThreadId_t DisplayTFHandle;
const osThreadAttr_t DisplayTF_attributes = {
  .name = "DisplayTF",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GenCarsRandomly */
osThreadId_t GenCarsRandomlyHandle;
const osThreadAttr_t GenCarsRandomly_attributes = {
  .name = "GenCarsRandomly",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myMutex01 */
osMutexId_t myMutex01Handle;
const osMutexAttr_t myMutex01_attributes = {
  .name = "myMutex01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void updateTrafficLightState(traffic* inputTrafficLight);
uint32_t getDelayUpdateTrafficLightState(traffic inputTrafficLight);
void displayTrafficLight(traffic inputTrafficLight);
void updateCarOnRoadWhileNotGreen(uint8_t* initialFirstTraffic, uint8_t* initialSecondTraffic);
void updateCarOnRoad(uint8_t* initialFirstTraffic, uint8_t* initialSecondTraffic, traffic inputTrafficLight);
/* USER CODE END FunctionPrototypes */

void StartUpdateTLNSState(void *argument);
void StartUpdateTLEWState(void *argument);
void StartDisplayTL(void *argument);
void StartUpdateTF(void *argument);
void StartDisplayTF(void *argument);
void StartGenCarsRandomly(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  // Init traffic light
  NST.ori = NS;
  NST.prevState = Yellow;
  NST.curState = Green;
  EWT.ori = EW;
  EWT.prevState = Yellow;
  EWT.curState = Red;
  SNT.ori = SN;
  SNT.prevState = Yellow;
  SNT.curState = Green;
  WET.ori = WE;
  WET.prevState = Yellow;
  WET.curState = Red;

  srand(HAL_GetTick()); // Use system time as a simple seed
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of myMutex01 */
  myMutex01Handle = osMutexNew(&myMutex01_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of UpdateTLNSState */
  UpdateTLNSStateHandle = osThreadNew(StartUpdateTLNSState, NULL, &UpdateTLNSState_attributes);

  /* creation of UpdateTLEWState */
  UpdateTLEWStateHandle = osThreadNew(StartUpdateTLEWState, NULL, &UpdateTLEWState_attributes);

  /* creation of DisplayTL */
  DisplayTLHandle = osThreadNew(StartDisplayTL, NULL, &DisplayTL_attributes);

  /* creation of UpdateTF */
  UpdateTFHandle = osThreadNew(StartUpdateTF, NULL, &UpdateTF_attributes);

  /* creation of DisplayTF */
  DisplayTFHandle = osThreadNew(StartDisplayTF, NULL, &DisplayTF_attributes);

  /* creation of GenCarsRandomly */
  GenCarsRandomlyHandle = osThreadNew(StartGenCarsRandomly, NULL, &GenCarsRandomly_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartUpdateTLNSState */
/**
  * @brief  Function implementing the UpdateTLNSState thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartUpdateTLNSState */
void StartUpdateTLNSState(void *argument)
{
  /* USER CODE BEGIN StartUpdateTLNSState */
  /* Infinite loop */
  for(;;)
  {
//    printf("StartUpdateTLNSState b = %d\n", NST.curState);
    updateTrafficLightState(&NST);
    updateTrafficLightState(&SNT);
    uint32_t delayValue = getDelayUpdateTrafficLightState(NST);
//    printf("StartUpdateTLNSState a = %d %lu\n", NST.curState, delayValue);
	osDelay(delayValue);
  }
  /* USER CODE END StartUpdateTLNSState */
}

/* USER CODE BEGIN Header_StartUpdateTLEWState */
/**
* @brief Function implementing the UpdateTLEWState thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUpdateTLEWState */
void StartUpdateTLEWState(void *argument)
{
  /* USER CODE BEGIN StartUpdateTLEWState */
  /* Infinite loop */
  for(;;)
  {
//    printf("StartUpdateTLEWState b = %d\n", EWT.curState);
	updateTrafficLightState(&EWT);
	updateTrafficLightState(&WET);
	uint32_t delayValue = getDelayUpdateTrafficLightState(EWT);
//    printf("StartUpdateTLEWState a = %d %lu\n", EWT.curState, delayValue);
	osDelay(delayValue);

  }
  /* USER CODE END StartUpdateTLEWState */
}

/* USER CODE BEGIN Header_StartDisplayTL */
/**
* @brief Function implementing the DisplayTL thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTL */
void StartDisplayTL(void *argument)
{
  /* USER CODE BEGIN StartDisplayTL */
  /* Infinite loop */
  for(;;)
  {
    displayTrafficLight(NST);
	displayTrafficLight(EWT);
	displayTrafficLight(SNT);
	displayTrafficLight(WET);
	osDelay(displayTrafficLightsTimeout);
  }
  /* USER CODE END StartDisplayTL */
}

/* USER CODE BEGIN Header_StartUpdateTF */
/**
* @brief Function implementing the UpdateTF thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUpdateTF */
void StartUpdateTF(void *argument)
{
  /* USER CODE BEGIN StartUpdateTF */
  /* Infinite loop */
  for(;;)
  {
    if (osMutexAcquire(myMutex01Handle, 0) == osOK) {
//    	printf("StartUpdateTF\n");
    	// Critical section
		uint8_t updatedNorthSouth = ((carPositions[0] & 0x08) << 4) | ((carPositions[1] & 0x08) << 3) | ((carPositions[2] & 0x08) << 2) | ((carPositions[3] & 0x08) << 1) |
	    (carPositions[4] & 0x08) | ((carPositions[5] & 0x08) >> 1) | ((carPositions[6] & 0x08) >> 2) | ((carPositions[7] & 0x08) >> 3);
		uint8_t updatedSouthNorth = ((carPositions[0] & 0x10) << 3) | ((carPositions[1] & 0x10) << 2) | ((carPositions[2] & 0x10) << 1) | (carPositions[3] & 0x10) |
	    ((carPositions[4] & 0x10) >> 1) | ((carPositions[5] & 0x10) >> 2) | ((carPositions[6] & 0x10) >> 3) | ((carPositions[7] & 0x10) >> 4);
		uint8_t updatedWestEast = carPositions[3];
		uint8_t updatedEastWest = carPositions[4];
//		printf("Data NS Before 0x%x SN 0x%x WE 0x%x EW 0x%x\n", updatedNorthSouth, updatedSouthNorth, updatedWestEast, updatedEastWest);

		updateCarOnRoad(&updatedNorthSouth, &updatedSouthNorth, NST);
		updateCarOnRoad(&updatedWestEast, &updatedEastWest, EWT);
//		printf("Data NS After 0x%x SN 0x%x WE 0x%x EW 0x%x\n", updatedNorthSouth, updatedSouthNorth, updatedWestEast, updatedEastWest);

		for (int i = 0; i < 8; i++) {   // update 8 column
			uint8_t movingVertically = (((updatedNorthSouth >> (7-i)) & 1) << 3) | (((updatedSouthNorth >> (7-i)) & 1) << 4);
			if (i == 3) {
			  carPositions[i] = updatedWestEast | movingVertically;
			}
			else if (i == 4) {
			  carPositions[i] = updatedEastWest | movingVertically;
			}
			else {
			  carPositions[i] = movingVertically;
			}
		}
		// Give mutex after critical section
		osMutexRelease(myMutex01Handle);
		osDelay(updateTrafficFlowTimeout);
	}
	else {
//		printf("StartUpdateTF: mutex taken\n");
	}

  }
  /* USER CODE END StartUpdateTF */
}

/* USER CODE BEGIN Header_StartDisplayTF */
/**
* @brief Function implementing the DisplayTF thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTF */
void StartDisplayTF(void *argument)
{
  /* USER CODE BEGIN StartDisplayTF */
  /* Infinite loop */
  for(;;)
  {
    MAX7219_MatrixSetRow(0, carPositions);
	MAX7219_MatrixUpdate();
	osDelay(displayTrafficFlowTimeout);
  }
  /* USER CODE END StartDisplayTF */
}

/* USER CODE BEGIN Header_StartGenCarsRandomly */
/**
* @brief Function implementing the GenCarsRandomly thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGenCarsRandomly */
void StartGenCarsRandomly(void *argument)
{
  /* USER CODE BEGIN StartGenCarsRandomly */
  /* Infinite loop */
  for(;;)
  {
    if (osMutexAcquire(myMutex01Handle, 0) == osOK) {
		HAL_RNG_GenerateRandomNumber(&hrng, &myRandomNum);
		randomCarGenerator.pos = myRandomNum%16;
//		printf("Random Pos %d\n", randomCarGenerator.ori);
		for (int i = 0; i < 4; i++) {
			bool isCarAdded = ((randomCarGenerator.pos >> (3-i)) & 1);
			if (i == 0) {// Add car from north
			  bool isRearEmpty = !((carPositions[0] & 0x08) >> 3);
			  if (isCarAdded && isRearEmpty) {
				carPositions[0] |= 0x08;
//				printf("Added N\n");
			  }
			}
			else if (i == 1) {// Add car from south
			  bool isRearEmpty = !((carPositions[7] & 0x10) >> 4);
			  if (isCarAdded && isRearEmpty) {
				carPositions[7] |= 0x10;
//				printf("Added S\n");
			  }
			}
			else if (i == 2) {// Add car from west
			  bool isRearEmpty = !((carPositions[3] & 0x80) >> 7);
			  if (isCarAdded && isRearEmpty) {
				carPositions[3] |= 0x80;
//				printf("Added W\n");
			  }
			}
			else { // Add car from east
			  bool isRearEmpty = !(carPositions[4] & 0x01);
			  if (isCarAdded && isRearEmpty) {
				carPositions[4] |= 0x01;
//				printf("Added E\n");
			  }
			}
		}
		HAL_RNG_GenerateRandomNumber(&hrng, &myRandomNum);
		randomCarGenerator.timeout = myRandomNum%7000 + 1000;
//		printf("Random Timeout %d\n", randomCarGenerator.timeout);
		// Give mutex after critical section
		osMutexRelease(myMutex01Handle);
		osDelay(randomCarGenerator.timeout);
	}
	else {
//    	printf("StartGenCarsRandomly: mutex taken\n");
	}
  }
  /* USER CODE END StartGenCarsRandomly */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int _write(int file, char *ptr, int len)
{
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    //__io_putchar(*ptr++);
	  ITM_SendChar(*ptr++);
  }
  return len;
}

void updateTrafficLightState(traffic* inputTrafficLight){

  if (inputTrafficLight->curState == Red)
  {
    inputTrafficLight->prevState = inputTrafficLight->curState;
    inputTrafficLight->curState = Yellow;
    printf("updateTrafficLightState Red = %d %d\n", inputTrafficLight->prevState, inputTrafficLight->curState);
  }
  else if (inputTrafficLight->curState == Yellow)
  {
    if (inputTrafficLight->prevState == Red)
    {
      inputTrafficLight->prevState = inputTrafficLight->curState;
      inputTrafficLight->curState = Green;
      printf("updateTrafficLightState Yellow pR = %d %d\n", inputTrafficLight->prevState, inputTrafficLight->curState);
    }
    else
    {
      inputTrafficLight->prevState = inputTrafficLight->curState;
      inputTrafficLight->curState = Red;
      printf("updateTrafficLightState Yellow pG = %d %d\n", inputTrafficLight->prevState, inputTrafficLight->curState);
    }
  }
  else // Green
  {
    inputTrafficLight->prevState = inputTrafficLight->curState;
    inputTrafficLight->curState = Yellow;
    printf("updateTrafficLightState Green = %d %d\n", inputTrafficLight->prevState, inputTrafficLight->curState);
  }
}

uint32_t getDelayUpdateTrafficLightState(traffic inputTrafficLight) {

  uint32_t delayUpdate;
  if (inputTrafficLight.curState == Red) {
    delayUpdate = redTimeout;
  }
  else if (inputTrafficLight.curState == Yellow) {
    delayUpdate = yellowTimeout;
  }
  else { // Green
    delayUpdate = greenTimeout;
  }

  return delayUpdate;
}

void displayTrafficLight(traffic inputTrafficLight) {
  GPIO_TypeDef* R_GPIO_PORT = TL_NS_R_GPIO_Port;
  uint16_t R_PIN = TL_NS_R_Pin;
  GPIO_TypeDef* Y_GPIO_PORT = TL_NS_Y_GPIO_Port;
  uint16_t Y_PIN = TL_NS_Y_Pin;
  GPIO_TypeDef* G_GPIO_PORT = TL_NS_G_GPIO_Port;
  uint16_t G_PIN = TL_NS_G_Pin;
  if (inputTrafficLight.ori == EW) {
	  R_GPIO_PORT = TL_EW_R_GPIO_Port;
	  R_PIN = TL_EW_R_Pin;
	  Y_GPIO_PORT = TL_EW_Y_GPIO_Port;
	  Y_PIN = TL_EW_Y_Pin;
	  G_GPIO_PORT = TL_EW_G_GPIO_Port;
	  G_PIN = TL_EW_G_Pin;
  } else if (inputTrafficLight.ori == SN) {
	  R_GPIO_PORT = TL_SN_R_GPIO_Port;
	  R_PIN = TL_SN_R_Pin;
	  Y_GPIO_PORT = TL_SN_Y_GPIO_Port;
	  Y_PIN = TL_SN_Y_Pin;
	  G_GPIO_PORT = TL_SN_G_GPIO_Port;
	  G_PIN = TL_SN_G_Pin;
  } else if (inputTrafficLight.ori == WE) {
	  R_GPIO_PORT = TL_WE_R_GPIO_Port;
	  R_PIN = TL_WE_R_Pin;
	  Y_GPIO_PORT = TL_WE_Y_GPIO_Port;
	  Y_PIN = TL_WE_Y_Pin;
	  G_GPIO_PORT = TL_WE_G_GPIO_Port;
	  G_PIN = TL_WE_G_Pin;
  }

  if (inputTrafficLight.curState == Red){
    HAL_GPIO_WritePin(R_GPIO_PORT, R_PIN, GPIO_PIN_SET); // LED ON
	HAL_GPIO_WritePin(Y_GPIO_PORT, Y_PIN, GPIO_PIN_RESET); // LED OFF
	HAL_GPIO_WritePin(G_GPIO_PORT, G_PIN, GPIO_PIN_RESET); // LED OFF
  }
  else if (inputTrafficLight.curState == Yellow)
  {
    if (inputTrafficLight.prevState == Red)
    {
    	HAL_GPIO_WritePin(R_GPIO_PORT, R_PIN, GPIO_PIN_SET); // LED ON
    }
    else
    {
      HAL_GPIO_WritePin(R_GPIO_PORT, R_PIN, GPIO_PIN_RESET); // LED OFF
    }
    HAL_GPIO_WritePin(Y_GPIO_PORT, Y_PIN, GPIO_PIN_SET); // LED ON
    HAL_GPIO_WritePin(G_GPIO_PORT, G_PIN, GPIO_PIN_RESET); // LED OFF
  }
  else // Green
  {
    HAL_GPIO_WritePin(R_GPIO_PORT, R_PIN, GPIO_PIN_RESET); // LED OFF
	HAL_GPIO_WritePin(Y_GPIO_PORT, Y_PIN, GPIO_PIN_RESET); // LED OFF
	HAL_GPIO_WritePin(G_GPIO_PORT, G_PIN, GPIO_PIN_SET); // LED ON
  }
}

void updateCarOnRoadWhileNotGreen(uint8_t* initialFirstTraffic, uint8_t* initialSecondTraffic) {
  // First direction
  *initialFirstTraffic >>= 1;
  uint8_t halfRear = (*initialFirstTraffic & 0xF0) >> 4;
  uint8_t halfFront = *initialFirstTraffic & 0x0F;
  if ((halfRear & 1) != 0)
  {
      if ((halfRear & 0x0E) == 0x04)
      {
        halfRear = 0x04 | 0x02;
      }
      else
      {
        halfRear = ((halfRear & 0x0E) << 1) | 0x02;
      }
  }
  *initialFirstTraffic = (halfRear << 4) | halfFront;

  // Second direction
  *initialSecondTraffic <<= 1;
  halfRear = (*initialSecondTraffic & 0xF0) >> 4;
  halfFront = *initialSecondTraffic & 0x0F;
  if ((halfFront & 8) != 0)
  {
      if ((halfFront & 0x07) == 0x02)
      {
        halfFront = 0x04 | 0x02;
      }
      else
      {
        halfFront = 0x04 | ((halfFront & 0x07) >> 1);
      }
  }
  *initialSecondTraffic = (halfRear << 4) | halfFront;
}

void updateCarOnRoad(uint8_t* initialFirstTraffic, uint8_t* initialSecondTraffic, traffic inputTrafficLight) {

  if (inputTrafficLight.curState == Green)
  {
//    printf("updateCarOnRoad Green b 0x%x 0x%x\n", *initialFirstTraffic, *initialSecondTraffic);
    *initialFirstTraffic >>= 1;
    *initialSecondTraffic <<= 1;
//    printf("updateCarOnRoad Green a 0x%x 0x%x\n", *initialFirstTraffic, *initialSecondTraffic);
  }
  else if (inputTrafficLight.curState == Yellow && inputTrafficLight.prevState == Green)
  {
    updateCarOnRoadWhileNotGreen(initialFirstTraffic, initialSecondTraffic);
  }
  else
  {
//    printf("updateCarOnRoad Red b 0x%x 0x%x\n", *initialFirstTraffic, *initialSecondTraffic);
    *initialFirstTraffic &= 0xE7;
    *initialSecondTraffic &= 0xE7;
//    printf("updateCarOnRoad Red a 0x%x 0x%x\n", *initialFirstTraffic, *initialSecondTraffic);
    updateCarOnRoadWhileNotGreen(initialFirstTraffic, initialSecondTraffic);
  }
}
/* USER CODE END Application */

