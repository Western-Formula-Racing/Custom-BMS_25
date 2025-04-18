#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_states.h"

void led_diagnostics(void)
{
	pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
	pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
	pull_low(GPIOC, GPIO_PIN_8);		// BALANCE LED
	pull_low(GPIOC, GPIO_PIN_7);		// HOT LED
	
	pull_high(GPIOC, GPIO_PIN_4);		// Side A LTC6820 enable
	pull_high(GPIOD, GPIO_PIN_2);		// Side B LTC6820 enable
}


void led_active(void)
{
	pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
	pull_low(GPIOC, GPIO_PIN_9);		// CHARGE LED
}


void led_charge(void)
{
	pull_low(GPIOA, GPIO_PIN_8);		// ACTIVE LED
	pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
}


void led_diagnostic_success(void)
{
	uint8_t flashCount = 0;
	
	while(flashCount < 5) {
		pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
		pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
		wait(100);
		pull_low(GPIOA, GPIO_PIN_8);		// ACTIVE LED
		pull_low(GPIOC, GPIO_PIN_9);		// CHARGE LED
		wait(100);
		flashCount++;
	}
}


void internal_error(void)
{
	pull_low(GPIOC, GPIO_PIN_8);		// BALANCE LED
	pull_low(GPIOC, GPIO_PIN_7);		// HOT LED
	while(1) {
		pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
		pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
		wait(1000);
		pull_low(GPIOA, GPIO_PIN_8);		// ACTIVE LED
		pull_low(GPIOC, GPIO_PIN_9);		// CHARGE LED
		wait(1000);
	}
}


void external_error(void)
{
	pull_low(GPIOC, GPIO_PIN_8);		// BALANCE LED
	pull_low(GPIOC, GPIO_PIN_7);		// HOT LED
	while(1) {
		pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
		pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
		wait(250);
		pull_low(GPIOA, GPIO_PIN_8);		// ACTIVE LED
		pull_low(GPIOC, GPIO_PIN_9);		// CHARGE LED
		wait(250);
	}
}