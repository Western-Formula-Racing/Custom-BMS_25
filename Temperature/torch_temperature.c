#include "main.h"
#include "torch_main.h"
#include "torch_temperature.h"
#include "torch_LTC6813.h"
#include "torch_STM32.h"
#include <math.h>

void read_thermistors(float *thermistorArray_ptr)		// FOR PROTOTYPE B. MUST BE UPDATED FOR FINAL BOARDS
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
	
	for(uint8_t i = 0; i < 18; i++) {
		thermistorVoltage[i] = (thermistorRawADC[i] / 4095.0) * 3.3;
	}
	
	for(uint8_t i = 0; i < 18; i++) {
		*(thermistorArray_ptr + i) = thermistorVoltage[i];
	}
}


void thermistor_transfer_function(float *thermistorVoltages, uint8_t thermistorQty, float sideA_vref2, float sideB_vref2)
{
	float thermistorResistance[thermistorQty];
	float temperature[thermistorQty];
	
	float VCC = 3.3;		// STM32 VCC (temporarily 3.3 V; check for exact value with multimeter when boards arrive!)
	
	uint16_t Rpu25 = 10000;							// Thermistor pull up resistor AND resistance at 25 C (they're both 10k, so combining to 1 variable)
	float moduleRpa[18] = {0};						// Parasitic resistance (temporarily 0)
	float pcbRpa[18] = {0};							// Parasitic resistance (temporarily 0)
	// A, B, C, and D are constants for thermistor equation
	float A = 0.003354016;
	float B = 0.000256985;
	float C = 0.000002620131;
	float D = 0.00000006383091;

	if(sideA_vref2 > 0 && sideB_vref2 > 0) {		// Condition for checking PCB temperatures
		for(uint8_t i = 0; i < 9; i++) { thermistorResistance[i] = (*(thermistorVoltages + i)*(Rpu25 + pcbRpa[i]))/(sideA_vref2 - *(thermistorVoltages + i)); }
		
		for(uint8_t i = 9; i < 18; i++) { thermistorResistance[i] = (*(thermistorVoltages + i)*(Rpu25 + pcbRpa[i]))/(sideB_vref2 - *(thermistorVoltages + i)); }
		
		for(uint8_t i = 0; i < thermistorQty; i++) {
			bmsInputs.pcbTemperatures[i] = (1/(A +
											   B*logf(thermistorResistance[i]/Rpu25) +
											   C*powf(logf(thermistorResistance[i]/Rpu25), 2) +
											   D*powf(logf(thermistorResistance[i]/Rpu25), 3))) - 273.15;
		}
	}
	else {		// Condition for checking module temperatures
		for(uint8_t i = 0; i < thermistorQty; i++) { thermistorResistance[i] = (*(thermistorVoltages + i)*(Rpu25 + moduleRpa[i]))/(VCC - *(thermistorVoltages + i)); }
		
		for(uint8_t i = 0; i < thermistorQty; i++) {
			bmsInputs.moduleTemperatures[i] = (1/(A +
												  B*logf(thermistorResistance[i]/Rpu25) +
												  C*powf(logf(thermistorResistance[i]/Rpu25), 2) +
												  D*powf(logf(thermistorResistance[i]/Rpu25), 3))) - 273.15;
		}
	}
}


void module_temperature_sense(void)
{
	float moduleThermistorVoltages[18];
	
	read_thermistors(moduleThermistorVoltages);
	
	thermistor_transfer_function(moduleThermistorVoltages, THERM_QTY, 0, 0);
}


void pcb_temperature_sense(void)
{
	LTC6813 asicRegisters = {0};
	float pcbThermistorVoltages[18];
	float sideA_vref2;
	float sideB_vref2;
	
	ADAXD(SIDE_A);
	wait(2);		// consider waiting for 2 ms..
	RDAUXA(&asicRegisters, SIDE_A);
	wait(1);
	RDAUXB(&asicRegisters, SIDE_A);
	wait(1);
	RDAUXC(&asicRegisters, SIDE_A);
	wait(1);
	RDAUXD(&asicRegisters, SIDE_A);
	
	pcbThermistorVoltages[0] = ((asicRegisters.auxRegisterA[1] << 8) | asicRegisters.auxRegisterA[0]) / 10000.0f;
	pcbThermistorVoltages[1] = ((asicRegisters.auxRegisterA[3] << 8) | asicRegisters.auxRegisterA[2]) / 10000.0f;
	pcbThermistorVoltages[2] = ((asicRegisters.auxRegisterA[5] << 8) | asicRegisters.auxRegisterA[4]) / 10000.0f;
	pcbThermistorVoltages[3] = ((asicRegisters.auxRegisterB[1] << 8) | asicRegisters.auxRegisterB[0]) / 10000.0f;
	pcbThermistorVoltages[4] = ((asicRegisters.auxRegisterB[3] << 8) | asicRegisters.auxRegisterB[2]) / 10000.0f;
	pcbThermistorVoltages[5] = ((asicRegisters.auxRegisterC[1] << 8) | asicRegisters.auxRegisterC[0]) / 10000.0f;
	pcbThermistorVoltages[6] = ((asicRegisters.auxRegisterC[3] << 8) | asicRegisters.auxRegisterC[2]) / 10000.0f;
	pcbThermistorVoltages[7] = ((asicRegisters.auxRegisterC[5] << 8) | asicRegisters.auxRegisterC[4]) / 10000.0f;
	pcbThermistorVoltages[8] = ((asicRegisters.auxRegisterD[1] << 8) | asicRegisters.auxRegisterD[0]) / 10000.0f;
	sideA_vref2 = ((asicRegisters.auxRegisterB[5] << 8) | asicRegisters.auxRegisterB[4]) / 10000.0f;
	
	ADAXD(SIDE_B);
	wait(2);		// consider waiting for 2 ms..
	RDAUXA(&asicRegisters, SIDE_B);
	wait(1);
	RDAUXB(&asicRegisters, SIDE_B);
	wait(1);
	RDAUXC(&asicRegisters, SIDE_B);
	wait(1);
	RDAUXD(&asicRegisters, SIDE_B);
	wait(1);
	
	pcbThermistorVoltages[9] = ((asicRegisters.auxRegisterA[1] << 8) | asicRegisters.auxRegisterA[0]) / 10000.0f;
	pcbThermistorVoltages[10] = ((asicRegisters.auxRegisterA[3] << 8) | asicRegisters.auxRegisterA[2]) / 10000.0f;
	pcbThermistorVoltages[11] = ((asicRegisters.auxRegisterA[5] << 8) | asicRegisters.auxRegisterA[4]) / 10000.0f;
	pcbThermistorVoltages[12] = ((asicRegisters.auxRegisterB[1] << 8) | asicRegisters.auxRegisterB[0]) / 10000.0f;
	pcbThermistorVoltages[13] = ((asicRegisters.auxRegisterB[3] << 8) | asicRegisters.auxRegisterB[2]) / 10000.0f;
	pcbThermistorVoltages[14] = ((asicRegisters.auxRegisterC[1] << 8) | asicRegisters.auxRegisterC[0]) / 10000.0f;
	pcbThermistorVoltages[15] = ((asicRegisters.auxRegisterC[3] << 8) | asicRegisters.auxRegisterC[2]) / 10000.0f;
	pcbThermistorVoltages[16] = ((asicRegisters.auxRegisterC[5] << 8) | asicRegisters.auxRegisterC[4]) / 10000.0f;
	pcbThermistorVoltages[17] = ((asicRegisters.auxRegisterD[1] << 8) | asicRegisters.auxRegisterD[0]) / 10000.0f;
	sideB_vref2 = ((asicRegisters.auxRegisterB[5] << 8) | asicRegisters.auxRegisterB[4]) / 10000.0f;
	
	thermistor_transfer_function(pcbThermistorVoltages, THERM_QTY, sideA_vref2, sideB_vref2);
}