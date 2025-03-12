#ifndef INC_TORCH_TEMP_SENSE_H_
#define INC_TORCH_TEMP_SENSE_H_

void read_thermistors(float *thermistorArray_ptr);

void compute_resistance(float *thermistorVoltage_ptr);

void compute_temperature(float *thermistorResistance_ptr);

void temperature_sense(float *temperature_ptr);

#endif
