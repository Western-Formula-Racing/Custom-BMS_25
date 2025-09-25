#ifndef INC_TORCH_DEPENDENCIES_H_
#define INC_TORCH_DEPENDENCIES_H_

// External CAN message decimal IDs
#define CAN_PACK_STATUS_ID 1056			// PackStatus message sent by the accumulator motherboard
#define CAN_START_BALANCE_ID 998		// Message that orders the BMS to start balancing
#define CAN_STOP_BALANCE_ID 999			// Message that orders the BMS to halt balancing

// PackStatus values
#define PACK_STATUS_BYTE_POSITION 5		// Defines the PackStatus byte position in the PackStatus CAN message
#define PACK_STATUS_IDLE 0				// PackStatus byte value of 0 indicates that the accumulator is idle (AIRs are open)
#define PACK_STATUS_PRECHARGE_START 1
#define PACK_STATUS_PRECHARGING 2
#define PACK_STATUS_ACTIVE 3
#define PACK_STATUS_CHARGING 4			// PackStatus byte value of 4 indicates that the accumulator is charging
#define PACK_STATUS_CHARGING_COMPLETE 5
#define PACK_STATUS_FAULT 6				// PackStatus byte value of 6 indicates a fault

#endif
