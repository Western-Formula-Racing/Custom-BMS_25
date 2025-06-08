#ifndef INC_TORCH_MAIN_H_
#define INC_TORCH_MAIN_H_

#define MODULE_ID_FLASH_ADDRESS ((uint32_t)0x080E0000)

// BMS States
#define ACTIVE 1
#define CHARGE 2
#define DONE_CHARGE 3

#define FILTER_LEN 10
#define CELL_QTY 20
#define THERM_QTY 18
#define SIDE_A 1
#define SIDE_B 0
#define ATTEMPT_LIMIT 10

#define CONFIG_ENABLE_BALANCE 1				// Toggles the TORCH balance feature. 1 means balancing's enabled, 0 means it's disabled
#define CONFIG_LOCAL_BALANCE 1				// Toggles between using local and global minimum cell voltage as the reference for balancing. 1 means balance with respect to local min, 0 means balance with respect to global min

// Limits
#define MAX_TEMPERATURE 60
#define MAX_CELL_VOLTAGE 42000
#define MIN_CELL_VOLTAGE 25000

void MX_CAN1_Init(uint8_t mode);

extern const uint8_t moduleID;			// Defines which module this is
extern uint16_t transmissionDelay;		// Delay between message transmission

extern volatile uint32_t transmitCounter;
extern volatile uint32_t measureCounter;
extern volatile uint32_t balanceCounter;
extern volatile uint32_t transientCounter;
//extern volatile uint32_t canTimeoutCounter;

void torch_main(void);

#endif
