#include "main.h"
#include "torch_STM32.h"

volatile uint32_t Counter = 0;

void pull_low(GPIO_TypeDef *port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}


void pull_high(GPIO_TypeDef *port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}


void wait(uint32_t msDelay)
{
	HAL_Delay(msDelay);
}

// Transmits SPI messages on the specified bus
uint8_t SPI_transmit(SPI_HandleTypeDef *hspi, uint8_t *transmissionData, uint8_t transmissionDataLen)
{
	HAL_StatusTypeDef status;		// status indicates whether the transmission was successful or not
	uint8_t attempts = 0;			// transmission attempts (set to 5)
	// Here are the possible values that the function can return:
		// 0 -> Message successfully transmitted
		// 1 -> Error
		
	while(attempts < 5) {
		status = HAL_SPI_Transmit(hspi, transmissionData, transmissionDataLen, HAL_MAX_DELAY);
		
		if(status == HAL_OK) { attempts = 10; }
		
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts == 10) { return 0; }
	
	else { return 1; }
}

// Receives SPI messages on specified bus
uint8_t SPI_receive(SPI_HandleTypeDef *hspi, uint8_t *receptionData, uint8_t receptionDataLen)
{
	HAL_StatusTypeDef status;													// status indicates whether the reception was successful or not
	uint8_t attempts = 0;														// reception attempts (set to 5)
	uint8_t dummies[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};		// dummy bytes to send
	// Here are the possible values that the function can return:
		// 0 -> Message successfully received
		// 1 -> Error
	
	while(attempts < 5) {
		status = HAL_SPI_TransmitReceive(hspi, dummies, receptionData, receptionDataLen, HAL_MAX_DELAY);
		
		if(status == HAL_OK) { attempts = 10; }
		
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts == 10) { return 0; }
	
	else { return 1; }
}


uint8_t start_timer(TIM_HandleTypeDef *htim)
{
	HAL_StatusTypeDef status;										// status indicates whether the reception was successful or not
	uint8_t attempts = 0;											// 5 attempts
	// Here are the possible values that the function can return:
		// 0 -> Timer started successfully
		// 1 -> Error
	
	Counter = 0;
	
	while(attempts < 5) {
		status = HAL_TIM_Base_Start_IT(htim);
		
		if(status == HAL_OK) { attempts = 10; }
		
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts == 10) { return 0; }
	
	else { return 1; }
}


uint8_t stop_timer(TIM_HandleTypeDef *htim)
{
	HAL_StatusTypeDef status;										// status indicates whether the reception was successful or not
	uint8_t attempts = 0;											// 5 attempts
	// Here are the possible values that the function can return:
		// 0 -> Timer stopped successfully
		// 1 -> Error
	
	while(attempts < 5) {
		status = HAL_TIM_Base_Stop_IT(htim);
		
		if(status == HAL_OK) { attempts = 10; }
		
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts == 10) {
		__HAL_TIM_SET_COUNTER(htim, 0);
		Counter = 0;
		return 0;
	}
	
	else { return 1; }
}