#ifndef INC_TORCH_MAIN_H_
#define INC_TORCH_MAIN_H_

#define FILTER_LEN 10
#define CELL_QTY 20
#define THERM_QTY 18
#define SIDE_A 1
#define SIDE_B 0

typedef struct BMS_inputs
{
	uint16_t cellVoltages[20];
	uint16_t priorCellVoltages[20];
	uint16_t deltaCellVoltages[20];
	
	float moduleTemperatures[18];
	float pcbTemperatures[18];
	
	uint16_t asicVREF2;
	uint16_t asicVREG;
	uint16_t asicVREGD;
	uint16_t asicITMP;
} Inputs;

extern Inputs bmsInputs;
extern uint8_t bms_state;		// 1 is ACTIVE; 2 is CHARGE; 3 is DEBUG; 0 is ERROR

#endif
