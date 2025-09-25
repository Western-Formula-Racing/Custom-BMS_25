#ifndef INC_TORCH_MAIN_H_
#define INC_TORCH_MAIN_H_

// Debouncing control
#define ATTEMPT_LIMIT 10			// Number of times the BMS will repeat operations (i.e. measurements, diagnosis, etc.) before throwing a fault (default: 10 tries)

// Module characteristics
#define CELL_QTY 20					// Number of series connected cells in the module (default: 20 cells)
#define MODULE_THERM_QTY 18			// Number of thermistors embedded within module measuring the cell temperatures (default: 18 thermistors)
#define PCB_THERM_QTY 18			// Number of thermistors on interface PCB measuring the balance resistor temperatures (default: 18 thermistors)

// Limits
#define MAX_TEMPERATURE 60				// Maximum permitted cell temperature before a fault (default: 60 C)
#define MAX_CELL_VOLTAGE 42000			// Maximum cell voltage before a fault (default: 4.2 V)
#define MIN_CELL_VOLTAGE 25000			// Minimum cell voltage before a fault (default: 2.5 V)

// Interrupt intervals (in milliseconds)
#define MEASURE_INTERVAL 100				// How often cell voltages and temperatures are measured (default: 100 milliseconds)
#define QUICK_DIAGNOSIS_INTERVAL 10000		// How often the BMS performs the quick self diagnosis (default: 10 seconds)
#define BALANCE_COMMAND_DURATION 5000		// How long module 1 transmits the balance initiation CAN message, ordering modules 2 to 5 to begin balancing (default: 5 seconds)
#define BALANCE_CYCLE_DURATION 60000		// How long the BMS balances continuously balances cells for (default: 60 seconds)
#define POST_BALANCE_DELAY 20000			// How long the BMS waits after cell balancing to measure the cell voltages (default: 20 seconds)

// Global variables
extern const uint8_t bmsMode;			// Sets the operating mode of the BMS
extern const uint8_t moduleID;			// Defines which module this is
extern uint16_t transmissionDelay;		// Delay between message transmission

// Counters
extern volatile uint32_t transmitCounter;		// Counter for CAN message transmission
extern volatile uint32_t measureCounter;		// Counter for measuring cell voltages and temperatures
extern volatile uint32_t balanceCounter;		// Counter used during cell balancing
extern volatile uint32_t diagnosisCounter;		// Counter for the BMS's self diagnosis

// Main function of the BMS program
void torch_main(void);

#endif
