#include "main.h"
#include "torch_can.h"
#include "torch_main.h"
#include "torch_stm32.h"


void can_transmit(uint16_t canMsgID, uint8_t *payload)
{
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;

	TxHeader.DLC = 8;
	TxHeader.ExtId = 0;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.StdId = canMsgID;
	TxHeader.TransmitGlobalTime = DISABLE;

	HAL_CAN_AddTxMessage(&hcan1, &TxHeader, payload, &TxMailbox);
	wait(1);
}


void transmit_voltages(uint16_t *cellVoltages)
{
	uint8_t msgV1[8];
	uint8_t msgV2[8];
	uint8_t msgV3[8];
	uint8_t msgV4[8];
	uint8_t msgV5[8];

	msgV1[0] = (uint8_t)(*cellVoltages & 0xFF);
	msgV1[1] = (uint8_t)((*cellVoltages >> 8) & 0xFF);
	msgV1[2] = (uint8_t)(*(cellVoltages + 1) & 0xFF);
	msgV1[3] = (uint8_t)((*(cellVoltages + 1) >> 8) & 0xFF);
	msgV1[4] = (uint8_t)(*(cellVoltages + 2) & 0xFF);
	msgV1[5] = (uint8_t)((*(cellVoltages + 2) >> 8) & 0xFF);
	msgV1[6] = (uint8_t)(*(cellVoltages + 3) & 0xFF);
	msgV1[7] = (uint8_t)((*(cellVoltages + 3) >> 8) & 0xFF);

	msgV2[0] = (uint8_t)(*(cellVoltages + 4) & 0xFF);
	msgV2[1] = (uint8_t)((*(cellVoltages + 4) >> 8) & 0xFF);
	msgV2[2] = (uint8_t)(*(cellVoltages + 5) & 0xFF);
	msgV2[3] = (uint8_t)((*(cellVoltages + 5) >> 8) & 0xFF);
	msgV2[4] = (uint8_t)(*(cellVoltages + 6) & 0xFF);
	msgV2[5] = (uint8_t)((*(cellVoltages + 6) >> 8) & 0xFF);
	msgV2[6] = (uint8_t)(*(cellVoltages + 7) & 0xFF);
	msgV2[7] = (uint8_t)((*(cellVoltages + 7) >> 8) & 0xFF);

	msgV3[0] = (uint8_t)(*(cellVoltages + 8) & 0xFF);
	msgV3[1] = (uint8_t)((*(cellVoltages + 8) >> 8) & 0xFF);
	msgV3[2] = (uint8_t)(*(cellVoltages + 9) & 0xFF);
	msgV3[3] = (uint8_t)((*(cellVoltages + 9) >> 8) & 0xFF);
	msgV3[4] = (uint8_t)(*(cellVoltages + 10) & 0xFF);
	msgV3[5] = (uint8_t)((*(cellVoltages + 10) >> 8) & 0xFF);
	msgV3[6] = (uint8_t)(*(cellVoltages + 11) & 0xFF);
	msgV3[7] = (uint8_t)((*(cellVoltages + 11) >> 8) & 0xFF);

	msgV4[0] = (uint8_t)(*(cellVoltages + 12) & 0xFF);
	msgV4[1] = (uint8_t)((*(cellVoltages + 12) >> 8) & 0xFF);
	msgV4[2] = (uint8_t)(*(cellVoltages + 13) & 0xFF);
	msgV4[3] = (uint8_t)((*(cellVoltages + 13) >> 8) & 0xFF);
	msgV4[4] = (uint8_t)(*(cellVoltages + 14) & 0xFF);
	msgV4[5] = (uint8_t)((*(cellVoltages + 14) >> 8) & 0xFF);
	msgV4[6] = (uint8_t)(*(cellVoltages + 15) & 0xFF);
	msgV4[7] = (uint8_t)((*(cellVoltages + 15) >> 8) & 0xFF);

	msgV5[0] = (uint8_t)(*(cellVoltages + 16) & 0xFF);
	msgV5[1] = (uint8_t)((*(cellVoltages + 16) >> 8) & 0xFF);
	msgV5[2] = (uint8_t)(*(cellVoltages + 17) & 0xFF);
	msgV5[3] = (uint8_t)((*(cellVoltages + 17) >> 8) & 0xFF);
	msgV5[4] = (uint8_t)(*(cellVoltages + 18) & 0xFF);
	msgV5[5] = (uint8_t)((*(cellVoltages + 18) >> 8) & 0xFF);
	msgV5[6] = (uint8_t)(*(cellVoltages + 19) & 0xFF);
	msgV5[7] = (uint8_t)((*(cellVoltages + 19) >> 8) & 0xFF);

	switch(moduleID) {
		case 1:
			can_transmit(CAN_M1_V1_ID, msgV1);
			can_transmit(CAN_M1_V2_ID, msgV2);
			can_transmit(CAN_M1_V3_ID, msgV3);
			can_transmit(CAN_M1_V4_ID, msgV4);
			can_transmit(CAN_M1_V5_ID, msgV5);
			break;
		case 2:
			can_transmit(CAN_M2_V1_ID, msgV1);
			can_transmit(CAN_M2_V2_ID, msgV2);
			can_transmit(CAN_M2_V3_ID, msgV3);
			can_transmit(CAN_M2_V4_ID, msgV4);
			can_transmit(CAN_M2_V5_ID, msgV5);
			break;
		case 3:
			can_transmit(CAN_M3_V1_ID, msgV1);
			can_transmit(CAN_M3_V2_ID, msgV2);
			can_transmit(CAN_M3_V3_ID, msgV3);
			can_transmit(CAN_M3_V4_ID, msgV4);
			can_transmit(CAN_M3_V5_ID, msgV5);
			break;
		case 4:
			can_transmit(CAN_M4_V1_ID, msgV1);
			can_transmit(CAN_M4_V2_ID, msgV2);
			can_transmit(CAN_M4_V3_ID, msgV3);
			can_transmit(CAN_M4_V4_ID, msgV4);
			can_transmit(CAN_M4_V5_ID, msgV5);
			break;
		case 5:
			can_transmit(CAN_M5_V1_ID, msgV1);
			can_transmit(CAN_M5_V2_ID, msgV2);
			can_transmit(CAN_M5_V3_ID, msgV3);
			can_transmit(CAN_M5_V4_ID, msgV4);
			can_transmit(CAN_M5_V5_ID, msgV5);
			break;
	}
}


void transmit_temperatures(float *temperatures)
{
	uint8_t msgT1[8];
	uint8_t msgT2[8];
	uint8_t msgT3[8];
	uint8_t msgT4[8];
	uint8_t msgT5[8];
	float tempScale = 1000.0f;
	uint16_t intTemps[THERM_QTY];

	for(uint8_t i = 0; i < THERM_QTY; i++) {
		intTemps[i] = (uint16_t)(*(temperatures + i) * tempScale);
	}

	msgM1T1[0] = (uint8_t)(intTemps[0] & 0xFF);
	msgM1T1[1] = (uint8_t)((intTemps[0] >> 8) & 0xFF);
	msgM1T1[2] = (uint8_t)(intTemps[1] & 0xFF);
	msgM1T1[3] = (uint8_t)((intTemps[1] >> 8) & 0xFF);
	msgM1T1[4] = (uint8_t)(intTemps[2] & 0xFF);
	msgM1T1[5] = (uint8_t)((intTemps[2] >> 8) & 0xFF);
	msgM1T1[6] = (uint8_t)(intTemps[3] & 0xFF);
	msgM1T1[7] = (uint8_t)((intTemps[3] >> 8) & 0xFF);

	msgM1T2[0] = (uint8_t)(intTemps[4] & 0xFF);
	msgM1T2[1] = (uint8_t)((intTemps[4] >> 8) & 0xFF);
	msgM1T2[2] = (uint8_t)(intTemps[5] & 0xFF);
	msgM1T2[3] = (uint8_t)((intTemps[5] >> 8) & 0xFF);
	msgM1T2[4] = (uint8_t)(intTemps[6] & 0xFF);
	msgM1T2[5] = (uint8_t)((intTemps[6] >> 8) & 0xFF);
	msgM1T2[6] = (uint8_t)(intTemps[7] & 0xFF);
	msgM1T2[7] = (uint8_t)((intTemps[7] >> 8) & 0xFF);

	msgM1T3[0] = (uint8_t)(intTemps[8] & 0xFF);
	msgM1T3[1] = (uint8_t)((intTemps[8] >> 8) & 0xFF);
	msgM1T3[2] = (uint8_t)(intTemps[9] & 0xFF);
	msgM1T3[3] = (uint8_t)((intTemps[9] >> 8) & 0xFF);
	msgM1T3[4] = (uint8_t)(intTemps[10] & 0xFF);
	msgM1T3[5] = (uint8_t)((intTemps[10] >> 8) & 0xFF);
	msgM1T3[6] = (uint8_t)(intTemps[11] & 0xFF);
	msgM1T3[7] = (uint8_t)((intTemps[11] >> 8) & 0xFF);

	msgM1T4[0] = (uint8_t)(intTemps[12] & 0xFF);
	msgM1T4[1] = (uint8_t)((intTemps[12] >> 8) & 0xFF);
	msgM1T4[2] = (uint8_t)(intTemps[13] & 0xFF);
	msgM1T4[3] = (uint8_t)((intTemps[13] >> 8) & 0xFF);
	msgM1T4[4] = (uint8_t)(intTemps[14] & 0xFF);
	msgM1T4[5] = (uint8_t)((intTemps[14] >> 8) & 0xFF);
	msgM1T4[6] = (uint8_t)(intTemps[15] & 0xFF);
	msgM1T4[7] = (uint8_t)((intTemps[15] >> 8) & 0xFF);

	msgM1T5[0] = (uint8_t)(intTemps[16] & 0xFF);
	msgM1T5[1] = (uint8_t)((intTemps[16] >> 8) & 0xFF);
	msgM1T5[2] = (uint8_t)(intTemps[17] & 0xFF);
	msgM1T5[3] = (uint8_t)((intTemps[17] >> 8) & 0xFF);
	msgM1T5[4] = 0;
	msgM1T5[5] = 0;
	msgM1T5[6] = 0;
	msgM1T5[7] = 0;

	switch(moduleID) {

	}
}
