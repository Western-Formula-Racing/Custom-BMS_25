#ifndef INC_TORCH_DIAGNOSTIC_H_
#define INC_TORCH_DIAGNOSTIC_H_

#define PU 0		// Sets the PUP bit to 1 in the ADOW command
#define PD 1		// Sets the PUP bit to 0 in the ADOW command

#define VA_MIN 44000		// Minimum permitted analog supply voltage
#define VA_MAX 56000		// Maximum permitted analog supply voltage

#define VD_MIN 26000		// Minimum permitted digital supply voltage
#define VD_MAX 37000		// Maximum permitted digital supply voltage

#define REF2_MIN 28900		// Minimum permitted 2nd reference voltage
#define REF2_MAX 31140		// Maximum permitted 2nd reference voltage

#define MAX_ADC_DELTA 22	// Maximum permitted measurement difference between LTC6813 internal ADCs

#define OPEN_CELL_THRESHOLD -4000	// -400 mV is the threshold for open cell detection as described on page 32 of the LTC6813 data sheet

#define LTC6813_TEMPERATURE_LIMIT 140.0f

#define THERMISTOR_OPEN_CIRCUIT_THRESHOLD 3.2f

void diagnosis(void);

void ltc6813_analog_supply_check(void);

void ltc6813_digital_supply_check(void);

void ltc6813_reference_check(void);

void ltc6813_temperature_check(void);

void open_cell_check(void);

void open_thermistor_check(void);

void mux_test(void);

void stat_register_test(void);

void cell_register_test(void);

void aux_register_test(void);

void overlap_cell_measurement_test(void);

#endif
