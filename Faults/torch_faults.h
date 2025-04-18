#ifndef INC_TORCH_FAULTS_H_
#define INC_TORCH_FAULTS_H_

#define ERROR_PEC 61
#define ERROR_OPEN_CIRCUIT 62

void error(uint8_t errorCode);		// All error states

void slow_blink(void);		// Perma blink in 1 s intervals

void fast_blink(void);		// Perma blink in 200 ms intervals

#endif