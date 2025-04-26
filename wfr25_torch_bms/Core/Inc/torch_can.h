#ifndef INC_TORCH_CAN_H_
#define INC_TORCH_CAN_H_

#define CAN_FAULT_ID 1000
#define CAN_PACK_STAT_ID 1056
#define CAN_MIN_VCELL_ID 999

// STAT status codes
#define STAT_DONE_CHARGING 127		// A cell has reached max VCELL, indicating that charging has stopped
#define STAT_STATS 128				// Sent by M2-M5, picked up by M1. M1 uses this data to determine min VCELL in entire pack
#define STAT_MIN_VCELL 129			// Sent by M1, picked up by M2-M5. Stores the global min VCELL. All modules will balance with respect to this VCELL

#define ERROR_OVERHEAT 69
#define ERROR_UNDERVOLT 70
#define ERROR_OVERVOLT 71
#define ERROR_IMBALANCE 72
#define ERROR_CELL_OPEN 73
#define ERROR_THERMISTOR_OPEN 74
#define ERROR_DIAGN 75
#define ERROR_AXST 76
#define ERROR_CVST 77
#define ERROR_STATST 78
#define ERROR_ADOW 79
#define ERROR_AXOW 80
#define ERROR_ADOL 81
#define ERROR_PEC 82
#define ERROR_CAN_READ 83

#define CAN_M1_STAT_ID 1001

// VOLTAGE MESSAGE IDs
#define CAN_M1_V1_ID 1006
#define CAN_M1_V2_ID 1007
#define CAN_M1_V3_ID 1008
#define CAN_M1_V4_ID 1009
#define CAN_M1_V5_ID 1010

#define CAN_M2_V1_ID 1011
#define CAN_M2_V2_ID 1012
#define CAN_M2_V3_ID 1013
#define CAN_M2_V4_ID 1014
#define CAN_M2_V5_ID 1015

#define CAN_M3_V1_ID 1016
#define CAN_M3_V2_ID 1017
#define CAN_M3_V3_ID 1018
#define CAN_M3_V4_ID 1019
#define CAN_M3_V5_ID 1020

#define CAN_M4_V1_ID 1021
#define CAN_M4_V2_ID 1022
#define CAN_M4_V3_ID 1023
#define CAN_M4_V4_ID 1024
#define CAN_M4_V5_ID 1025

#define CAN_M5_V1_ID 1026
#define CAN_M5_V2_ID 1027
#define CAN_M5_V3_ID 1028
#define CAN_M5_V4_ID 1029
#define CAN_M5_V5_ID 1030

// TEMPERATURE MESSAGE IDs
#define CAN_M1_T1_ID 1031
#define CAN_M1_T2_ID 1032
#define CAN_M1_T3_ID 1033
#define CAN_M1_T4_ID 1034
#define CAN_M1_T5_ID 1035

#define CAN_M2_T1_ID 1036
#define CAN_M2_T2_ID 1037
#define CAN_M2_T3_ID 1038
#define CAN_M2_T4_ID 1039
#define CAN_M2_T5_ID 1040

#define CAN_M3_T1_ID 1041
#define CAN_M3_T2_ID 1042
#define CAN_M3_T3_ID 1043
#define CAN_M3_T4_ID 1044
#define CAN_M3_T5_ID 1045

#define CAN_M4_T1_ID 1046
#define CAN_M4_T2_ID 1047
#define CAN_M4_T3_ID 1048
#define CAN_M4_T4_ID 1049
#define CAN_M4_T5_ID 1050

#define CAN_M5_T1_ID 1051
#define CAN_M5_T2_ID 1052
#define CAN_M5_T3_ID 1053
#define CAN_M5_T4_ID 1054
#define CAN_M5_T5_ID 1055

void can_transmit(uint16_t canMsgID, uint8_t *payload);

void transmit_voltages(uint16_t *cellVoltages);

void transmit_temperatures(float *temperatures);

void error_loop(uint8_t errorCode, uint16_t faultValue, uint8_t faultIndex);

void silent_error_loop(void);

#endif
