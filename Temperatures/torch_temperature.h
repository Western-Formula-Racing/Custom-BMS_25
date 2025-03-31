#ifndef INC_TORCH_TEMPERATURE_H_
#define INC_TORCH_TEMPERATURE_H_

void read_thermistors(float *thermistorArray_ptr);		// Digitizes the analog readings of the module thermistors (UPDATE!!!)

void thermistor_transfer_function(float *thermistorVoltages,
								  uint8_t thermistorQty, 
								  float sideA_vref2,
								  float sideB_vref2);

void module_temperature_sense(void);

void pcb_temperature_sense(void);

#endif
