#include "main.h"
#include "torch_main.h"
#include "torch_STM32.h"
#include "torch_LTC6813.h"
#include "torch_voltage.h"


void voltage_sense(void)
{
	LTC6813 asicRegisters = {0};
	
	ADCV(SIDE_A);
	wait(2);		// consider waiting for 2 ms..
	RDCVA(&asicRegisters, SIDE_A);
	wait(1);
	RDCVB(&asicRegisters, SIDE_A);
	wait(1);
	RDCVC(&asicRegisters, SIDE_A);
	wait(1);
	RDCVD(&asicRegisters, SIDE_A);
	
	/*
	*cellVoltages = (asicRegisters.voltageRegisterA[1] << 8) | asicRegisters.voltageRegisterA[0];
	*(cellVoltages + 1) = (asicRegisters.voltageRegisterA[3] << 8) | asicRegisters.voltageRegisterA[2];
	*(cellVoltages + 2) = (asicRegisters.voltageRegisterA[5] << 8) | asicRegisters.voltageRegisterA[4];
	*(cellVoltages + 3) = (asicRegisters.voltageRegisterB[1] << 8) | asicRegisters.voltageRegisterB[0];
	*(cellVoltages + 4) = (asicRegisters.voltageRegisterB[3] << 8) | asicRegisters.voltageRegisterB[2];
	*(cellVoltages + 5) = (asicRegisters.voltageRegisterB[5] << 8) | asicRegisters.voltageRegisterB[4];
	*(cellVoltages + 6) = (asicRegisters.voltageRegisterC[1] << 8) | asicRegisters.voltageRegisterC[0];
	*(cellVoltages + 7) = (asicRegisters.voltageRegisterC[3] << 8) | asicRegisters.voltageRegisterC[2];
	*(cellVoltages + 8) = (asicRegisters.voltageRegisterC[5] << 8) | asicRegisters.voltageRegisterC[4];
	*(cellVoltages + 9) = (asicRegisters.voltageRegisterD[1] << 8) | asicRegisters.voltageRegisterD[0];
	*/
	bmsInputs.cellVoltages[0] = (asicRegisters.voltageRegisterA[1] << 8) | asicRegisters.voltageRegisterA[0];
	bmsInputs.cellVoltages[1] = (asicRegisters.voltageRegisterA[3] << 8) | asicRegisters.voltageRegisterA[2];
	bmsInputs.cellVoltages[2] = (asicRegisters.voltageRegisterA[5] << 8) | asicRegisters.voltageRegisterA[4];
	bmsInputs.cellVoltages[3] = (asicRegisters.voltageRegisterB[1] << 8) | asicRegisters.voltageRegisterB[0];
	bmsInputs.cellVoltages[4] = (asicRegisters.voltageRegisterB[3] << 8) | asicRegisters.voltageRegisterB[2];
	bmsInputs.cellVoltages[5] = (asicRegisters.voltageRegisterB[5] << 8) | asicRegisters.voltageRegisterB[4];
	bmsInputs.cellVoltages[6] = (asicRegisters.voltageRegisterC[1] << 8) | asicRegisters.voltageRegisterC[0];
	bmsInputs.cellVoltages[7] = (asicRegisters.voltageRegisterC[3] << 8) | asicRegisters.voltageRegisterC[2];
	bmsInputs.cellVoltages[8] = (asicRegisters.voltageRegisterC[5] << 8) | asicRegisters.voltageRegisterC[4];
	bmsInputs.cellVoltages[9] = (asicRegisters.voltageRegisterD[1] << 8) | asicRegisters.voltageRegisterD[0];
	
	ADCV(SIDE_B);
	wait(2);		// consider waiting for 2 ms..
	RDCVA(&asicRegisters, SIDE_B);
	wait(1);
	RDCVB(&asicRegisters, SIDE_B);
	wait(1);
	RDCVC(&asicRegisters, SIDE_B);
	wait(1);
	RDCVD(&asicRegisters, SIDE_B);
	wait(1);

	/*
	*(cellVoltages + 10) = (asicRegisters.voltageRegisterA[1] << 8) | asicRegisters.voltageRegisterA[0];
	*(cellVoltages + 11) = (asicRegisters.voltageRegisterA[3] << 8) | asicRegisters.voltageRegisterA[2];
	*(cellVoltages + 12) = (asicRegisters.voltageRegisterA[5] << 8) | asicRegisters.voltageRegisterA[4];
	*(cellVoltages + 13) = (asicRegisters.voltageRegisterB[1] << 8) | asicRegisters.voltageRegisterB[0];
	*(cellVoltages + 14) = (asicRegisters.voltageRegisterB[3] << 8) | asicRegisters.voltageRegisterB[2];
	*(cellVoltages + 15) = (asicRegisters.voltageRegisterB[5] << 8) | asicRegisters.voltageRegisterB[4];
	*(cellVoltages + 16) = (asicRegisters.voltageRegisterC[1] << 8) | asicRegisters.voltageRegisterC[0];
	*(cellVoltages + 17) = (asicRegisters.voltageRegisterC[3] << 8) | asicRegisters.voltageRegisterC[2];
	*(cellVoltages + 18) = (asicRegisters.voltageRegisterC[5] << 8) | asicRegisters.voltageRegisterC[4];
	*(cellVoltages + 19) = (asicRegisters.voltageRegisterD[1] << 8) | asicRegisters.voltageRegisterD[0];
	*/
	bmsInputs.cellVoltages[10] = (asicRegisters.voltageRegisterA[1] << 8) | asicRegisters.voltageRegisterA[0];
	bmsInputs.cellVoltages[11] = (asicRegisters.voltageRegisterA[3] << 8) | asicRegisters.voltageRegisterA[2];
	bmsInputs.cellVoltages[12] = (asicRegisters.voltageRegisterA[5] << 8) | asicRegisters.voltageRegisterA[4];
	bmsInputs.cellVoltages[13] = (asicRegisters.voltageRegisterB[1] << 8) | asicRegisters.voltageRegisterB[0];
	bmsInputs.cellVoltages[14] = (asicRegisters.voltageRegisterB[3] << 8) | asicRegisters.voltageRegisterB[2];
	bmsInputs.cellVoltages[15] = (asicRegisters.voltageRegisterB[5] << 8) | asicRegisters.voltageRegisterB[4];
	bmsInputs.cellVoltages[16] = (asicRegisters.voltageRegisterC[1] << 8) | asicRegisters.voltageRegisterC[0];
	bmsInputs.cellVoltages[17] = (asicRegisters.voltageRegisterC[3] << 8) | asicRegisters.voltageRegisterC[2];
	bmsInputs.cellVoltages[18] = (asicRegisters.voltageRegisterC[5] << 8) | asicRegisters.voltageRegisterC[4];
	bmsInputs.cellVoltages[19] = (asicRegisters.voltageRegisterD[1] << 8) | asicRegisters.voltageRegisterD[0];
}

// FOLLOWING CODE'S OBSOLETE PROTOTYPING CODE (not deleting it quite yet, as some parts are reusable)
void balance_heat_test(void)
{
	Counter = 0;
	float temperatures[THERM_QTY];
	uint8_t payloadRegisterA[8];
	uint8_t payloadRegisterB[2];

	// Setting VUV to 3 V for test
	payloadRegisterA[0] = 0xFE;
	payloadRegisterA[1] = 0x52;
	payloadRegisterA[2] = 0x47;
	payloadRegisterA[3] = 0x9C;

	payloadRegisterA[4] = 0x00;		// Cell to balance
	//payloadRegisterA[5] = 0x10;		// DCTO of 30 seconds
	//payloadRegisterA[5] = 0x20;		// DCTO of 1 minute
	payloadRegisterA[5] = 0x32;		// DCTO of 2 minute

	payloadRegisterB[0] = 0x0F;
	payloadRegisterB[1] = 0x08;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);		// Turns off ACTIVE LED
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);	// Turns off STANDBY LED (changed to DEBUG on final)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);	// Turns on CHARGE LED

	WRCFGA(payloadRegisterA, SIDE_B);
	HAL_Delay(1);
	WRCFGB(payloadRegisterB, SIDE_B);

	HAL_TIM_Base_Start_IT(&htim2);
	while (Counter <= 122000) {			// Counter should be adaptive to DCTO
		temperature_sense(temperatures);
		for(uint8_t i = 0; i < 16; i++) {
			if(temperatures[i] >= 60.0) {
				HAL_TIM_Base_Stop_IT(&htim2);
				__HAL_TIM_SET_COUNTER(&htim2, 0);

				force_refup();
				HAL_Delay(1);
				MUTE(SIDE_A);
				while(1) {
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
					HAL_Delay(200);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
					HAL_Delay(200);
				}
			}
		}
		HAL_Delay(1);
	}
	HAL_TIM_Base_Stop_IT(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	Counter = 0;
}

void balance_cells(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty)
{
	// Setting VUV to 3.624 V / 0x8D8
	// Setting VOV to 4 V / 0x09C4
	Counter = 0;

	uint8_t evenCellCount = 0;
	uint8_t oddCellCount = 0;

	// For loop below counts up all the even & odd cells that need to be balanced
	for(uint8_t i = 0; i < cellsToBalanceQty; i ++) {
		if(*(cellsToBalance + i) % 2 == 0) { evenCellCount++; }

		else { oddCellCount++; }
	}

	uint8_t evenCells[evenCellCount];
	uint8_t oddCells[oddCellCount];
	uint8_t evenIndex = 0;
	uint8_t oddIndex = 0;

	// For loop below assigns all even and odd cells in need of balancing to their own arrays
	for(uint8_t i = 0; i < cellsToBalanceQty; i++) {
		if(*(cellsToBalance + i) % 2 == 0) { evenCells[evenIndex++] = *(cellsToBalance + i); }

		else { oddCells[oddIndex++] = *(cellsToBalance + i); }
	}

	uint8_t sideAEvenCount = 0;
	uint8_t sideAOddCount = 0;
	uint8_t sideBEvenCount = 0;
	uint8_t sideBOddCount = 0;

	for(uint8_t i = 0; i < evenCellCount; i++) {
		if(evenCells[i] > 10) { sideBEvenCount++; }

		else { sideAEvenCount++; }
	}
	for(uint8_t i = 0; i < oddCellCount; i++) {
		if(oddCells[i] > 9) { sideBOddCount++; }

		else { sideAOddCount++; }
	}

	uint8_t sideAEvenCells[sideAEvenCount];
	uint8_t sideAOddCells[sideAOddCount];
	uint8_t sideBEvenCells[sideBEvenCount];
	uint8_t sideBOddCells[sideBOddCount];
	uint8_t sideAEvenIndex = 0;
	uint8_t sideAOddIndex = 0;
	uint8_t sideBEvenIndex = 0;
	uint8_t sideBOddIndex = 0;

	for(uint8_t i = 0; i < evenCellCount; i++) {
		if(evenCells[i] > 10) { sideBEvenCells[sideBEvenIndex++] = evenCells[i] - 10; }

		else { sideAEvenCells[sideAEvenIndex++] = evenCells[i]; }
	}
	for(uint8_t i = 0; i < oddCellCount; i++) {
		if(oddCells[i] > 9) { sideBOddCells[sideBOddIndex++] = oddCells[i] - 10; }

		else { sideAOddCells[sideAOddIndex++] = oddCells[i]; }
	}

	uint8_t sideA_payloadRegisterA[8];
	uint8_t sideB_payloadRegisterA[8];
	uint8_t payloadRegisterB[2];

	uint8_t DCC1 = 0, DCC2 = 0, DCC3 = 0, DCC4 = 0, DCC5 = 0, DCC6 = 0, DCC7 = 0, DCC8 = 0, DCC9 = 0, DCC10 = 0;
	uint8_t DCTO = 0x2;		// Add logic for determining ideal DCTO

	sideA_payloadRegisterA[0] = 0xFE;
	sideB_payloadRegisterA[0] = 0xFE;

	sideA_payloadRegisterA[1] = 0xD8;
	sideB_payloadRegisterA[1] = 0xD8;

	sideA_payloadRegisterA[2] = 0x48;
	sideB_payloadRegisterA[2] = 0x48;

	sideA_payloadRegisterA[3] = 0x9C;
	sideB_payloadRegisterA[3] = 0x9C;

	payloadRegisterB[0] = 0x0F;
	payloadRegisterB[1] = 0x08;

	for(uint8_t i = 0; i < sideAEvenCount; i++) {
		switch(sideAEvenCells[i]) {
		case 2:
			DCC2 = 1;
			break;
		case 4:
			DCC4 = 1;
			break;
		case 6:
			DCC6 = 1;
			break;
		case 8:
			DCC8 = 1;
			break;
		case 10:
			DCC10 = 1;
			break;
		default:		// default should NEVER happen. Put proper error handling here later
		}
	}
	sideA_payloadRegisterA[4] = (DCC8 << 7) | (DCC7 << 6) | (DCC6 << 5) | (DCC5 << 4) | (DCC4 << 3) | (DCC3 << 2) | (DCC2 << 1) | (DCC1 << 0);
	sideA_payloadRegisterA[5] = (DCTO << 4) | (DCC10 << 1) | (DCC9 << 0);
	DCC1 = 0, DCC2 = 0, DCC3 = 0, DCC4 = 0, DCC5 = 0, DCC6 = 0, DCC7 = 0, DCC8 = 0, DCC9 = 0, DCC10 = 0;

	for(uint8_t i = 0; i < sideBEvenCount; i++) {
		switch(sideBEvenCells[i]) {
		case 2:
			DCC2 = 1;
			break;
		case 4:
			DCC4 = 1;
			break;
		case 6:
			DCC6 = 1;
			break;
		case 8:
			DCC8 = 1;
			break;
		case 10:
			DCC10 = 1;
			break;
		default:		// default should NEVER happen. Put proper error handling here later
		}
	}
	sideB_payloadRegisterA[4] = (DCC8 << 7) | (DCC7 << 6) | (DCC6 << 5) | (DCC5 << 4) | (DCC4 << 3) | (DCC3 << 2) | (DCC2 << 1) | (DCC1 << 0);
	sideB_payloadRegisterA[5] = (DCTO << 4) | (DCC10 << 1) | (DCC9 << 0);
	DCC1 = 0, DCC2 = 0, DCC3 = 0, DCC4 = 0, DCC5 = 0, DCC6 = 0, DCC7 = 0, DCC8 = 0, DCC9 = 0, DCC10 = 0;

	force_refup();
	HAL_Delay(1);

	WRCFGA(sideA_payloadRegisterA, SIDE_A);
	WRCFGA(sideB_payloadRegisterA, SIDE_B);
	HAL_Delay(1);
	WRCFGB(payloadRegisterB, SIDE_A);
	WRCFGB(payloadRegisterB, SIDE_B);

	HAL_TIM_Base_Start_IT(&htim2);
	while (Counter <= 65000) {			// Counter should be adaptive to DCTO
		HAL_Delay(1);		// Temp sense here
	}
	HAL_TIM_Base_Stop_IT(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	Counter = 0;

	for(uint8_t i = 0; i < sideAOddCount; i++) {
		switch(sideAOddCells[i]) {
		case 1:
			DCC1 = 1;
			break;
		case 3:
			DCC3 = 1;
			break;
		case 5:
			DCC5 = 1;
			break;
		case 7:
			DCC7 = 1;
			break;
		case 9:
			DCC9 = 1;
			break;
		default:		// default should NEVER happen. Put proper error handling here later
		}
	}
	sideA_payloadRegisterA[4] = (DCC8 << 7) | (DCC7 << 6) | (DCC6 << 5) | (DCC5 << 4) | (DCC4 << 3) | (DCC3 << 2) | (DCC2 << 1) | (DCC1 << 0);
	sideA_payloadRegisterA[5] = (DCTO << 4) | (DCC10 << 1) | (DCC9 << 0);
	DCC1 = 0, DCC2 = 0, DCC3 = 0, DCC4 = 0, DCC5 = 0, DCC6 = 0, DCC7 = 0, DCC8 = 0, DCC9 = 0, DCC10 = 0;

	for(uint8_t i = 0; i < sideBOddCount; i++) {
		switch(sideBOddCells[i]) {
		case 1:
			DCC1 = 1;
			break;
		case 3:
			DCC3 = 1;
			break;
		case 5:
			DCC5 = 1;
			break;
		case 7:
			DCC7 = 1;
			break;
		case 9:
			DCC9 = 1;
			break;
		default:		// default should NEVER happen. Put proper error handling here later
		}
	}
	sideB_payloadRegisterA[4] = (DCC8 << 7) | (DCC7 << 6) | (DCC6 << 5) | (DCC5 << 4) | (DCC4 << 3) | (DCC3 << 2) | (DCC2 << 1) | (DCC1 << 0);
	sideB_payloadRegisterA[5] = (DCTO << 4) | (DCC10 << 1) | (DCC9 << 0);
	DCC1 = 0, DCC2 = 0, DCC3 = 0, DCC4 = 0, DCC5 = 0, DCC6 = 0, DCC7 = 0, DCC8 = 0, DCC9 = 0, DCC10 = 0;

	force_refup();
	HAL_Delay(1);

	WRCFGA(sideA_payloadRegisterA, SIDE_A);
	WRCFGA(sideB_payloadRegisterA, SIDE_B);
	HAL_Delay(1);
	WRCFGB(payloadRegisterB, SIDE_A);
	WRCFGB(payloadRegisterB, SIDE_B);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

	HAL_TIM_Base_Start_IT(&htim2);
	while (Counter <= 65000) {			// Counter should be adaptive to DCTO
		HAL_Delay(1);		// Temp sense here
	}
	HAL_TIM_Base_Stop_IT(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	Counter = 0;
}
