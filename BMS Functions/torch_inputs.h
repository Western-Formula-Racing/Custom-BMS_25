#ifndef INC_TORCH_LTC6813_COMMS_H_
#define INC_TORCH_LTC6813_COMMS_H_

void voltage_sense(uint16_t *cellVoltages);

void force_refup(void);

void balance_cells(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty);

void balance_heat_test(void); // test dummy function

#endif
