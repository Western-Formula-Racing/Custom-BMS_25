#ifndef INC_TORCH_TEMPERATURE_H_
#define INC_TORCH_TEMPERATURE_H_

void read_thermistors(float *thermistorArray_ptr);

void compute_resistance(float *thermistorVoltage_ptr);

void compute_temperature(float *thermistorResistance_ptr);

void temperature_sense(float *temperature_ptr);

void board_temperature_sense(float *boardThermistorVoltages, float VREF2, float *boardTemperatures);

#endif /* INC_TORCH_TEMPERATURE_H_ */
