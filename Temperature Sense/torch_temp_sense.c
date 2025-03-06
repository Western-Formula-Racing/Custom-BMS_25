#include "main.h"
#include "torch_temp_sense.h"
#include "torch_stm32.h"

void read_thermistors(float *thermistorArray_ptr)
{
	uint16_t thermistorRawADC[18];
	float thermistorVoltage[18];
	uint16_t ADCSum;
	
	//HAL_ADC_Start(&hadc1);
	// ADD MUX ENABLE ON FINAL BOARD
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[0] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[1] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[2] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[3] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i <= 10; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[4] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[5] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i <= 10; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[6] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[7] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[8] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[9] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[10] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[11] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[12] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[13] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i <= 10; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[14] = ADCSum / FILTER_LEN;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[15] = ADCSum / FILTER_LEN;
	
	// TEMPORARY 0 ASSIGNMENT TO LAST 2 THERMISTORS CAUSE I FORGOT TO ADD PULL UP RESISTORS
	thermistorRawADC[16] = 0;
	thermistorRawADC[17] = 0;
	
	// ADD MUX DISABLE ON FINAL BOARD
	//HAL_ADC_Stop(&hadc1);
	
	for(uint8_t i = 0; i < 18; i++) {
		thermistorVoltage[i] = (thermistorRawADC[i] / 4095.0) * 3.3;
	}
	
	for(uint8_t i = 0; i < 18; i++) {
		*(thermistorArray_ptr + i) = thermistorVoltage[i];
	}
}
