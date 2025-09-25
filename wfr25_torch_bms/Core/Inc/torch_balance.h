#ifndef INC_TORCH_BALANCE_H_
#define INC_TORCH_BALANCE_H_

// Limits
#define MAX_DELTA 150						// Potential difference limit that determines if a cell needs to be balanced (default 150 = 15 mV)
#define HOT_LED_THRESHOLD 45				// The required resistor surface temperature to turn on the hot LED (default: 45 degrees C)
#define	RESISTOR_TEMPERATURE_LIMIT 115		// If a resistor gets this hot, then the BMS will halt balancing for that cycle (default: 115 degrees C)

// Determines which cells are out of balance and returns the quantity
uint8_t balance_check(uint8_t *cellsToBalance,		// Pointer to array holding the indexes of the cells that need to be balanced
					  uint16_t *cellVoltages,		// Pointer to array holding the cell voltages
					  uint16_t minCellVoltage		// The minimum given cell voltage
					 );

// Extracts the minimum cell voltage in the entire pack. This function is only meant to be called by module 1 when force balance is enabled
void extract_min_cell_voltage(uint16_t *absMinCellVoltage		// Pointer to an integer holding the lowest cell voltage in the entire battery pack
							 );

// The cycle where the BMS balances even cells, odd cells, and waits for the cell voltages to settle
void balance_cycle(uint8_t *cellsToBalance,			// Pointer to array holding the indexes of the cells that need to be balanced
				   uint8_t cellsToBalanceQty,		// Quantity of cells that need to be balanced
				   uint16_t absMinCellVoltage,		// The lowest cell voltage in the entire battery pack
				   uint8_t *haltBalanceFlag			// Pointer to flag that's used for stopping balancing upon command
				  );

// Configures the cell balancing flags for the TORCH_Mx_BALANCE CAN messages
void config_balance_flags(uint8_t *balanceMsg,		// Pointer to the balance CAN message
						  uint8_t *cellIndexes,		// Pointer to the cell indexes that need to be balanced
						  uint8_t cellQty			// Quantity of cells that need their balance flags updated
						 );

// Configures the discharge control (DCC) bits in the LTC6813
void config_DCC_bits(uint8_t *cellsToBalance,		// Pointer to array holding the indexes of the cells that need to be balanced
					 uint8_t cellsToBalanceQty,		// Quantity of cells that need to be balanced
					 uint8_t *payloadRegisterA,		// Pointer to array holding the LTC6813's Configuration Register Group A
					 uint8_t DCTO					// LTC6813 discharge timer monitor
					);

// Configures the pulse-width modulation bits in the LTC6813 used for controlling the balancing current
void config_PWM_bits(uint8_t *cellsToBalance,		// Pointer to array holding the indexes of the cells that need to be balanced
					 uint8_t cellsToBalanceQty,		// Quantity of cells that need to be balanced
					 uint8_t *payloadRegisterPWM,	// Pointer to array holding the LTC6813's PWM Register Group
					 uint16_t *cellVoltages,		// Pointer to array holding the cell voltages
					 uint8_t side					// Side of which to write the PWM bits (SIDE_A or SIDE_B)
					);

// Measures the temperatures of the balancing resistors
void resistor_temperature_sense(float *pcbTemperatures		// Pointer to array holding the balance resistor temperatures
							   );

#endif
