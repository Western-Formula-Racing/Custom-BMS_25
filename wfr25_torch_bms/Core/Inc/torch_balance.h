#ifndef INC_TORCH_BALANCE_H_
#define INC_TORCH_BALANCE_H_

#define MAX_DELTA 100		// Potential difference limit that determines if a cell needs to be balanced (default 100 = 10 mV)

void force_balance(uint8_t cell);

void manual_balance(void);

void manual_overheat_recover(void);

void manual_emergency_mute(void);

uint8_t balance_check(uint8_t *cellsToBalance, uint16_t *cellVoltages, uint16_t *minVcell);

void balance_cycle(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint16_t minCellVoltage);

void config_DCC_bits(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint8_t *payloadRegisterA, uint8_t DCTO);

void resistor_temp_sense(void);

#endif
