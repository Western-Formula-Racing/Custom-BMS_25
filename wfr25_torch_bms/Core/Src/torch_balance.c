#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_balance.h"
#include "torch_ltc6813.h"
#include "torch_temperature.h"


void resistor_temp_sense(void)
{
	uint8_t sideA_auxRegisterA[8];
	uint8_t sideA_auxRegisterB[8];
	uint8_t sideA_auxRegisterC[8];
	uint8_t sideA_auxRegisterD[8];

	uint8_t sideB_auxRegisterA[8];
	uint8_t sideB_auxRegisterB[8];
	uint8_t sideB_auxRegisterC[8];
	uint8_t sideB_auxRegisterD[8];

	float sideA_VREF2;
	float sideB_VREF2;

	float sideA_boardThermistorVoltages[9];
	float sideB_boardThermistorVoltages[9];

	float sideA_temperatures[9];
	float sideB_temperatures[9];

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
}


void cell_sorter(uint8_t *cellsToBalance, uint16_t *cellVoltages)
{
	uint16_t minCellVoltage = *cellVoltages;
	uint8_t minCellVoltageIndex = 0;
	uint8_t cellsToBalanceQty = 0;

	// For loop below finds the minimum cell voltage & its index
	for(uint8_t i = 1; i < CELL_QTY; i++) {
		if(*(cellVoltages + i) < minCellVoltage) {
			minCellVoltage = *(cellVoltages + i);
			minCellVoltageIndex = i;
		}
	}

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
}


void balance_loop(uint8_t *cellsToBalance, uint8_t cellsToBalanceQty)
{
	uint8_t evenCellCount = 0;
	uint8_t oddCellCount = 0;

	// For loop below counts up all the even & odd cells that need to be balanced
	for(uint8_t i = 0; i < cellsToBalanceQty; i ++) {
		if(*(cellsToBalance + i) % 2 == 0) {
			evenCellCount++;
		}

		else {
			oddCellCount++;
		}
	}

	uint8_t evenCells[evenCellCount];
	uint8_t oddCells[oddCellCount];
	uint8_t evenIndex = 0;
	uint8_t oddIndex = 0;

	// For loop below assigns all even and odd cells in need of balancing to their own arrays
	for(uint8_t i = 0; i < cellsToBalanceQty; i++) {
		if(*(cellsToBalance + i) % 2 == 0) {
			evenCells[evenIndex++] = *(cellsToBalance + i);
		}

		else {
			oddCells[oddIndex++] = *(cellsToBalance + i);
		}
	}

	uint8_t sideAEvenCount = 0;
	uint8_t sideAOddCount = 0;
	uint8_t sideBEvenCount = 0;
	uint8_t sideBOddCount = 0;

	// For loop below counts up all the side A even & odd cells
	for(uint8_t i = 0; i < evenCellCount; i++) {
		if(evenCells[i] > 10) {
			sideBEvenCount++;
		}

		else {
			sideAEvenCount++;
		}
	}
	// For loop below counts up all the side B even & odd cells
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

	// For loop below sets up the two side A even & odd cells to balance
	for(uint8_t i = 0; i < evenCellCount; i++) {
		if(evenCells[i] > 10) {
			sideBEvenCells[sideBEvenIndex++] = evenCells[i] - 10;
		}

		else {
			sideAEvenCells[sideAEvenIndex++] = evenCells[i];
		}
	}
	// For loop below sets up the two side B even & odd cells to balance
	for(uint8_t i = 0; i < oddCellCount; i++) {
		if(oddCells[i] > 9) {
			sideBOddCells[sideBOddIndex++] = oddCells[i] - 10;
		}

		else {
			sideAOddCells[sideAOddIndex++] = oddCells[i];
		}
	}
}


void force_balance(uint8_t cell)
{
	uint8_t sideA_payloadRegisterA[8];
	uint8_t sideB_payloadRegisterA[8];
	//uint8_t payloadRegisterB[2];

	sideA_payloadRegisterA[0] = 0xFE;
	sideA_payloadRegisterA[1] = 0x00;
	sideA_payloadRegisterA[2] = 0x00;
	sideA_payloadRegisterA[3] = 0x00;

	sideB_payloadRegisterA[0] = 0xFE;
	sideB_payloadRegisterA[1] = 0x00;
	sideB_payloadRegisterA[2] = 0x00;
	sideB_payloadRegisterA[3] = 0x00;

	//payloadRegisterB[0] = 0x0F;
	//payloadRegisterB[1] = 0x08;

	switch(cell) {
		case 1:
			sideA_payloadRegisterA[4] = 0x01;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 2:
			sideA_payloadRegisterA[4] = 0x02;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 3:
			sideA_payloadRegisterA[4] = 0x04;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 4:
			sideA_payloadRegisterA[4] = 0x08;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 5:
			sideA_payloadRegisterA[4] = 0x10;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 6:
			sideA_payloadRegisterA[4] = 0x20;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 7:
			sideA_payloadRegisterA[4] = 0x40;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 8:
			sideA_payloadRegisterA[4] = 0x80;
			sideA_payloadRegisterA[5] = 0x10;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 9:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x11;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 10:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x12;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x00;
			break;
		case 11:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x01;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 12:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x02;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 13:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x04;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 14:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x08;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 15:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x10;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 16:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x20;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 17:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x40;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 18:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x80;
			sideB_payloadRegisterA[5] = 0x10;
			break;
		case 19:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x11;
			break;
		case 20:
			sideA_payloadRegisterA[4] = 0x00;
			sideA_payloadRegisterA[5] = 0x00;
			sideB_payloadRegisterA[4] = 0x00;
			sideB_payloadRegisterA[5] = 0x12;
			break;
	}

	if(!refup_check()) {
		force_refup();
		wait(1);
	}
	pull_high(GPIOC, GPIO_PIN_8);		// BALANCE LED

	WRCFGA(sideA_payloadRegisterA, SIDE_A);
	WRCFGA(sideB_payloadRegisterA, SIDE_B);
	wait(1);

	// BREAKPOINT HERE
	MUTE(SIDE_A);
	MUTE(SIDE_B);
	wait(1);
	MUTE(SIDE_A);
	MUTE(SIDE_B);

	// ANOTHER BREAKPOINT HERE
	UNMUTE(SIDE_A);
	UNMUTE(SIDE_B);
	wait(1);
	UNMUTE(SIDE_A);
	UNMUTE(SIDE_B);
}
