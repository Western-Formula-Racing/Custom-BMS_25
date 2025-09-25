#ifndef INC_TORCH_STM32_H_
#define INC_TORCH_STM32_H_

// STM32 peripheral variables
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;
extern ADC_HandleTypeDef hadc1;
extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim2;

// Forces both LTC6820s to be enabled
void ltc6820_awaken(void);

// BMS enters a low power state, consuming minimal current from the battery pack
void low_power_state(void);

// Permanent blink with a 1 second latency
void blink(void);

// Permanent blink with a 300 millisecond latency
void fast_blink(void);

// Pulls the specified STM32 GPIO pin to 0 V
void pull_low(GPIO_TypeDef *port,		// GPIO port letter
			  uint16_t pin				// GPIO port number
			  );

// Pulls the specified STM32 GPIO pin to 3.3 V
void pull_high(GPIO_TypeDef *port,		// GPIO port letter
			   uint16_t pin				// GPIO port number
			   );

// Makes the STM32 wait for a specified duration
void wait(uint32_t msDelay);			// MCU waits for specified duration in milliseconds

// Transmits an SPI message through the STM32's SPI peripheral
uint8_t SPI_transmit(SPI_HandleTypeDef *hspi,			// SPI peripheral (hspi1-3 for TORCH 2025)
					 uint8_t *transmissionData,			// Transmission data array
					 uint8_t transmissionDataLen		// Transmission data length
					 );

// Receives an SPI message on the STM32's SPI peripheral
uint8_t SPI_receive(SPI_HandleTypeDef *hspi,			// SPI peripheral (hspi1-3 for TORCH 2025)
					uint8_t *receptionData,				// Reception data array
					uint8_t receptionDataLen			// Reception data length
				    );

// Starts the STM32's internal timer
void start_timer(TIM_HandleTypeDef *htim);			// Timer peripheral (htim2 for TORCH 2025)

// Stops the STM32's internal timer
void stop_timer(TIM_HandleTypeDef *htim);			// Timer peripheral (htim2 for TORCH 2025)


// The eight functions below turn on/off the interface PCB LEDs
void active_led_on(void);

void active_led_off(void);

void charge_led_on(void);

void charge_led_off(void);

void balance_led_on(void);

void balance_led_off(void);

void hot_led_on(void);

void hot_led_off(void);

#endif
