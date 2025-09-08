#include "main.h"
#include "torch_can.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_config.h"


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
	wait(10);
}


void silent_error_loop(void)
{
	pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
	pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
	//stop_timer(&htim2);
	//HAL_CAN_Stop(&hcan1);

	uint16_t cellVoltages[CELL_QTY];
	float temperatures[THERM_QTY];
	transmitCounter = 0;
	measureCounter = 0;
	while(1) {

		if(measureCounter > 100) {
			if(!refup_check()) {
				force_refup();
				wait(1);
			}

			temperature_sense(temperatures);
			voltage_sense(cellVoltages);

			measureCounter = 0;
		}

		if(transmitCounter > transmissionDelay) {
			transmit_voltages(cellVoltages);
			transmit_temperatures(temperatures);

			transmitCounter = 0;
		}

		wait(1);
	}
}


void error_loop(uint8_t errorCode, uint16_t faultValue, uint8_t faultIndex)
{
	uint8_t msgFault[8];

	msgFault[0] = moduleID;
	msgFault[1] = errorCode;

	switch(errorCode) {
		case ERROR_OVERHEAT:
			// Setting cell voltage portions of fault message to zero
			msgFault[2] = 0;
			msgFault[3] = 0;
			msgFault[4] = 0;

			msgFault[5] = (uint8_t)(faultValue & 0xFF);
			msgFault[6] = (uint8_t)((faultValue >> 8) & 0xFF);
			msgFault[7] = faultIndex;
			break;
		case ERROR_THERMISTOR_OPEN:
			msgFault[2] = 0;
			msgFault[3] = 0;
			msgFault[4] = 0;

			msgFault[5] = 0;
			msgFault[6] = 0;
			msgFault[7] = faultIndex;
			break;
		case ERROR_UNDERVOLT:
			msgFault[2] = (uint8_t)(faultValue & 0xFF);
			msgFault[3] = (uint8_t)((faultValue >> 8) & 0xFF);
			msgFault[4] = faultIndex;

			// Setting temperature portions of fault message to zero
			msgFault[5] = 0;
			msgFault[6] = 0;
			msgFault[7] = 0;
			break;
		case ERROR_OVERVOLT:
			msgFault[2] = (uint8_t)(faultValue & 0xFF);
			msgFault[3] = (uint8_t)((faultValue >> 8) & 0xFF);
			msgFault[4] = faultIndex;

			// Setting temperature portions of fault message to zero
			msgFault[5] = 0;
			msgFault[6] = 0;
			msgFault[7] = 0;
			break;
		default:
			msgFault[2] = 0;
			msgFault[3] = 0;
			msgFault[4] = 0;
			msgFault[5] = 0;
			msgFault[6] = 0;
			msgFault[7] = 0;
			break;
	}
	active_led_on();
	charge_led_on();
	balance_led_off();
	hot_led_off();

	uint16_t cellVoltages[CELL_QTY];
	float temperatures[THERM_QTY];
	transmitCounter = 0;
	measureCounter = 0;
	while(1) {

		if(measureCounter > 100) {
			if(!refup_check()) {
				force_refup();
				wait(1);
			}

			temperature_sense(temperatures);
			voltage_sense(cellVoltages);

			measureCounter = 0;
		}

		if(transmitCounter > transmissionDelay) {
			can_transmit(CAN_FAULT_ID, msgFault);
			transmit_voltages(cellVoltages);
			transmit_temperatures(temperatures);

			transmitCounter = 0;
		}

		wait(1);
	}
}


void transmit_balance(uint8_t *balanceMsg)
{
	uint8_t payload[8] = {0};

	for(uint8_t i = 0; i < 8; i++) { payload[i] = *(balanceMsg + i); }

	switch(moduleID) {
		case 1:
			can_transmit(CAN_M1_BALANCE_ID, payload);
			break;
		case 2:
			can_transmit(CAN_M2_BALANCE_ID, payload);
			break;
		case 3:
			can_transmit(CAN_M3_BALANCE_ID, payload);
			break;
		case 4:
			can_transmit(CAN_M4_BALANCE_ID, payload);
			break;
		case 5:
			can_transmit(CAN_M5_BALANCE_ID, payload);
			break;
	}
}


void transmit_balance_initiation(uint16_t absMinCellVoltage)
{
	uint8_t payload[8] = {0};

	payload[0] = (uint8_t)(absMinCellVoltage & 0xFF);
	payload[1] = (uint8_t)((absMinCellVoltage >> 8) & 0xFF);

	can_transmit(CAN_MIN_VCELL_ID, payload);
}


void transmit_vmin(uint16_t minCellVoltage)
{
	uint8_t payload[8] = {0};

	payload[0] = (uint8_t)(minCellVoltage & 0xFF);
	payload[1] = (uint8_t)((minCellVoltage >> 8) & 0xFF);

	switch(moduleID) {
		case 2:
			can_transmit(CAN_M2_VMIN_ID, payload);
			break;
		case 3:
			can_transmit(CAN_M3_VMIN_ID, payload);
			break;
		case 4:
			can_transmit(CAN_M4_VMIN_ID, payload);
			break;
		case 5:
			can_transmit(CAN_M5_VMIN_ID, payload);
			break;
	}
}

void transmit_extract_vmin(void)
{
	uint8_t zeroPayload[8] = {0};

	can_transmit(CAN_EXTRACT_VMIN_ID, zeroPayload);
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

	msgT1[0] = (uint8_t)(intTemps[0] & 0xFF);
	msgT1[1] = (uint8_t)((intTemps[0] >> 8) & 0xFF);
	msgT1[2] = (uint8_t)(intTemps[1] & 0xFF);
	msgT1[3] = (uint8_t)((intTemps[1] >> 8) & 0xFF);
	msgT1[4] = (uint8_t)(intTemps[2] & 0xFF);
	msgT1[5] = (uint8_t)((intTemps[2] >> 8) & 0xFF);
	msgT1[6] = (uint8_t)(intTemps[3] & 0xFF);
	msgT1[7] = (uint8_t)((intTemps[3] >> 8) & 0xFF);

	msgT2[0] = (uint8_t)(intTemps[4] & 0xFF);
	msgT2[1] = (uint8_t)((intTemps[4] >> 8) & 0xFF);
	msgT2[2] = (uint8_t)(intTemps[5] & 0xFF);
	msgT2[3] = (uint8_t)((intTemps[5] >> 8) & 0xFF);
	msgT2[4] = (uint8_t)(intTemps[6] & 0xFF);
	msgT2[5] = (uint8_t)((intTemps[6] >> 8) & 0xFF);
	msgT2[6] = (uint8_t)(intTemps[7] & 0xFF);
	msgT2[7] = (uint8_t)((intTemps[7] >> 8) & 0xFF);

	msgT3[0] = (uint8_t)(intTemps[8] & 0xFF);
	msgT3[1] = (uint8_t)((intTemps[8] >> 8) & 0xFF);
	msgT3[2] = (uint8_t)(intTemps[9] & 0xFF);
	msgT3[3] = (uint8_t)((intTemps[9] >> 8) & 0xFF);
	msgT3[4] = (uint8_t)(intTemps[10] & 0xFF);
	msgT3[5] = (uint8_t)((intTemps[10] >> 8) & 0xFF);
	msgT3[6] = (uint8_t)(intTemps[11] & 0xFF);
	msgT3[7] = (uint8_t)((intTemps[11] >> 8) & 0xFF);

	msgT4[0] = (uint8_t)(intTemps[12] & 0xFF);
	msgT4[1] = (uint8_t)((intTemps[12] >> 8) & 0xFF);
	msgT4[2] = (uint8_t)(intTemps[13] & 0xFF);
	msgT4[3] = (uint8_t)((intTemps[13] >> 8) & 0xFF);
	msgT4[4] = (uint8_t)(intTemps[14] & 0xFF);
	msgT4[5] = (uint8_t)((intTemps[14] >> 8) & 0xFF);
	msgT4[6] = (uint8_t)(intTemps[15] & 0xFF);
	msgT4[7] = (uint8_t)((intTemps[15] >> 8) & 0xFF);

	msgT5[0] = (uint8_t)(intTemps[16] & 0xFF);
	msgT5[1] = (uint8_t)((intTemps[16] >> 8) & 0xFF);
	msgT5[2] = (uint8_t)(intTemps[17] & 0xFF);
	msgT5[3] = (uint8_t)((intTemps[17] >> 8) & 0xFF);
	msgT5[4] = 0;
	msgT5[5] = 0;
	msgT5[6] = 0;
	msgT5[7] = 0;

	switch(moduleID) {
		case 1:
			can_transmit(CAN_M1_T1_ID, msgT1);
			can_transmit(CAN_M1_T2_ID, msgT2);
			can_transmit(CAN_M1_T3_ID, msgT3);
			can_transmit(CAN_M1_T4_ID, msgT4);
			can_transmit(CAN_M1_T5_ID, msgT5);
			break;
		case 2:
			can_transmit(CAN_M2_T1_ID, msgT1);
			can_transmit(CAN_M2_T2_ID, msgT2);
			can_transmit(CAN_M2_T3_ID, msgT3);
			can_transmit(CAN_M2_T4_ID, msgT4);
			can_transmit(CAN_M2_T5_ID, msgT5);
			break;
		case 3:
			can_transmit(CAN_M3_T1_ID, msgT1);
			can_transmit(CAN_M3_T2_ID, msgT2);
			can_transmit(CAN_M3_T3_ID, msgT3);
			can_transmit(CAN_M3_T4_ID, msgT4);
			can_transmit(CAN_M3_T5_ID, msgT5);
			break;
		case 4:
			can_transmit(CAN_M4_T1_ID, msgT1);
			can_transmit(CAN_M4_T2_ID, msgT2);
			can_transmit(CAN_M4_T3_ID, msgT3);
			can_transmit(CAN_M4_T4_ID, msgT4);
			can_transmit(CAN_M4_T5_ID, msgT5);
			break;
		case 5:
			can_transmit(CAN_M5_T1_ID, msgT1);
			can_transmit(CAN_M5_T2_ID, msgT2);
			can_transmit(CAN_M5_T3_ID, msgT3);
			can_transmit(CAN_M5_T4_ID, msgT4);
			can_transmit(CAN_M5_T5_ID, msgT5);
			break;
	}
}
