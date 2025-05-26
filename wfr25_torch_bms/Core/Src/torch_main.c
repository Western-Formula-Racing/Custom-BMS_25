#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_ltc6813.h"
#include "torch_can.h"
#include "torch_balance.h"


const uint8_t moduleID = 1;

volatile uint32_t transmitCounter;
volatile uint32_t measureCounter;
volatile uint32_t balanceCounter;
volatile uint32_t muteCounter;
//volatile uint32_t canTimeoutCounter;

uint8_t mode;
uint8_t state;


void torch_main(void)
{
	// mode = 0 is perma blink
	// mode = 3 is NORMAL program
	// mode = 4 is for development
	mode = 3;

	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8];

	uint16_t cellVoltages[CELL_QTY];		// Holds all cell voltages within a module
	float temperatures[THERM_QTY];			// Holds all module thermistor temperatures

	uint8_t cellsToBalance[CELL_QTY] = {0};
	uint8_t cellsToBalanceQty = 0;

	uint16_t packCurrent;
	uint16_t globalMinCellVoltage = 0;			// Holds the minimum cell voltage in the entire pack
	uint16_t localMinCellVoltage = 0;

	uint8_t overheats = 0;
	uint8_t overheatFlag = 0;

	uint8_t overvolts = 0;
	uint8_t overvoltFlag = 0;

	uint8_t undervolts = 0;
	uint8_t undervoltFlag = 0;

	uint8_t doneChargingFlag = 0;		// Status flag that defines how CHARGE state came to an end
		// 0 means pack has not finished charging (default)
		// 1 means motherboard sent BMS_Min_VCELL message
		// 2 means

	uint8_t faultingThermistorIndex;
	float faultingTemperature;
	uint8_t faultingCellIndex;
	uint16_t faultingCellVoltage;


	pull_low(GPIOA, GPIO_PIN_4);		// LTC6820 side A !SS
	pull_low(GPIOA, GPIO_PIN_15);		// LTC6820 side B !SS
	pull_high(GPIOC, GPIO_PIN_4);		// LTC6820 side A force EN
	pull_high(GPIOD, GPIO_PIN_2);		// LTC6820 side B force EN

	// 2 SECOND BUFFER
	/*
	start_timer(&htim2);
	while(Counter <= 2000) {
		wait(1);
	}
	stop_timer(&htim2);
	*/

	setup_PEC15();

	// !! MODE = 3 IS STANDALONE ACTIVE !!
	if(mode == 3) {
		transmitCounter = 0;
		measureCounter = 0;
		state = ACTIVE;						// Change this to CHARGE if you wanna force it to start CHARGE
		pull_high(GPIOA, GPIO_PIN_8);		// TURN ON ACTIVE LED
		HAL_CAN_Start(&hcan1);
		start_timer(&htim2);
		force_refup();
	}
	while(mode == 3) {
		while(state == ACTIVE) {

			if(measureCounter > 100) {
				if(!refup_check()) {
					force_refup();
					wait(1);
				}

				temperature_sense(temperatures);
				voltage_sense(cellVoltages);

				for(uint8_t i = 0; i < THERM_QTY; i++) {
					if(temperatures[i] > MAX_TEMPERATURE) {
						overheatFlag = 1;
						faultingThermistorIndex = i + 1;
						faultingTemperature = temperatures[i];
					}
				}
				if(overheatFlag) { overheats++; }

				else {
					if(overheats > 0) { overheats--; }
				}
				// MODULE OVERHEAT FAULT
				if(overheats > ATTEMPT_LIMIT) {
					float tempScale = 1000.0f;
					uint16_t intFaultingTemperature = (uint16_t)(faultingTemperature * tempScale);

					error_loop(ERROR_OVERHEAT, intFaultingTemperature, faultingThermistorIndex);
				}

				for(uint8_t i = 0; i < CELL_QTY; i++) {
					if(cellVoltages[i] > MAX_CELL_VOLTAGE) {
						overvoltFlag = 1;
						faultingCellIndex = i + 1;
						faultingCellVoltage = cellVoltages[i];
					}
					else if(cellVoltages[i] < MIN_CELL_VOLTAGE) {
						undervoltFlag = 1;
						faultingCellIndex = i + 1;
						faultingCellVoltage = cellVoltages[i];
					}
				}
				if(overvoltFlag) { overvolts++; }

				if(undervoltFlag) { undervolts++; }

				if(overvoltFlag == 0 && undervoltFlag == 0) {
					if(overvolts > 0) { overvolts--; }

					if(undervolts > 0) { undervolts--; }
				}
				// OVERVOLT/UNDERVOLT FAULTS
				if(overvolts > ATTEMPT_LIMIT) {
					error_loop(ERROR_OVERVOLT, faultingCellVoltage, faultingCellIndex);
				}
				if(undervolts > ATTEMPT_LIMIT) {
					error_loop(ERROR_UNDERVOLT, faultingCellVoltage, faultingCellIndex);
				}

				overvoltFlag = 0;
				undervoltFlag = 0;
				measureCounter = 0;
			}

			if(transmitCounter > 1000) {			// Module 5 waits 800 ms. Module 3 waits 700 ms. The rest wait 1000 ms
				//pull_high(GPIOC, GPIO_PIN_9);
				transmit_voltages(cellVoltages);
				transmit_temperatures(temperatures);
				transmitCounter = 0;
				//pull_low(GPIOC, GPIO_PIN_9);
			}

			if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
				uint8_t attempts = 0;

				while(attempts < ATTEMPT_LIMIT) {
					if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {

						if(RxHeader.StdId == CAN_FAULT_ID) { silent_error_loop(); }

						attempts = 13;
					}
					else {
						attempts++;
						wait(5);
					}
				}

				if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0); }
			}
			if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO1) > 0) {
				uint8_t attempts = 0;

				while(attempts < ATTEMPT_LIMIT) {
					if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) {
						if(RxHeader.StdId == CAN_PACK_STAT_ID) {
							packCurrent = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8);
							switch(RxData[5]) {
								case 4:					// CHARGE
									state = CHARGE;
									break;
								case 6:					// Fault
									silent_error_loop();
									break;
								default:				// ACTIVE
									state = ACTIVE;
									break;
							}
						}
						attempts = 13;
					}
					else {
						attempts++;
						wait(5);
					}
				}
				if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0); }
			}

			wait(1);
		}
		if(state == CHARGE) {
			pull_low(GPIOA, GPIO_PIN_8);		// TURN OFF ACTIVE LED
			pull_high(GPIOC, GPIO_PIN_9);		// TURN ON CHARGE LED

			transmitCounter = 0;
			measureCounter = 0;
		}
		// *** BEGIN CHARGE LOOP ***
		while(state == CHARGE) {
			if(measureCounter > 100) {
				if(!refup_check()) {
					force_refup();
					wait(1);
				}

				temperature_sense(temperatures);
				voltage_sense(cellVoltages);

				for(uint8_t i = 0; i < THERM_QTY; i++) {
					if(temperatures[i] > MAX_TEMPERATURE) {
						overheatFlag = 1;
						faultingThermistorIndex = i + 1;
						faultingTemperature = temperatures[i];
					}
				}
				if(overheatFlag) { overheats++; }

				else {
					if(overheats > 0) { overheats--; }
				}
				// MODULE OVERHEAT FAULT
				if(overheats > ATTEMPT_LIMIT) {
					float tempScale = 1000.0f;
					uint16_t intFaultingTemperature = (uint16_t)(faultingTemperature * tempScale);

					error_loop(ERROR_OVERHEAT, intFaultingTemperature, faultingThermistorIndex);
				}

				for(uint8_t i = 0; i < CELL_QTY; i++) {
					if(cellVoltages[i] > MAX_CELL_VOLTAGE) {
						overvoltFlag = 1;
						faultingCellIndex = i + 1;
						faultingCellVoltage = cellVoltages[i];
					}
					else if(cellVoltages[i] < MIN_CELL_VOLTAGE) {
						undervoltFlag = 1;
						faultingCellIndex = i + 1;
						faultingCellVoltage = cellVoltages[i];
					}
				}
				if(overvoltFlag) { overvolts++; }

				if(undervoltFlag) { undervolts++; }

				if(overvoltFlag == 0 && undervoltFlag == 0) {
					if(overvolts > 0) { overvolts--; }

					if(undervolts > 0) { undervolts--; }
				}
				// UNDERVOLT FAULTS

				if(undervolts > ATTEMPT_LIMIT) {
					error_loop(ERROR_UNDERVOLT, faultingCellVoltage, faultingCellIndex);
				}

				// Overvolt means charging's done
				if(overvolts > ATTEMPT_LIMIT) {
					error_loop(ERROR_OVERVOLT, faultingCellVoltage, faultingCellIndex);
				}

				overvoltFlag = 0;
				undervoltFlag = 0;
				measureCounter = 0;
			}

			if(transmitCounter > 1000) {					// Module 5 waits 800 ms. Module 3 waits 700 ms. The rest wait 1000 ms
				transmit_voltages(cellVoltages);
				transmit_temperatures(temperatures);
				transmitCounter = 0;
			}

			if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
				uint8_t attempts = 0;

				while(attempts < ATTEMPT_LIMIT) {
					if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {

						if(RxHeader.StdId == CAN_FAULT_ID) { silent_error_loop(); }

						if(RxHeader.StdId == CAN_MIN_VCELL_ID) { globalMinCellVoltage = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8); }

						attempts = 13;
					}
					else {
						attempts++;
						wait(5);
					}
				}
				if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0); }
			}
			if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO1) > 0) {
				uint8_t attempts = 0;

				while(attempts < ATTEMPT_LIMIT) {
					if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) {
						if(RxHeader.StdId == CAN_PACK_STAT_ID) {
							packCurrent = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8);
							switch(RxData[5]) {
								case 4:					// CHARGE
									state = CHARGE;
									break;
								case 5:
									state = DONE_CHARGE;
									break;
								case 6:					// Fault
									silent_error_loop();
									break;
								default:				// ACTIVE
									//state = ACTIVE;
									doneChargingFlag = 1;
									break;
							}
						}
						attempts = 13;
					}
					else {
						attempts++;
						wait(5);
					}
				}
				if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0); }
			}

			wait(1);
		}
		// *** END CHARGE LOOP ***
		// *** BEGIN CHARGE COMPLETE LOOP ***
		while(state == DONE_CHARGE) {
			stop_timer(&htim2);
			HAL_CAN_Stop(&hcan1);

			while(1) {
				  pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
				  pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
				  pull_high(GPIOC, GPIO_PIN_8);		// BALANCE LED
				  pull_high(GPIOC, GPIO_PIN_7);		// HOT LED
				  wait(500);
				  pull_low(GPIOA, GPIO_PIN_8);
				  pull_low(GPIOC, GPIO_PIN_9);
				  pull_low(GPIOC, GPIO_PIN_8);
				  pull_low(GPIOC, GPIO_PIN_7);
				  wait(500);
			}
		}
		// *** END CHARGE COMPLETE LOOP ***
	}

	// !! MODE = 4 IS BALANCE TEST CODE !!
	if(mode == 4) {
		start_timer(&htim2);
		force_refup();
		wait(1);
		pull_low(GPIOA, GPIO_PIN_8);		// TURN OFF ACTIVE LED
		pull_high(GPIOC, GPIO_PIN_9);		// TURN ON CHARGE LED

		voltage_sense(cellVoltages);

		cellsToBalanceQty = cell_sorter(cellsToBalance, cellVoltages, &localMinCellVoltage);

		manual_balance();
		//manual_overheat_recover();
	}
	stop_timer(&htim2);
	mode = 0;

		/*
		temperature_sense(temperatures);
		voltage_sense(cellVoltages);

		cell_sorter(cellsToBalance, cellVoltages);
		for(uint8_t i = 0; i < CELL_QTY; i++) {
			if(cellsToBalance[i] > 0) { cellsToBalanceQty++; }
		}*/

		// start test
		/*uint16_t sideA_SC;
		uint16_t sideA_VA;
		uint16_t sideA_ITMP;
		uint8_t sideA_statusRegisterGroupA[8];

		uint16_t sideB_SC;
		uint16_t sideB_VA;
		uint16_t sideB_ITMP;
		uint8_t sideB_statusRegisterGroupA[8];

		ADSTAT(SIDE_A);
		ADSTAT(SIDE_B);
		HAL_Delay(2);
		RDSTATA(sideA_statusRegisterGroupA, SIDE_A);
		RDSTATA(sideB_statusRegisterGroupA, SIDE_B);

	    sideA_SC = (sideA_statusRegisterGroupA[1] << 8) | sideA_statusRegisterGroupA[0];
	    sideA_ITMP = (sideA_statusRegisterGroupA[3] << 8) | sideA_statusRegisterGroupA[2];
	    sideA_VA = (sideA_statusRegisterGroupA[5] << 8) | sideA_statusRegisterGroupA[4];

	    sideB_SC = (sideB_statusRegisterGroupA[1] << 8) | sideB_statusRegisterGroupA[0];
	    sideB_ITMP = (sideB_statusRegisterGroupA[3] << 8) | sideB_statusRegisterGroupA[2];
	    sideB_VA = (sideB_statusRegisterGroupA[5] << 8) | sideB_statusRegisterGroupA[4];*/
		// end test


	while(mode == 0) {
		  pull_high(GPIOA, GPIO_PIN_8);		// ACTIVE LED
		  pull_high(GPIOC, GPIO_PIN_9);		// CHARGE LED
		  pull_high(GPIOC, GPIO_PIN_8);		// BALANCE LED
		  pull_high(GPIOC, GPIO_PIN_7);		// HOT LED
		  wait(1000);
		  pull_low(GPIOA, GPIO_PIN_8);
		  pull_low(GPIOC, GPIO_PIN_9);
		  pull_low(GPIOC, GPIO_PIN_8);
		  pull_low(GPIOC, GPIO_PIN_7);
		  wait(1000);
	}
}
