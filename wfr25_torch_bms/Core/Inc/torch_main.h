#ifndef INC_TORCH_MAIN_H_
#define INC_TORCH_MAIN_H_

// BMS States
#define ACTIVE 1
#define CHARGE 2
#define CHARGE_COMPLETE 3

#define SIDE_A 1
#define SIDE_B 0

#define FILTER_LEN 10

#define CELL_QTY 20
#define THERM_QTY 18		// delete this later
#define MODULE_THERM_QTY 18
#define PCB_THERM_QTY 18
#define ATTEMPT_LIMIT 10

#define CONFIG_ENABLE_BALANCE 0				// Toggles the cell balancing feature. 1 means balancing's enabled, 0 means it's disabled
#define CONFIG_LOCAL_BALANCE 1				// Toggles between using local and global minimum cell voltage as the reference for balancing. 1 means balance with respect to local min, 0 means balance with respect to global min

// Limits
#define MAX_TEMPERATURE 60
#define MAX_CELL_VOLTAGE 42000
#define MIN_CELL_VOLTAGE 25000

// Pack status values
#define PACK_STAT_ACTIVE 3
#define PACK_STAT_CHARGE 4
#define PACK_STAT_FAULT 6

extern const uint8_t bmsMode;			// Sets the operating mode of the BMS
extern const uint8_t moduleID;			// Defines which module this is
extern uint16_t transmissionDelay;		// Delay between message transmission

// Counters
extern volatile uint32_t transmitCounter;
extern volatile uint32_t measureCounter;
extern volatile uint32_t balanceCounter;
extern volatile uint32_t transientCounter;

void torch_main(void);

//void MX_CAN1_Init(uint8_t mode, int8_t moduleID);

#endif
