#ifndef INC_TORCH_MAIN_H_
#define INC_TORCH_MAIN_H_

#define FILTER_LEN 10
#define CELL_QTY 20
#define THERM_QTY 18
#define SIDE_A 1
#define SIDE_B 0
#define THERMAL_COUNT_LIM 10

typedef struct BMS_inputs
{
	uint16_t cellVoltages[20];
	uint16_t oldCellVoltages[20];
	float deltaCellVoltages[20];
	
	float moduleTemperatures[18];
	float pcbTemperatures[18];
	
	uint8_t packCurrentCAN[8];
	uint8_t minCellVoltageCAN[8];
	uint8_t doneChargingCAN[8];
	
	uint16_t asicVREF2;
	uint16_t asicVREG;
	uint16_t asicVREGD;
} Inputs;


typedef struct BMS_limits
{
	const uint16_t cellOvervoltage;			// Upper cell voltage limit - 4.2 V
	const uint16_t cellUndervoltage;		// Lower cell voltage limit - 2.5 V
	
	const float maxCellVoltageDelta;		// Cell voltage 
	const float maxCellVoltageSpread;
	
	const uint8_t moduleMaxTemperature;
	const uint8_t pcbMaxTemperature;
} Limits;

extern Inputs bmsInputs;
extern const Limits bmsLimits;
extern uint8_t bmsState;
extern uint8_t thermalCount;

#endif
