#ifndef INC_TORCH_MAIN_H_
#define INC_TORCH_MAIN_H_

#define ACTIVE 1
#define CHARGE 2

#define FILTER_LEN 10
#define CELL_QTY 20
#define THERM_QTY 18
#define SIDE_A 1
#define SIDE_B 0
#define ATTEMPT_LIMIT 10

// Limits
#define MAX_TEMPERATURE 60
#define MAX_CELL_VOLTAGE 42000
#define MIN_CELL_VOLTAGE 25000

void MX_CAN1_Init(uint8_t mode);

typedef struct BMS_inputs		// Will structify everything after first drive (OR WILL I)
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


extern Inputs bmsInputs;
extern uint8_t bmsState;
extern const uint8_t moduleID;			// Defines which module this is

extern volatile uint32_t transmitCounter;
extern volatile uint32_t measureCounter;
//extern volatile uint32_t canTimeoutCounter;

void torch_main(void);

#endif
