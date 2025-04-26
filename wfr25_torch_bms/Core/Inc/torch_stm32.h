#ifndef INC_TORCH_STM32_H_
#define INC_TORCH_STM32_H_

extern SPI_HandleTypeDef hspi1;
//extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern ADC_HandleTypeDef hadc1;
extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim2;

extern volatile uint32_t Counter;

void pull_low(GPIO_TypeDef *port,		// GPIO port letter
			  uint16_t pin				// GPIO port number
			  );

void pull_high(GPIO_TypeDef *port,		// GPIO port letter
			   uint16_t pin				// GPIO port number
			   );

void wait(uint32_t msDelay);			// MCU waits for specified duration in milliseconds

uint8_t SPI_transmit(SPI_HandleTypeDef *hspi,			// SPI peripheral (hspi1-3 for TORCH 2025)
					 uint8_t *transmissionData,			// Transmission data array
					 uint8_t transmissionDataLen		// Transmission data length
					 );

uint8_t SPI_receive(SPI_HandleTypeDef *hspi,			// SPI peripheral (hspi1-3 for TORCH 2025)
					uint8_t *receptionData,				// Reception data array
					uint8_t receptionDataLen			// Reception data length
				    );

void start_timer(TIM_HandleTypeDef *htim);			// Timer peripheral (htim2 for TORCH 2025)
void stop_timer(TIM_HandleTypeDef *htim);			// Timer peripheral (htim2 for TORCH 2025)

// TO ADD: ADC (DIFFERS FROM PROTOTYPE B) & CAN

#endif
