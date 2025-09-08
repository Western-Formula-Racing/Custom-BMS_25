#ifndef INC_TORCH_BALANCE_H_
#define INC_TORCH_BALANCE_H_

#define MAX_DELTA 150					// Potential difference limit that determines if a cell needs to be balanced (default 150 = 15 mV)
#define HOT_LED_THRESHOLD 45			// The required resistor surface temperature to turn on the hot LED (default: 45 degrees C)
#define	RESISTOR_TEMPERATURE_LIMIT 115	// If a resistor gets this hot, then the BMS will halt balancing for that cycle, but it WON'T fault (default: 115 degrees C)

uint8_t balance_check(uint8_t *cellsToBalance, uint16_t *cellVoltages, uint16_t minCellVoltage);

void calculate_duty_cycle(uint8_t *dutyCycles, uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint16_t *cellVoltages);

void extract_min_cell_voltage(uint16_t *absMinCellVoltage);

void balance_cycle(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint16_t absMinCellVoltage);

void config_balance_flags(uint8_t *balanceMsg, uint8_t *cellIndexes, uint8_t cellQty);

void config_DCC_bits(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint8_t *payloadRegisterA, uint8_t DCTO);

void config_DCC_bits(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint8_t *payloadRegisterA, uint8_t DCTO);

void resistor_temperature_sense(float *pcbTemperatures);

#endif
