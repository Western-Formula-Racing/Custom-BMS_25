#ifndef INC_TORCH_TEMPERATURE_H_
#define INC_TORCH_TEMPERATURE_H_

#define FILTER_LEN 10		// The length of the moving average filter used during AD conversions

// Reads the voltage drop across the thermistors
void read_thermistors(float *thermistorArray_ptr		// Pointer to array holding thermistor voltages
					 );

// Computes the resistance of the thermistors
void compute_resistance(float *thermistorVoltage_ptr	// Pointer to array holding thermistor voltages
					   );

// Computes the temperature in degrees Celcius of the thermistors
void compute_temperature(float *thermistorResistance_ptr		// Pointer to array holding thermistor resistances
						);

// Measures the module temperatures
void temperature_sense(float *temperature_ptr		// Pointer to array holding module temperatures
					  );

// Measures the interface PCB temperatures
void board_temperature_sense(float *boardThermistorVoltages,		// Pointer to array holding the thermistor voltages
							 float VREF2,							// The second reference voltage of the LTC6813
							 float *boardTemperatures				// Pointer to array holding the balance resistor temperatures
							);

#endif
