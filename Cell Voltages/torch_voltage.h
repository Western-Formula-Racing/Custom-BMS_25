#ifndef INC_TORCH_VOLTAGE_H_
#define INC_TORCH_VOLTAGE_H_

void voltage_sense(void);			// Reads and stores the 20 cell voltage values

void voltage_derivative(void);		// Calculates the cell voltage rate of change

#endif
