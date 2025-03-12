#ifndef INC_TORCH_STM32_H_
#define INC_TORCH_STM32_H_

#define FILTER_LEN 10
#define CELL_QTY 20
#define THERM_QTY 18
#define CELL_R25 30

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

extern volatile uint32_t Counter;
extern uint8_t BMS_state;		// 1 is ACTIVE; 2 is CHARGE; 3 is DEBUG; 0 is ERROR

#endif
