#ifndef INC_TORCH_BALANCE_H_
#define INC_TORCH_BALANCE_H_

#define MAX_DELTA 100		// Potential difference limit that determines if a cell needs to be balanced (default 100 = 10 mV)

void force_balance(uint8_t cell);

void cell_sorter(uint8_t *cellsToBalance, uint16_t *cellVoltages);

void balance_loop(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty);

void resistor_temp_sense(void);

#endif
