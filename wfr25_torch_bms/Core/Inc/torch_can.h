#ifndef INC_TORCH_CAN_H_
#define INC_TORCH_CAN_H_

#define ERROR_OVERHEAT 69
#define ERROR_UNDERVOLT 70
#define ERROR_OVERVOLT 71
#define ERROR_CELL_OPEN 72
#define ERROR_THERMISTOR_OPEN 73
#define ERROR_DIAGN 74
#define ERROR_MUTE 75
#define ERROR_CVST 76
#define ERROR_STATST 77
#define ERROR_AXST 78
#define ERROR_ADOL 79
#define ERROR_OUT_OF_RANGE_VA 80
#define ERROR_OUT_OF_RANGE_VD 81
#define ERROR_OUT_OF_RANGE_REF2 82
#define ERROR_LTC6813_OVERHEAT 83
#define ERROR_PWM_SETUP 84
#define ERROR_BALANCE_INITIATION 85
#define ERROR_PEC 86
#define ERROR_CAN_READ 87

// Balance message IDs
#define CAN_M1_BALANCE_ID 1001
#define CAN_M2_BALANCE_ID 1002
#define CAN_M3_BALANCE_ID 1003
#define CAN_M4_BALANCE_ID 1004
#define CAN_M5_BALANCE_ID 1005

// Voltage message IDs
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

// FORCE BALANCE MESSAGE IDs
#define CAN_EXTRACT_VMIN_ID 69
#define CAN_M2_VMIN_ID 70
#define CAN_M3_VMIN_ID 71
#define CAN_M4_VMIN_ID 72
#define CAN_M5_VMIN_ID 73

void can_transmit(uint16_t canMsgID, uint8_t *payload);

void transmit_balance(uint8_t *balanceMsg);

void transmit_balance_initiation(uint16_t absMinCellVoltage);

void transmit_extract_vmin(void);

void transmit_vmin(uint16_t minCellVoltage);

void transmit_voltages(uint16_t *cellVoltages);

void transmit_temperatures(float *temperatures);

void error_loop(uint8_t errorCode, uint16_t faultValue, uint8_t faultIndex);

void silent_error_loop(void);

#endif
