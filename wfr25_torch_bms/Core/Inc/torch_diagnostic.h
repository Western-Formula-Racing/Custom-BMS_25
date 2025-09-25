#ifndef INC_TORCH_DIAGNOSTIC_H_
#define INC_TORCH_DIAGNOSTIC_H_

#define PU 0						// Sets the PUP bit to 1 in the ADOW command (default: 0)
#define PD 1						// Sets the PUP bit to 0 in the ADOW command (default: 1)
#define ADOW_CONVERSION_RUNS 3		// The quantity of conversions done during the open cell check command (default: 4 conversions)

#define VA_MIN 44000		// Minimum permitted analog supply voltage (default: 44000 = 4.4 V)
#define VA_MAX 56000		// Maximum permitted analog supply voltage (default: 56000 = 5.6 V)

#define VD_MIN 26000		// Minimum permitted digital supply voltage (default: 26000 = 2.6 V)
#define VD_MAX 37000		// Maximum permitted digital supply voltage (default: 37000 = 3.7 V)

#define REF2_MIN 28900		// Minimum permitted 2nd reference voltage (default: 28900 = 2.89 V)
#define REF2_MAX 31140		// Maximum permitted 2nd reference voltage (default: 31140 = 3.114 V)

#define MAX_ADC_DELTA 30	// Maximum permitted measurement difference between LTC6813 internal ADCs (default: 30 = 3 mV)

#define OPEN_CELL_THRESHOLD -4000					// Threshold for open cell detection as described on page 32 of the LTC6813 data sheet (default: -4000 = -400 mV)

#define LTC6813_TEMPERATURE_LIMIT 140.0f			// LTC6813 temperature limit (default: 140.0f = 140 C)

#define THERMISTOR_OPEN_CIRCUIT_THRESHOLD 3.2f		// Voltage reading threshold for thermistor open circuit detection (default: 3.2f = 3.2 V)

// Performs the full self-diagnosis
void full_diagnosis(void);

// Performs a shortened self-diagnosis
void quick_diagnosis(uint8_t packStatus);

// Measures and checks that the LTC6813's analog supply voltage is within a normal range
void ltc6813_analog_supply_check(void);

// Measures and checks that the LTC6813's digital supply voltage is within a normal range
void ltc6813_digital_supply_check(void);

// Measures and checks that the LTC6813's second reference voltage is within a normal range
void ltc6813_reference_check(void);

// Measures and checks the LTC6813's internal temperature
void ltc6813_temperature_check(void);

// Checks if there are open circuits on the cell voltage connections
void open_cell_check(void);

// Checks if there are open circuits on the module thermistor connections
void open_thermistor_check(void);

// Checks if the LTC6813's internal multiplexer is functioning properly
void mux_test(void);

// Checks if the LTC6813's status register group is functioning properly
void stat_register_test(void);

// Checks if the LTC6813's cell register group is functioning properly
void cell_register_test(void);

// Checks if the LTC6813's auxiliary register group is functioning properly
void aux_register_test(void);

// Checks if the LTC6813's internal ADCs are functioning properly
void overlap_cell_measurement_test(void);

#endif
