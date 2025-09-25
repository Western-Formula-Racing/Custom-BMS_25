#include "main.h"
#include "torch_main.h"
#include "torch_temperature.h"
#include "torch_stm32.h"
#include <math.h>


void compute_resistance(float *thermistorVoltage_ptr)
{
	float thermistorResistance[MODULE_THERM_QTY];
	uint16_t Rpu = 10000;			// Thermistor pull up resistor
	float Rmw = 0;					// Trace resistance on module board
	float Rmc = 0.03;				// Module board connector resistance
	float Rec = 0.03;				// Embedded board connector resistance
	float Rew = 0;					// Trace resistance on embedded board
	float Req = Rpu + Rmc + Rec;	// Equivalent resistance

	for(uint8_t i = 0; i < 18; i++) { thermistorResistance[i] = (*(thermistorVoltage_ptr + i)*Req)/(3.3 - *(thermistorVoltage_ptr + i)); }

	for(uint8_t i = 0; i < 18; i++) { *(thermistorVoltage_ptr + i) = thermistorResistance[i]; }
}


void compute_temperature(float *thermistorResistance_ptr)
{
	float temperature[MODULE_THERM_QTY];
	float A = 0.003354016;
	float B = 0.000256985;
	float C = 0.000002620131;
	float D = 0.00000006383091;
	uint16_t R25 = 10000;


	for(uint8_t i = 0; i < 18; i++) {
		temperature[i] = (1/(A +
							 B*logf(*(thermistorResistance_ptr + i)/R25) +
							 C*powf(logf(*(thermistorResistance_ptr + i)/R25), 2) +
							 D*powf(logf(*(thermistorResistance_ptr + i)/R25), 3))) - 273.15;
	}

	for(uint8_t i = 0; i < 18; i++) { *(thermistorResistance_ptr + i) = temperature[i]; }
}


void temperature_sense(float *temperature_ptr)
{
	float thermistorArray[MODULE_THERM_QTY];

	read_thermistors(thermistorArray);

	compute_resistance(thermistorArray);

	compute_temperature(thermistorArray);

	for(uint8_t i = 0; i < 18; i++) { *(temperature_ptr + i) = thermistorArray[i]; }
}


void board_temperature_sense(float *boardThermistorVoltages, float VREF2, float *boardTemperatures)
{
	float thermistorResistance[9];
	uint16_t Rpu = 10000;

	float A = 0.003354016;
	float B = 0.000256985;
	float C = 0.000002620131;
	float D = 0.00000006383091;
	uint16_t R25 = 10000;

	for(uint8_t i = 0; i < 9; i++) {
		thermistorResistance[i] = (*(boardThermistorVoltages + i)*Rpu)/(VREF2 - *(boardThermistorVoltages + i));
	}

	for(uint8_t i = 0; i < 9; i++) {
		*(boardTemperatures + i) = (1/(A +
									   B*logf(thermistorResistance[i]/R25) +
									   C*powf(logf(thermistorResistance[i]/R25), 2) +
									   D*powf(logf(thermistorResistance[i]/R25), 3))) - 273.15;
	}
}


void read_thermistors(float *thermistorArray_ptr)
{
	uint16_t thermistorRawADC[18];
	float thermistorVoltage[18];
	uint16_t ADCSum;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[0] = ADCSum / FILTER_LEN;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[1] = ADCSum / FILTER_LEN;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[2] = ADCSum / FILTER_LEN;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[3] = ADCSum / FILTER_LEN;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i <= 10; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[4] = (ADCSum / FILTER_LEN) - 200;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[5] = ADCSum / FILTER_LEN;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i <= 10; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[6] = (ADCSum / FILTER_LEN) - 200;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[7] = ADCSum / FILTER_LEN;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[8] = ADCSum / FILTER_LEN;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[9] = ADCSum / FILTER_LEN;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[10] = ADCSum / FILTER_LEN;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[11] = ADCSum / FILTER_LEN;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[12] = ADCSum / FILTER_LEN;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_low(GPIOA, GPIO_PIN_2);		// U10 B
	pull_low(GPIOC, GPIO_PIN_13);		// U10 C
	pull_low(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[13] = ADCSum / FILTER_LEN;

	pull_low(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i <= 10; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[14] = (ADCSum / FILTER_LEN) - 200;
	thermistorRawADC[15] = thermistorRawADC[13] + 22;

	pull_high(GPIOA, GPIO_PIN_3);		// U10 A
	pull_high(GPIOA, GPIO_PIN_2);		// U10 B
	pull_high(GPIOC, GPIO_PIN_13);		// U10 C
	pull_high(GPIOC, GPIO_PIN_14);		// U10 D
	wait(1);
	ADCSum = 0;
	for (uint8_t i = 0; i < FILTER_LEN; i++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCSum += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}
	thermistorRawADC[16] = ADCSum / FILTER_LEN;
	thermistorRawADC[17] = thermistorRawADC[12] + 33;

	for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) {
		thermistorVoltage[i] = (thermistorRawADC[i] / 4095.0) * 3.3;
	}
	for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) {
		*(thermistorArray_ptr + i) = thermistorVoltage[i];
	}
}
