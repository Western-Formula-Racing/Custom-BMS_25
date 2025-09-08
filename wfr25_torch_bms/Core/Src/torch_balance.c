#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_balance.h"
#include "torch_ltc6813.h"
#include "torch_temperature.h"
#include "torch_can.h"
#include "torch_config.h"
#include <math.h>


void resistor_temperature_sense(float *pcbTemperatures)
{
	uint8_t attempts = 0;

	uint8_t sideA_auxRegisterA[8];
	uint8_t sideA_auxRegisterB[8];
	uint8_t sideA_auxRegisterC[8];
	uint8_t sideA_auxRegisterD[8];

	uint8_t sideB_auxRegisterA[8];
	uint8_t sideB_auxRegisterB[8];
	uint8_t sideB_auxRegisterC[8];
	uint8_t sideB_auxRegisterD[8];

	uint8_t sideA_auxRegisterA_PECflag;
	uint8_t sideA_auxRegisterB_PECflag;
	uint8_t sideA_auxRegisterC_PECflag;
	uint8_t sideA_auxRegisterD_PECflag;

	uint8_t sideB_auxRegisterA_PECflag;
	uint8_t sideB_auxRegisterB_PECflag;
	uint8_t sideB_auxRegisterC_PECflag;
	uint8_t sideB_auxRegisterD_PECflag;

	float sideA_VREF2;
	float sideB_VREF2;

	float sideA_boardThermistorVoltages[9];
	float sideB_boardThermistorVoltages[9];

	float sideA_temperatures[9];
	float sideB_temperatures[9];

	while(attempts < ATTEMPT_LIMIT) {
		CLRAUX(SIDE_A);
		CLRAUX(SIDE_B);
		wait(3);

		ADAXD(SIDE_A);
		ADAXD(SIDE_B);
		wait(3);

		RDAUXA(sideA_auxRegisterA, SIDE_A);
		RDAUXA(sideB_auxRegisterA, SIDE_B);
		wait(1);
		RDAUXB(sideA_auxRegisterB, SIDE_A);
		RDAUXB(sideB_auxRegisterB, SIDE_B);
		wait(1);
		RDAUXC(sideA_auxRegisterC, SIDE_A);
		RDAUXC(sideB_auxRegisterC, SIDE_B);
		wait(1);
		RDAUXD(sideA_auxRegisterD, SIDE_A);
		RDAUXD(sideB_auxRegisterD, SIDE_B);

		sideA_auxRegisterA_PECflag = verify_PEC15(sideA_auxRegisterA);
		sideA_auxRegisterB_PECflag = verify_PEC15(sideA_auxRegisterB);
		sideA_auxRegisterC_PECflag = verify_PEC15(sideA_auxRegisterC);
		sideA_auxRegisterD_PECflag = verify_PEC15(sideA_auxRegisterD);

		sideB_auxRegisterA_PECflag = verify_PEC15(sideB_auxRegisterA);
		sideB_auxRegisterB_PECflag = verify_PEC15(sideB_auxRegisterB);
		sideB_auxRegisterC_PECflag = verify_PEC15(sideB_auxRegisterC);
		sideB_auxRegisterD_PECflag = verify_PEC15(sideB_auxRegisterD);

		if(sideA_auxRegisterA_PECflag == 2 &&
		   sideA_auxRegisterB_PECflag == 2 &&
		   sideA_auxRegisterC_PECflag == 2 &&
		   sideA_auxRegisterD_PECflag == 2 &&
		   sideB_auxRegisterA_PECflag == 2 &&
		   sideB_auxRegisterB_PECflag == 2 &&
		   sideB_auxRegisterC_PECflag == 2 &&
		   sideB_auxRegisterD_PECflag == 2 &&
		   sideA_auxRegisterA[1] != 0xFF &&
		   sideA_auxRegisterA[3] != 0xFF &&
		   sideA_auxRegisterA[5] != 0xFF &&
		   sideA_auxRegisterB[1] != 0xFF &&
		   sideA_auxRegisterB[3] != 0xFF &&
		   sideA_auxRegisterB[5] != 0xFF &&
		   sideA_auxRegisterC[1] != 0xFF &&
		   sideA_auxRegisterC[3] != 0xFF &&
		   sideA_auxRegisterC[5] != 0xFF &&
		   sideA_auxRegisterD[1] != 0xFF &&
		   sideB_auxRegisterA[1] != 0xFF &&
		   sideB_auxRegisterA[3] != 0xFF &&
		   sideB_auxRegisterA[5] != 0xFF &&
		   sideB_auxRegisterB[1] != 0xFF &&
		   sideB_auxRegisterB[3] != 0xFF &&
		   sideB_auxRegisterB[5] != 0xFF &&
		   sideB_auxRegisterC[1] != 0xFF &&
		   sideB_auxRegisterC[3] != 0xFF &&
		   sideB_auxRegisterC[5] != 0xFF &&
		   sideB_auxRegisterD[1] != 0xFF)
		{
			attempts = 13;
		}
		else {
			attempts++;
			wait(1);
		}
	}
	if(attempts != 13) { error_loop(ERROR_PEC, 0, 0); }

	sideA_boardThermistorVoltages[0] = ((sideA_auxRegisterA[1] << 8) | sideA_auxRegisterA[0]) / 10000.0f;
	sideA_boardThermistorVoltages[1] = ((sideA_auxRegisterA[3] << 8) | sideA_auxRegisterA[2]) / 10000.0f;
	sideA_boardThermistorVoltages[2] = ((sideA_auxRegisterA[5] << 8) | sideA_auxRegisterA[4]) / 10000.0f;
	sideA_boardThermistorVoltages[3] = ((sideA_auxRegisterB[1] << 8) | sideA_auxRegisterB[0]) / 10000.0f;
	sideA_boardThermistorVoltages[4] = ((sideA_auxRegisterB[3] << 8) | sideA_auxRegisterB[2]) / 10000.0f;
	sideA_boardThermistorVoltages[5] = ((sideA_auxRegisterC[1] << 8) | sideA_auxRegisterC[0]) / 10000.0f;
	sideA_boardThermistorVoltages[6] = ((sideA_auxRegisterC[3] << 8) | sideA_auxRegisterC[2]) / 10000.0f;
	sideA_boardThermistorVoltages[7] = ((sideA_auxRegisterC[5] << 8) | sideA_auxRegisterC[4]) / 10000.0f;
	sideA_boardThermistorVoltages[8] = ((sideA_auxRegisterD[1] << 8) | sideA_auxRegisterD[0]) / 10000.0f;

	sideB_boardThermistorVoltages[0] = ((sideB_auxRegisterA[1] << 8) | sideB_auxRegisterA[0]) / 10000.0f;
	sideB_boardThermistorVoltages[1] = ((sideB_auxRegisterA[3] << 8) | sideB_auxRegisterA[2]) / 10000.0f;
	sideB_boardThermistorVoltages[2] = ((sideB_auxRegisterA[5] << 8) | sideB_auxRegisterA[4]) / 10000.0f;
	sideB_boardThermistorVoltages[3] = ((sideB_auxRegisterB[1] << 8) | sideB_auxRegisterB[0]) / 10000.0f;
	sideB_boardThermistorVoltages[4] = ((sideB_auxRegisterB[3] << 8) | sideB_auxRegisterB[2]) / 10000.0f;
	sideB_boardThermistorVoltages[5] = ((sideB_auxRegisterC[1] << 8) | sideB_auxRegisterC[0]) / 10000.0f;
	sideB_boardThermistorVoltages[6] = ((sideB_auxRegisterC[3] << 8) | sideB_auxRegisterC[2]) / 10000.0f;
	sideB_boardThermistorVoltages[7] = ((sideB_auxRegisterC[5] << 8) | sideB_auxRegisterC[4]) / 10000.0f;
	sideB_boardThermistorVoltages[8] = ((sideB_auxRegisterD[1] << 8) | sideB_auxRegisterD[0]) / 10000.0f;

	sideA_VREF2 = ((sideA_auxRegisterB[5] << 8) | sideA_auxRegisterB[4]) / 10000.0f;
	sideB_VREF2 = ((sideB_auxRegisterB[5] << 8) | sideB_auxRegisterB[4]) / 10000.0f;

	board_temperature_sense(sideA_boardThermistorVoltages, sideA_VREF2, sideA_temperatures);
	board_temperature_sense(sideB_boardThermistorVoltages, sideB_VREF2, sideB_temperatures);

	for(uint8_t i = 0; i < 9; i++) { *(pcbTemperatures + i) = sideA_temperatures[i]; }

	for(uint8_t i = 9; i < 18; i++) { *(pcbTemperatures + i) = sideB_temperatures[i]; }
}


uint8_t balance_check(uint8_t *cellsToBalance, uint16_t *cellVoltages, uint16_t minCellVoltage)
{
	uint8_t cellsToBalanceQty = 0;

	// For loop below finds all cells that have a delta greater than the maximum delta
	for(uint8_t i = 0; i < CELL_QTY; i++) {
		if(*(cellVoltages + i) > minCellVoltage + MAX_DELTA) {
			*(cellsToBalance + cellsToBalanceQty) = i + 1;
			cellsToBalanceQty++;
		}
	}

	// For loop below sorts the cells that need to balanced (cellsToBalance array) in descending order
    for (uint8_t i = 0; i < cellsToBalanceQty - 1; i++) {
        for (uint8_t j = 0; j < cellsToBalanceQty - i - 1; j++) {
            if (*(cellsToBalance + j) < *(cellsToBalance + j + 1)) {
                uint8_t dummy = *(cellsToBalance + j);
                *(cellsToBalance + j) = *(cellsToBalance + j + 1);
                *(cellsToBalance + j + 1) = dummy;
            }
        }
    }
    return cellsToBalanceQty;
}


void extract_min_cell_voltage(uint16_t *absMinCellVoltage)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8];

	uint16_t cellVoltages[CELL_QTY];
	float moduleTemperatures[MODULE_THERM_QTY];
	uint16_t localMinCellVoltages[5] = {0};

	uint8_t minCellVoltageReceptionFlag = 0;

	uint16_t dummyMinValue;

	transmitCounter = 0;
	measureCounter = 0;
	while(!minCellVoltageReceptionFlag) {
		if(measureCounter > 100) {
			if(!refup_check()) {
				force_refup();
				wait(1);
			}

			temperature_sense(moduleTemperatures);
			voltage_sense(cellVoltages);

			// Check for minimum cell voltage in own cell voltage set
			dummyMinValue = cellVoltages[0];
			for(uint8_t i = 0; i < CELL_QTY; i++) {
				if(cellVoltages[i] < dummyMinValue) {
					dummyMinValue = cellVoltages[i];
				}
			}
			localMinCellVoltages[0] = dummyMinValue;

			measureCounter = 0;
		}

		if(transmitCounter > transmissionDelay) {
			transmit_extract_vmin();
			transmit_voltages(cellVoltages);
			transmit_temperatures(moduleTemperatures);

			transmitCounter = 0;
		}

		if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
			uint8_t attempts = 0;

			while(attempts < ATTEMPT_LIMIT) {
				if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {

					if(RxHeader.StdId == CAN_FAULT_ID) { silent_error_loop(); }

					if(RxHeader.StdId == CAN_M2_VMIN_ID) { localMinCellVoltages[1] = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8); }

					if(RxHeader.StdId == CAN_M3_VMIN_ID) { localMinCellVoltages[2] = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8); }

					if(RxHeader.StdId == CAN_M4_VMIN_ID) { localMinCellVoltages[3] = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8); }

					if(RxHeader.StdId == CAN_M5_VMIN_ID) { localMinCellVoltages[4] = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8); }

					attempts = 13;
				}
				// If there's a problem with reading the CAN message, the 'attempts' counter will increment.
				else {
					attempts++;
					wait(5);
				}
			}
			// If the STM32 fails to read the CAN message 10 times in a row, fault!
			if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0); }
		}

		minCellVoltageReceptionFlag = 1;
		for(uint8_t i = 0; i < 5; i++) {
			if(localMinCellVoltages[i] == 0) {
				minCellVoltageReceptionFlag = 0;
			}
		}

		wait(1);
	}

	// Find lowest cell voltage in pack
	dummyMinValue = localMinCellVoltages[0];
	for(uint8_t i = 0; i < 5; i++) {
		if(localMinCellVoltages[i] < dummyMinValue) {
			dummyMinValue = localMinCellVoltages[i];
		}
	}
	*absMinCellVoltage = dummyMinValue;
}


void balance_cycle(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint16_t absMinCellVoltage)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8];

	uint8_t evenCellsFlag = 1;
	uint8_t oddCellsFlag = 1;

	uint8_t evenCellCount = 0;
	uint8_t oddCellCount = 0;

	// For-loop below counts up all the even & odd cells that need to be balanced.
	for(uint8_t i = 0; i < cellsToBalanceQty; i++) {
		if(*(cellsToBalance + i) != 0) {
			if(*(cellsToBalance + i) % 2 == 0) {
				evenCellCount++;
			}
			else {
				oddCellCount++;
			}
		}
	}

	uint8_t evenCells[evenCellCount];
	uint8_t oddCells[oddCellCount];
	uint8_t evenIndex = 0;
	uint8_t oddIndex = 0;

	// For-loop below assigns all even and odd cells in need of balancing to their own arrays.
	for(uint8_t i = 0; i < cellsToBalanceQty; i++) {
		if(*(cellsToBalance + i) != 0) {
			if(*(cellsToBalance + i) % 2 == 0) {
				evenCells[evenIndex++] = *(cellsToBalance + i);
			}
			else {
				oddCells[oddIndex++] = *(cellsToBalance + i);
			}
		}
	}

	uint8_t sideAEvenCount = 0;
	uint8_t sideAOddCount = 0;
	uint8_t sideBEvenCount = 0;
	uint8_t sideBOddCount = 0;

	// For-loop below counts up all the side A even & odd cells.
	for(uint8_t i = 0; i < evenCellCount; i++) {
		if(evenCells[i] > 10) {
			sideBEvenCount++;
		}

		else {
			sideAEvenCount++;
		}
	}
	// For-loop below counts up all the side B even & odd cells.
	for(uint8_t i = 0; i < oddCellCount; i++) {
		if(oddCells[i] > 9) {
			sideBOddCount++;
		}

		else {
			sideAOddCount++;
		}
	}

	uint8_t sideAEvenCells[sideAEvenCount];
	uint8_t sideAOddCells[sideAOddCount];
	uint8_t sideBEvenCells[sideBEvenCount];
	uint8_t sideBOddCells[sideBOddCount];
	uint8_t sideAEvenIndex = 0;
	uint8_t sideAOddIndex = 0;
	uint8_t sideBEvenIndex = 0;
	uint8_t sideBOddIndex = 0;

	// For-loop below sets up the two side A even & odd cells to balance.
	for(uint8_t i = 0; i < evenCellCount; i++) {
		if(evenCells[i] > 10) {
			sideBEvenCells[sideBEvenIndex++] = evenCells[i] - 10;
		}

		else {
			sideAEvenCells[sideAEvenIndex++] = evenCells[i];
		}
	}
	// For-loop below sets up the two side B even & odd cells to balance.
	for(uint8_t i = 0; i < oddCellCount; i++) {
		if(oddCells[i] > 9) {
			sideBOddCells[sideBOddIndex++] = oddCells[i] - 10;
		}

		else {
			sideAOddCells[sideAOddIndex++] = oddCells[i];
		}
	}

	if(evenCellCount == 0) { evenCellsFlag = 0; }

	if(oddCellCount == 0) { oddCellsFlag = 0; }

	uint16_t cellVoltages[CELL_QTY];
	float moduleTemperatures[MODULE_THERM_QTY];
	float pcbTemperatures[PCB_THERM_QTY];

	uint8_t balanceMsg[8];

	uint16_t maxCellVoltage;
	uint16_t minCellVoltage;
	uint16_t cellVoltageDelta;

	float maxResistorTemperature;
	uint16_t intMaxResistorTemperature;
	float tempScale = 100.0f;

	uint8_t overheatFlag = 0;
	uint8_t overheatCount = 0;

	uint8_t faultingThermistorIndex;
	float faultingTemperature;

	uint8_t sideA_payloadRegisterA_evenCells[8];
	uint8_t sideB_payloadRegisterA_evenCells[8];
	uint8_t sideA_payloadRegisterA_oddCells[8];
	uint8_t sideB_payloadRegisterA_oddCells[8];

	uint8_t sideA_payloadRegisterPWM_evenCells[8];
	uint8_t sideB_payloadRegisterPWM_evenCells[8];
	uint8_t sideA_payloadRegisterPWM_oddCells[8];
	uint8_t sideB_payloadRegisterPWM_oddCells[8];

	uint8_t DCTO = 0x2;

	sideA_payloadRegisterA_evenCells[0] = 0xFE;
	sideA_payloadRegisterA_evenCells[1] = 0x00;
	sideA_payloadRegisterA_evenCells[2] = 0x00;
	sideA_payloadRegisterA_evenCells[3] = 0x00;

	sideB_payloadRegisterA_evenCells[0] = 0xFE;
	sideB_payloadRegisterA_evenCells[1] = 0x00;
	sideB_payloadRegisterA_evenCells[2] = 0x00;
	sideB_payloadRegisterA_evenCells[3] = 0x00;

	sideA_payloadRegisterA_oddCells[0] = 0xFE;
	sideA_payloadRegisterA_oddCells[1] = 0x00;
	sideA_payloadRegisterA_oddCells[2] = 0x00;
	sideA_payloadRegisterA_oddCells[3] = 0x00;

	sideB_payloadRegisterA_oddCells[0] = 0xFE;
	sideB_payloadRegisterA_oddCells[1] = 0x00;
	sideB_payloadRegisterA_oddCells[2] = 0x00;
	sideB_payloadRegisterA_oddCells[3] = 0x00;

	config_DCC_bits(sideAEvenCells, sideAEvenCount, sideA_payloadRegisterA_evenCells, DCTO);
	config_DCC_bits(sideAOddCells, sideAOddCount, sideA_payloadRegisterA_oddCells, DCTO);

	config_DCC_bits(sideBEvenCells, sideBEvenCount, sideB_payloadRegisterA_evenCells, DCTO);
	config_DCC_bits(sideBOddCells, sideBOddCount, sideB_payloadRegisterA_oddCells, DCTO);

	if(!refup_check()) {
		force_refup();
		wait(1);
	}
	voltage_sense(cellVoltages);

	maxCellVoltage = cellVoltages[0];
	for(uint8_t i = 0; i < CELL_QTY; i++) {
		if(cellVoltages[i] > maxCellVoltage) {
			maxCellVoltage = cellVoltages[i];
		}
	}
	cellVoltageDelta = maxCellVoltage - absMinCellVoltage;
	balanceMsg[0] = (uint8_t)(cellVoltageDelta & 0xFF);
	balanceMsg[1] = (uint8_t)((cellVoltageDelta >> 8) & 0xFF);
	balanceMsg[7] = cellsToBalanceQty;

	config_PWM_bits(sideAEvenCells, sideAEvenCount, sideA_payloadRegisterPWM_evenCells, cellVoltages, SIDE_A);
	config_PWM_bits(sideAOddCells, sideAOddCount, sideA_payloadRegisterPWM_oddCells, cellVoltages, SIDE_A);

	config_PWM_bits(sideBEvenCells, sideBEvenCount, sideB_payloadRegisterPWM_evenCells, cellVoltages, SIDE_B);
	config_PWM_bits(sideBOddCells, sideBOddCount, sideB_payloadRegisterPWM_oddCells, cellVoltages, SIDE_B);

	force_unmute();
	wait(1);

	if(evenCellsFlag) {
		uint8_t attempts = 0;
		uint8_t subAttempts = 0;

		uint8_t sideA_receptionRegisterPWM[8];
		uint8_t sideB_receptionRegisterPWM[8];

		uint8_t sideA_receptionRegisterPWM_PECflag;
		uint8_t sideB_receptionRegisterPWM_PECflag;

		while(attempts < ATTEMPT_LIMIT) {
			while(subAttempts < ATTEMPT_LIMIT) {
				WRPWM(sideA_payloadRegisterPWM_evenCells, SIDE_A);
				WRPWM(sideB_payloadRegisterPWM_evenCells, SIDE_B);
				wait(1);

				RDPWM(sideA_receptionRegisterPWM, SIDE_A);
				RDPWM(sideB_receptionRegisterPWM, SIDE_B);
				wait(1);

				sideA_receptionRegisterPWM_PECflag = verify_PEC15(sideA_receptionRegisterPWM);
				sideB_receptionRegisterPWM_PECflag = verify_PEC15(sideB_receptionRegisterPWM);

				if(sideA_receptionRegisterPWM_PECflag == 2 &&
				   sideB_receptionRegisterPWM_PECflag == 2)
				{
					subAttempts = 13;
				}
				else {
					subAttempts++;
					wait(1);
				}
			}
			if(subAttempts != 13) { error_loop(ERROR_PEC, 0, 0); }

			uint8_t matchFlag = 1;

			for(uint8_t i = 0; i < 6; i++) {
				if(sideA_receptionRegisterPWM[i] != sideA_payloadRegisterPWM_evenCells[i] || sideB_receptionRegisterPWM[i] != sideB_payloadRegisterPWM_evenCells[i]) {
					matchFlag = 0;
				}
			}
			if(matchFlag) { attempts = 13; }

			else {
				attempts++;
				subAttempts = 0;
				wait(1);
			}
		}
		if(attempts != 13) { error_loop(ERROR_PWM_SETUP, 0, 0); }

		uint8_t sideA_receptionRegisterCFGA[8];
		uint8_t sideB_receptionRegisterCFGA[8];

		uint8_t sideA_receptionRegisterCFGA_PECflag;
		uint8_t sideB_receptionRegisterCFGA_PECflag;

		attempts = 0;
		subAttempts = 0;
		while(attempts < ATTEMPT_LIMIT) {
			while(subAttempts < ATTEMPT_LIMIT) {
				WRCFGA(sideA_payloadRegisterA_evenCells, SIDE_A);
				WRCFGA(sideB_payloadRegisterA_evenCells, SIDE_B);
				wait(1);

				RDCFGA(sideA_receptionRegisterCFGA, SIDE_A);
				RDCFGA(sideB_receptionRegisterCFGA, SIDE_B);
				wait(1);

				sideA_receptionRegisterCFGA_PECflag = verify_PEC15(sideA_receptionRegisterCFGA);
				sideB_receptionRegisterCFGA_PECflag = verify_PEC15(sideB_receptionRegisterCFGA);

				if(sideA_receptionRegisterCFGA_PECflag == 2 &&
				   sideB_receptionRegisterCFGA_PECflag == 2)
				{
					subAttempts = 13;
				}
				else {
					subAttempts++;
					wait(1);
				}
			}
			if(subAttempts != 13) { error_loop(ERROR_PEC, 0, 0); }

			uint8_t matchFlag = 1;

			for(uint8_t i = 4; i < 6; i++) {
				if(sideA_receptionRegisterCFGA[i] != sideA_payloadRegisterA_evenCells[i] || sideB_receptionRegisterCFGA[i] != sideB_payloadRegisterA_evenCells[i]) {
					matchFlag = 0;
				}
			}
			if(matchFlag) { attempts = 13; }

			else {
				attempts++;
				subAttempts = 0;
				wait(1);
			}
		}
		if(attempts != 13) { error_loop(ERROR_BALANCE_INITIATION, 0, 0); }

		config_balance_flags(balanceMsg, evenCells, evenCellCount);
		balance_led_on();
		/*
		WRCFGA(sideA_payloadRegisterA_evenCells, SIDE_A);
		WRCFGA(sideB_payloadRegisterA_evenCells, SIDE_B);
		wait(1);
		*/
	}

	balanceCounter = 0;
	measureCounter = 0;
	transmitCounter = 0;
	while(evenCellsFlag) {
		if(measureCounter > 100) {
			temperature_sense(moduleTemperatures);
			resistor_temperature_sense(pcbTemperatures);

			maxResistorTemperature = 0;

			for(uint8_t i = 0; i < PCB_THERM_QTY; i++) {
				if(!isnan(pcbTemperatures[i])) {
					if(pcbTemperatures[i] > maxResistorTemperature) {
						maxResistorTemperature = pcbTemperatures[i];
					}
				}
			}
			intMaxResistorTemperature = (uint16_t)(maxResistorTemperature*tempScale);
			balanceMsg[2] = (uint8_t)(intMaxResistorTemperature & 0xFF);
			balanceMsg[3] = (uint8_t)((intMaxResistorTemperature >> 8) & 0xFF);

			if(maxResistorTemperature > HOT_LED_THRESHOLD) { hot_led_on(); }

			else { hot_led_off(); }

			if(maxResistorTemperature > RESISTOR_TEMPERATURE_LIMIT) {
				force_refup();
				evenCellsFlag = 0;
			}

			for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) {
				if(moduleTemperatures[i] > MAX_TEMPERATURE) {
					overheatFlag = 1;
					faultingThermistorIndex = i + 1;
					faultingTemperature = moduleTemperatures[i];
				}
			}
			if(overheatFlag) { overheatCount++; }

			else {
				if(overheatCount > 0) {
					overheatCount--;
				}
			}
			// MODULE OVERHEAT FAULT
			if(overheatCount > ATTEMPT_LIMIT) {
				float tempScale = 1000.0f;
				uint16_t intFaultingTemperature = (uint16_t)(faultingTemperature * tempScale);

				force_mute();
				wait(1);
				force_refup();

				error_loop(ERROR_OVERHEAT, intFaultingTemperature, faultingThermistorIndex);
			}

			measureCounter = 0;
		}

		if(transmitCounter > transmissionDelay) {
			transmit_balance(balanceMsg);
			transmit_voltages(cellVoltages);
			transmit_temperatures(moduleTemperatures);

			transmitCounter = 0;
		}

		if(balanceCounter > 60000) {
			force_refup();
			evenCellsFlag = 0;
		}

		wait(1);
	}

	if(oddCellsFlag) {
		uint8_t attempts = 0;
		uint8_t subAttempts = 0;

		uint8_t sideA_receptionRegisterPWM[8];
		uint8_t sideB_receptionRegisterPWM[8];

		uint8_t sideA_receptionRegisterPWM_PECflag;
		uint8_t sideB_receptionRegisterPWM_PECflag;

		while(attempts < ATTEMPT_LIMIT) {
			while(subAttempts < ATTEMPT_LIMIT) {
				WRPWM(sideA_payloadRegisterPWM_oddCells, SIDE_A);
				WRPWM(sideB_payloadRegisterPWM_oddCells, SIDE_B);
				wait(1);

				RDPWM(sideA_receptionRegisterPWM, SIDE_A);
				RDPWM(sideB_receptionRegisterPWM, SIDE_B);
				wait(1);

				sideA_receptionRegisterPWM_PECflag = verify_PEC15(sideA_receptionRegisterPWM);
				sideB_receptionRegisterPWM_PECflag = verify_PEC15(sideB_receptionRegisterPWM);

				if(sideA_receptionRegisterPWM_PECflag == 2 &&
				   sideB_receptionRegisterPWM_PECflag == 2)
				{
					subAttempts = 13;
				}
				else {
					subAttempts++;
					wait(1);
				}
			}
			if(subAttempts != 13) { error_loop(ERROR_PEC, 0, 0); }

			uint8_t matchFlag = 1;

			for(uint8_t i = 0; i < 6; i++) {
				if(sideA_receptionRegisterPWM[i] != sideA_payloadRegisterPWM_oddCells[i] || sideB_receptionRegisterPWM[i] != sideB_payloadRegisterPWM_oddCells[i]) {
					matchFlag = 0;
				}
			}
			if(matchFlag) { attempts = 13; }

			else {
				attempts++;
				subAttempts = 0;
				wait(1);
			}
		}
		if(attempts != 13) { error_loop(ERROR_PWM_SETUP, 0, 0); }

		uint8_t sideA_receptionRegisterCFGA[8];
		uint8_t sideB_receptionRegisterCFGA[8];

		uint8_t sideA_receptionRegisterCFGA_PECflag;
		uint8_t sideB_receptionRegisterCFGA_PECflag;

		attempts = 0;
		subAttempts = 0;
		while(attempts < ATTEMPT_LIMIT) {
			while(subAttempts < ATTEMPT_LIMIT) {
				WRCFGA(sideA_payloadRegisterA_oddCells, SIDE_A);
				WRCFGA(sideB_payloadRegisterA_oddCells, SIDE_B);
				wait(1);

				RDCFGA(sideA_receptionRegisterCFGA, SIDE_A);
				RDCFGA(sideB_receptionRegisterCFGA, SIDE_B);
				wait(1);

				sideA_receptionRegisterCFGA_PECflag = verify_PEC15(sideA_receptionRegisterCFGA);
				sideB_receptionRegisterCFGA_PECflag = verify_PEC15(sideB_receptionRegisterCFGA);

				if(sideA_receptionRegisterCFGA_PECflag == 2 &&
				   sideB_receptionRegisterCFGA_PECflag == 2)
				{
					subAttempts = 13;
				}
				else {
					subAttempts++;
					wait(1);
				}
			}
			if(subAttempts != 13) { error_loop(ERROR_PEC, 0, 0); }

			uint8_t matchFlag = 1;

			for(uint8_t i = 4; i < 6; i++) {
				if(sideA_receptionRegisterCFGA[i] != sideA_payloadRegisterA_oddCells[i] || sideB_receptionRegisterCFGA[i] != sideB_payloadRegisterA_oddCells[i]) {
					matchFlag = 0;
				}
			}
			if(matchFlag) { attempts = 13; }

			else {
				attempts++;
				subAttempts = 0;
				wait(1);
			}
		}
		if(attempts != 13) { error_loop(ERROR_BALANCE_INITIATION, 0, 0); }

		config_balance_flags(balanceMsg, oddCells, oddCellCount);
		balance_led_on();

		/*
		WRCFGA(sideA_payloadRegisterA_oddCells, SIDE_A);
		WRCFGA(sideB_payloadRegisterA_oddCells, SIDE_B);
		wait(1);
		*/
	}

	balanceCounter = 0;
	measureCounter = 0;
	transmitCounter = 0;
	while(oddCellsFlag) {
		if(measureCounter > 100) {
			temperature_sense(moduleTemperatures);
			resistor_temperature_sense(pcbTemperatures);

			maxResistorTemperature = 0;
			for(uint8_t i = 0; i < PCB_THERM_QTY; i++) {
				if(!isnan(pcbTemperatures[i])) {
					if(pcbTemperatures[i] > maxResistorTemperature) {
						maxResistorTemperature = pcbTemperatures[i];
					}
				}
			}
			intMaxResistorTemperature = (uint16_t)(maxResistorTemperature*tempScale);
			balanceMsg[2] = (uint8_t)(intMaxResistorTemperature & 0xFF);
			balanceMsg[3] = (uint8_t)((intMaxResistorTemperature >> 8) & 0xFF);

			if(maxResistorTemperature > HOT_LED_THRESHOLD) { hot_led_on(); }

			else { hot_led_off(); }

			if(maxResistorTemperature > RESISTOR_TEMPERATURE_LIMIT) {
				force_refup();
				oddCellsFlag = 0;
			}

			for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) {
				if(moduleTemperatures[i] > MAX_TEMPERATURE) {
					overheatFlag = 1;
					faultingThermistorIndex = i + 1;
					faultingTemperature = moduleTemperatures[i];
				}
			}

			if(overheatFlag) { overheatCount++; }

			else {
				if(overheatCount > 0) {
					overheatCount--;
				}
			}
			// MODULE OVERHEAT FAULT
			if(overheatCount > ATTEMPT_LIMIT) {
				float tempScale = 1000.0f;
				uint16_t intFaultingTemperature = (uint16_t)(faultingTemperature * tempScale);

				force_mute();
				wait(1);
				force_refup();

				error_loop(ERROR_OVERHEAT, intFaultingTemperature, faultingThermistorIndex);
			}

			measureCounter = 0;
		}

		if(transmitCounter > transmissionDelay) {
			transmit_balance(balanceMsg);
			transmit_voltages(cellVoltages);
			transmit_temperatures(moduleTemperatures);

			transmitCounter = 0;
		}

		if(balanceCounter > 60000) {
			force_refup();
			oddCellsFlag = 0;
		}

		wait(1);
	}

	force_mute();
	balance_led_off();

	// Set all cell balancing flags to zero
	for(uint8_t i = 4; i < 7; i++) { balanceMsg[i] = 0; }

	transientCounter = 0;
	measureCounter = 0;
	transmitCounter = 0;
	while(transientCounter < 20000) {
		if(measureCounter > 100) {
			temperature_sense(moduleTemperatures);
			resistor_temperature_sense(pcbTemperatures);

			maxResistorTemperature = 0;
			for(uint8_t i = 0; i < PCB_THERM_QTY; i++) {
				if(!isnan(pcbTemperatures[i])) {
					if(pcbTemperatures[i] > maxResistorTemperature) {
						maxResistorTemperature = pcbTemperatures[i];
					}
				}
			}
			intMaxResistorTemperature = (uint16_t)(maxResistorTemperature*tempScale);
			balanceMsg[2] = (uint8_t)(intMaxResistorTemperature & 0xFF);
			balanceMsg[3] = (uint8_t)((intMaxResistorTemperature >> 8) & 0xFF);

			if(maxResistorTemperature > HOT_LED_THRESHOLD) { hot_led_on(); }

			else { hot_led_off(); }

			for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) {
				if(moduleTemperatures[i] > MAX_TEMPERATURE) {
					overheatFlag = 1;
					faultingThermistorIndex = i + 1;
					faultingTemperature = moduleTemperatures[i];
				}
			}
			if(overheatFlag) { overheatCount++; }

			else {
				if(overheatCount > 0) {
					overheatCount--;
				}
			}
			// MODULE OVERHEAT FAULT
			if(overheatCount > ATTEMPT_LIMIT) {
				float tempScale = 1000.0f;
				uint16_t intFaultingTemperature = (uint16_t)(faultingTemperature * tempScale);

				error_loop(ERROR_OVERHEAT, intFaultingTemperature, faultingThermistorIndex);
			}

			measureCounter = 0;
		}

		if(transmitCounter > transmissionDelay) {
			transmit_balance(balanceMsg);
			transmit_voltages(cellVoltages);
			transmit_temperatures(moduleTemperatures);

			transmitCounter = 0;
		}

		wait(1);
	}
	hot_led_off();
}


void config_balance_flags(uint8_t *balanceMsg, uint8_t *cellIndexes, uint8_t cellQty)
{
	uint8_t cell1Flag = 0;
	uint8_t cell2Flag = 0;
	uint8_t cell3Flag = 0;
	uint8_t cell4Flag = 0;
	uint8_t cell5Flag = 0;
	uint8_t cell6Flag = 0;
	uint8_t cell7Flag = 0;
	uint8_t cell8Flag = 0;
	uint8_t cell9Flag = 0;
	uint8_t cell10Flag = 0;
	uint8_t cell11Flag = 0;
	uint8_t cell12Flag = 0;
	uint8_t cell13Flag = 0;
	uint8_t cell14Flag = 0;
	uint8_t cell15Flag = 0;
	uint8_t cell16Flag = 0;
	uint8_t cell17Flag = 0;
	uint8_t cell18Flag = 0;
	uint8_t cell19Flag = 0;
	uint8_t cell20Flag = 0;

	for(uint8_t i = 0; i < cellQty; i++) {
		switch(*(cellIndexes + i)) {
			case 1:
				cell1Flag = 1;
				break;
			case 2:
				cell2Flag = 1;
				break;
			case 3:
				cell3Flag = 1;
				break;
			case 4:
				cell4Flag = 1;
				break;
			case 5:
				cell5Flag = 1;
				break;
			case 6:
				cell6Flag = 1;
				break;
			case 7:
				cell7Flag = 1;
				break;
			case 8:
				cell8Flag = 1;
				break;
			case 9:
				cell9Flag = 1;
				break;
			case 10:
				cell10Flag = 1;
				break;
			case 11:
				cell11Flag = 1;
				break;
			case 12:
				cell12Flag = 1;
				break;
			case 13:
				cell13Flag = 1;
				break;
			case 14:
				cell14Flag = 1;
				break;
			case 15:
				cell15Flag = 1;
				break;
			case 16:
				cell16Flag = 1;
				break;
			case 17:
				cell17Flag = 1;
				break;
			case 18:
				cell18Flag = 1;
				break;
			case 19:
				cell19Flag = 1;
				break;
			case 20:
				cell20Flag = 1;
				break;
		}
	}
	*(balanceMsg + 4) = (cell8Flag << 7) | (cell7Flag << 6) | (cell6Flag << 5) | (cell5Flag << 4) | (cell4Flag << 3) | (cell3Flag << 2) | (cell2Flag << 1) | (cell1Flag << 0);
	*(balanceMsg + 5) = (cell16Flag << 7) | (cell15Flag << 6) | (cell14Flag << 5) | (cell13Flag << 4) | (cell12Flag << 3) | (cell11Flag << 2) | (cell10Flag << 1) | (cell9Flag << 0);
	*(balanceMsg + 6) = (cell20Flag << 3) | (cell19Flag << 2) | (cell18Flag << 1) | (cell17Flag << 0);
}


void config_DCC_bits(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint8_t *payloadRegisterA, uint8_t DCTO)
{
	uint8_t DCC1 = 0;
	uint8_t DCC2 = 0;
	uint8_t DCC3 = 0;
	uint8_t DCC4 = 0;
	uint8_t DCC5 = 0;
	uint8_t DCC6 = 0;
	uint8_t DCC7 = 0;
	uint8_t DCC8 = 0;
	uint8_t DCC9 = 0;
	uint8_t DCC10 = 0;
	uint8_t DCC11 = 0;
	uint8_t DCC12 = 0;

	for(uint8_t i = 0; i < cellsToBalanceQty; i++) {
		switch(*(cellsToBalance + i)) {
			case 1:
				DCC1 = 1;
				break;
			case 2:
				DCC2 = 1;
				break;
			case 3:
				DCC3 = 1;
				break;
			case 4:
				DCC4 = 1;
				break;
			case 5:
				DCC5 = 1;
				break;
			case 6:
				DCC6 = 1;
				break;
			case 7:
				DCC7 = 1;
				break;
			case 8:
				DCC8 = 1;
				break;
			case 9:
				DCC9 = 1;
				break;
			case 10:
				DCC10 = 1;
				break;
		}
	}
	*(payloadRegisterA + 4) = (DCC8 << 7) | (DCC7 << 6) | (DCC6 << 5) | (DCC5 << 4) | (DCC4 << 3) | (DCC3 << 2) | (DCC2 << 1) | (DCC1 << 0);
	*(payloadRegisterA + 5) = (DCTO << 4) | (DCC12 << 3) | (DCC11 << 2) | (DCC10 << 1) | (DCC9 << 0);
}


void config_PWM_bits(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty, uint8_t *payloadRegisterPWM, uint16_t *cellVoltages, uint8_t side)
{
	uint8_t PWM1_0 = 1;
	uint8_t PWM1_1 = 1;
	uint8_t PWM1_2 = 1;
	uint8_t PWM1_3 = 1;

	uint8_t PWM2_0 = 1;
	uint8_t PWM2_1 = 1;
	uint8_t PWM2_2 = 1;
	uint8_t PWM2_3 = 1;

	uint8_t PWM3_0 = 1;
	uint8_t PWM3_1 = 1;
	uint8_t PWM3_2 = 1;
	uint8_t PWM3_3 = 1;

	uint8_t PWM4_0 = 1;
	uint8_t PWM4_1 = 1;
	uint8_t PWM4_2 = 1;
	uint8_t PWM4_3 = 1;

	uint8_t PWM5_0 = 1;
	uint8_t PWM5_1 = 1;
	uint8_t PWM5_2 = 1;
	uint8_t PWM5_3 = 1;

	uint8_t PWM6_0 = 1;
	uint8_t PWM6_1 = 1;
	uint8_t PWM6_2 = 1;
	uint8_t PWM6_3 = 1;

	uint8_t PWM7_0 = 1;
	uint8_t PWM7_1 = 1;
	uint8_t PWM7_2 = 1;
	uint8_t PWM7_3 = 1;

	uint8_t PWM8_0 = 1;
	uint8_t PWM8_1 = 1;
	uint8_t PWM8_2 = 1;
	uint8_t PWM8_3 = 1;

	uint8_t PWM9_0 = 1;
	uint8_t PWM9_1 = 1;
	uint8_t PWM9_2 = 1;
	uint8_t PWM9_3 = 1;

	uint8_t PWM10_0 = 1;
	uint8_t PWM10_1 = 1;
	uint8_t PWM10_2 = 1;
	uint8_t PWM10_3 = 1;

	switch(side) {
		case SIDE_A:
			for(uint8_t i = 0; i < cellsToBalanceQty; i++) {
				switch(*(cellsToBalance + i)) {
					case 1:
						if(*cellVoltages < 42000 && *cellVoltages >= 40000) {
							PWM1_1 = 0;
							PWM1_0 = 0;
						}
						else if(*cellVoltages < 40000 && *cellVoltages >= 39000) {
							PWM1_1 = 0;
						}
						else if(*cellVoltages < 39000 && *cellVoltages >= 38000) {
							PWM1_0 = 0;
						}
						break;
					case 2:
						if(*(cellVoltages + 1) < 42000 && *(cellVoltages + 1) >= 40000) {
							PWM2_1 = 0;
							PWM2_0 = 0;
						}
						else if(*(cellVoltages + 1) < 40000 && *(cellVoltages + 1) >= 39000) {
							PWM2_1 = 0;
						}
						else if(*(cellVoltages + 1) < 39000 && *(cellVoltages + 1) >= 38000) {
							PWM2_0 = 0;
						}
						break;
					case 3:
						if(*(cellVoltages + 2) < 42000 && *(cellVoltages + 2) >= 40000) {
							PWM3_1 = 0;
							PWM3_0 = 0;
						}
						else if(*(cellVoltages + 2) < 40000 && *(cellVoltages + 2) >= 39000) {
							PWM3_1 = 0;
						}
						else if(*(cellVoltages + 2) < 39000 && *(cellVoltages + 2) >= 38000) {
							PWM3_0 = 0;
						}
						break;
					case 4:
						if(*(cellVoltages + 3) < 42000 && *(cellVoltages + 3) >= 40000) {
							PWM4_1 = 0;
							PWM4_0 = 0;
						}
						else if(*(cellVoltages + 3) < 40000 && *(cellVoltages + 3) >= 39000) {
							PWM4_1 = 0;
						}
						else if(*(cellVoltages + 3) < 39000 && *(cellVoltages + 3) >= 38000) {
							PWM4_0 = 0;
						}
						break;
					case 5:
						if(*(cellVoltages + 4) < 42000 && *(cellVoltages + 4) >= 40000) {
							PWM5_1 = 0;
							PWM5_0 = 0;
						}
						else if(*(cellVoltages + 4) < 40000 && *(cellVoltages + 4) >= 39000) {
							PWM5_1 = 0;
						}
						else if(*(cellVoltages + 4) < 39000 && *(cellVoltages + 4) >= 38000) {
							PWM5_0 = 0;
						}
						break;
					case 6:
						if(*(cellVoltages + 5) < 42000 && *(cellVoltages + 5) >= 40000) {
							PWM6_1 = 0;
							PWM6_0 = 0;
						}
						else if(*(cellVoltages + 5) < 40000 && *(cellVoltages + 5) >= 39000) {
							PWM6_1 = 0;
						}
						else if(*(cellVoltages + 5) < 39000 && *(cellVoltages + 5) >= 38000) {
							PWM6_0 = 0;
						}
						break;
					case 7:
						if(*(cellVoltages + 6) < 42000 && *(cellVoltages + 6) >= 40000) {
							PWM7_1 = 0;
							PWM7_0 = 0;
						}
						else if(*(cellVoltages + 6) < 40000 && *(cellVoltages + 6) >= 39000) {
							PWM7_1 = 0;
						}
						else if(*(cellVoltages + 6) < 39000 && *(cellVoltages + 6) >= 38000) {
							PWM7_0 = 0;
						}
						break;
					case 8:
						if(*(cellVoltages + 7) < 42000 && *(cellVoltages + 7) >= 40000) {
							PWM8_1 = 0;
							PWM8_0 = 0;
						}
						else if(*(cellVoltages + 7) < 40000 && *(cellVoltages + 7) >= 39000) {
							PWM8_1 = 0;
						}
						else if(*(cellVoltages + 7) < 39000 && *(cellVoltages + 7) >= 38000) {
							PWM8_0 = 0;
						}
						break;
					case 9:
						if(*(cellVoltages + 8) < 42000 && *(cellVoltages + 8) >= 40000) {
							PWM9_1 = 0;
							PWM9_0 = 0;
						}
						else if(*(cellVoltages + 8) < 40000 && *(cellVoltages + 8) >= 39000) {
							PWM9_1 = 0;
						}
						else if(*(cellVoltages + 8) < 39000 && *(cellVoltages + 8) >= 38000) {
							PWM9_0 = 0;
						}
						break;
					case 10:
						if(*(cellVoltages + 9) < 42000 && *(cellVoltages + 9) >= 40000) {
							PWM10_1 = 0;
							PWM10_0 = 0;
						}
						else if(*(cellVoltages + 9) < 40000 && *(cellVoltages + 9) >= 39000) {
							PWM10_1 = 0;
						}
						else if(*(cellVoltages + 9) < 39000 && *(cellVoltages + 9) >= 38000) {
							PWM10_0 = 0;
						}
						break;
				}
			}
			break;
		case SIDE_B:
			for(uint8_t i = 0; i < cellsToBalanceQty; i++) {
				switch(*(cellsToBalance + i)) {
					case 1:
						if(*(cellVoltages + 10) < 42000 && *(cellVoltages + 10) >= 40000) {
							PWM1_1 = 0;
							PWM1_0 = 0;
						}
						else if(*(cellVoltages + 10) < 40000 && *(cellVoltages + 10) >= 39000) {
							PWM1_1 = 0;
						}
						else if(*(cellVoltages + 10) < 39000 && *(cellVoltages + 10) >= 38000) {
							PWM1_0 = 0;
						}
						break;
					case 2:
						if(*(cellVoltages + 11) < 42000 && *(cellVoltages + 11) >= 40000) {
							PWM2_1 = 0;
							PWM2_0 = 0;
						}
						else if(*(cellVoltages + 11) < 40000 && *(cellVoltages + 11) >= 39000) {
							PWM2_1 = 0;
						}
						else if(*(cellVoltages + 11) < 39000 && *(cellVoltages + 11) >= 38000) {
							PWM2_0 = 0;
						}
						break;
					case 3:
						if(*(cellVoltages + 12) < 42000 && *(cellVoltages + 12) >= 40000) {
							PWM3_1 = 0;
							PWM3_0 = 0;
						}
						else if(*(cellVoltages + 12) < 40000 && *(cellVoltages + 12) >= 39000) {
							PWM3_1 = 0;
						}
						else if(*(cellVoltages + 12) < 39000 && *(cellVoltages + 12) >= 38000) {
							PWM3_0 = 0;
						}
						break;
					case 4:
						if(*(cellVoltages + 13) < 42000 && *(cellVoltages + 13) >= 40000) {
							PWM4_1 = 0;
							PWM4_0 = 0;
						}
						else if(*(cellVoltages + 13) < 40000 && *(cellVoltages + 13) >= 39000) {
							PWM4_1 = 0;
						}
						else if(*(cellVoltages + 13) < 39000 && *(cellVoltages + 13) >= 38000) {
							PWM4_0 = 0;
						}
						break;
					case 5:
						if(*(cellVoltages + 14) < 42000 && *(cellVoltages + 14) >= 40000) {
							PWM5_1 = 0;
							PWM5_0 = 0;
						}
						else if(*(cellVoltages + 14) < 40000 && *(cellVoltages + 14) >= 39000) {
							PWM5_1 = 0;
						}
						else if(*(cellVoltages + 14) < 39000 && *(cellVoltages + 14) >= 38000) {
							PWM5_0 = 0;
						}
						break;
					case 6:
						if(*(cellVoltages + 15) < 42000 && *(cellVoltages + 15) >= 40000) {
							PWM6_1 = 0;
							PWM6_0 = 0;
						}
						else if(*(cellVoltages + 15) < 40000 && *(cellVoltages + 15) >= 39000) {
							PWM6_1 = 0;
						}
						else if(*(cellVoltages + 15) < 39000 && *(cellVoltages + 15) >= 38000) {
							PWM6_0 = 0;
						}
						break;
					case 7:
						if(*(cellVoltages + 16) < 42000 && *(cellVoltages + 16) >= 40000) {
							PWM7_1 = 0;
							PWM7_0 = 0;
						}
						else if(*(cellVoltages + 16) < 40000 && *(cellVoltages + 16) >= 39000) {
							PWM7_1 = 0;
						}
						else if(*(cellVoltages + 16) < 39000 && *(cellVoltages + 16) >= 38000) {
							PWM7_0 = 0;
						}
						break;
					case 8:
						if(*(cellVoltages + 17) < 42000 && *(cellVoltages + 17) >= 40000) {
							PWM8_1 = 0;
							PWM8_0 = 0;
						}
						else if(*(cellVoltages + 17) < 40000 && *(cellVoltages + 17) >= 39000) {
							PWM8_1 = 0;
						}
						else if(*(cellVoltages + 17) < 39000 && *(cellVoltages + 17) >= 38000) {
							PWM8_0 = 0;
						}
						break;
					case 9:
						if(*(cellVoltages + 18) < 42000 && *(cellVoltages + 18) >= 40000) {
							PWM9_1 = 0;
							PWM9_0 = 0;
						}
						else if(*(cellVoltages + 18) < 40000 && *(cellVoltages + 18) >= 39000) {
							PWM9_1 = 0;
						}
						else if(*(cellVoltages + 18) < 39000 && *(cellVoltages + 18) >= 38000) {
							PWM9_0 = 0;
						}
						break;
					case 10:
						if(*(cellVoltages + 19) < 42000 && *(cellVoltages + 19) >= 40000) {
							PWM10_1 = 0;
							PWM10_0 = 0;
						}
						else if(*(cellVoltages + 19) < 40000 && *(cellVoltages + 19) >= 39000) {
							PWM10_1 = 0;
						}
						else if(*(cellVoltages + 19) < 39000 && *(cellVoltages + 19) >= 38000) {
							PWM10_0 = 0;
						}
						break;
				}
			}
			break;
	}
	*payloadRegisterPWM = (PWM2_3 << 7) | (PWM2_2 << 6) | (PWM2_1 << 5) | (PWM2_0 << 4) | (PWM1_3 << 3) | (PWM1_2 << 2) | (PWM1_1 <<1) | (PWM1_0 << 0);
	*(payloadRegisterPWM + 1) = (PWM4_3 << 7) | (PWM4_2 << 6) | (PWM4_1 << 5) | (PWM4_0 << 4) | (PWM3_3 << 3) | (PWM3_2 << 2) | (PWM3_1 <<1) | (PWM3_0 << 0);
	*(payloadRegisterPWM + 2) = (PWM6_3 << 7) | (PWM6_2 << 6) | (PWM6_1 << 5) | (PWM6_0 << 4) | (PWM5_3 << 3) | (PWM5_2 << 2) | (PWM5_1 <<1) | (PWM5_0 << 0);
	*(payloadRegisterPWM + 3) = (PWM8_3 << 7) | (PWM8_2 << 6) | (PWM8_1 << 5) | (PWM8_0 << 4) | (PWM7_3 << 3) | (PWM7_2 << 2) | (PWM7_1 <<1) | (PWM7_0 << 0);
	*(payloadRegisterPWM + 4) = (PWM10_3 << 7) | (PWM10_2 << 6) | (PWM10_1 << 5) | (PWM10_0 << 4) | (PWM9_3 << 3) | (PWM9_2 << 2) | (PWM9_1 <<1) | (PWM9_0 << 0);
	*(payloadRegisterPWM + 5) = 0xFF;
}
