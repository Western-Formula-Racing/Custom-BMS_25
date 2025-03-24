#include "main.h"
#include "torch_LTC6813.h"
#include "torch_main.h"
#include "torch_STM32.h"
#include "torch_faults.h"

void error(uint8_t errorCode)
{
	switch(errorCode) {
		case ERROR_PEC:
			// send 'PEC failed >3 times' error CAN msg
			slow_blink();
			break;
		default:
			// send 'undefined' error CAN msg
			slow_blink();
			break;
	}
}


void slow_blink(void)
{
	while(1) {
		pull_high(GPIOA, GPIO_PIN_8);
		pull_high(GPIOC, GPIO_PIN_9);
		HAL_Delay(1000);
		pull_low(GPIOA, GPIO_PIN_8);
		pull_low(GPIOC, GPIO_PIN_9);
		HAL_Delay(1000);
	}
}


void fast_blink(void)
{
	while(1) {
		pull_high(GPIOA, GPIO_PIN_8);
		pull_high(GPIOC, GPIO_PIN_9);
		HAL_Delay(200);
		pull_low(GPIOA, GPIO_PIN_8);
		pull_low(GPIOC, GPIO_PIN_9);
		HAL_Delay(200);
	}
}