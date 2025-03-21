#include "main.h"
#include "torch_STM32.h"

void pull_low(GPIO_TypeDef *port_ptr, uint16_t pin) { HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET); }

void pull_high(GPIO_TypeDef *port_ptr, uint16_t pin) { HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); }

// Transmits SPI messages on the specified bus
uint8_t SPI_transmit(SPI_HandleTypeDef *hspi, uint8_t *data_ptr, uint8_t dataLen)
{
	HAL_StatusTypeDef status;		// status indicates whether the transmission was successful or not
	uint8_t attempts = 0;			// transmission attempts (set to 3)
	// Here are the possible values that the function can return:
		// 0 -> Message successfully transmitted
		// 1 -> Error
		
	while(attempts < 3) {
		status = HAL_SPI_Transmit(hspi, data_ptr, dataLen, HAL_MAX_DELAY);
		
		if(status == HAL_OK) { attempts = 10; }
		
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts == 10) { return 0; }
	
	else { return 1; }
}