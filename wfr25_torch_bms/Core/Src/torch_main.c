#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_ltc6813.h"
#include "torch_diagnostic.h"
#include "torch_can.h"
#include "torch_balance.h"
#include "torch_dependencies.h"

const uint8_t moduleID = 1;
const uint8_t bmsMode = 1;			// A value of 1 puts the BMS in normal operation, any other value locks it in the low power state.
uint8_t forceBalance = 0;			// A value of 1 forces the BMS to balance the cells if necessary. This MUST be 0 for normal operation.

uint16_t transmissionDelay;

volatile uint32_t transmitCounter;
volatile uint32_t measureCounter;
volatile uint32_t balanceCounter;
volatile uint32_t diagnosisCounter;


void torch_main(void)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8];

	uint8_t packStatus = 0;		// Defines the status of the accumulator

	uint16_t cellVoltages[CELL_QTY];				// Holds all cell voltages within a module
	float moduleTemperatures[MODULE_THERM_QTY];		// Holds all module thermistor temperatures

	uint8_t cellsToBalance[CELL_QTY] = {0};
	uint8_t cellsToBalanceQty = 0;

	uint16_t absMinCellVoltage = 0;
	uint8_t balanceMsgReceivedFlag = 0;

	uint8_t overheatFlag = 0;
	uint8_t overheatCount = 0;

	uint8_t overvoltFlag = 0;
	uint8_t overvoltCount = 0;

	uint8_t undervoltFlag = 0;
	uint8_t undervoltCount = 0;

	uint8_t faultCellQty = 0;
	uint8_t faultCells[CELL_QTY + 1] = {0};				// The CELL_QTY + 1 accounts for cell 0
	uint8_t faultThermistorQty = 0;
	uint8_t faultThermistors[MODULE_THERM_QTY] = {0};

	uint8_t measureFlag = 0;

	// Switch statement below defines how often each module will transmit its data over CAN.
	switch(moduleID) {
		case 1:
			transmissionDelay = 1000;			// Module 1's transmission frequency (default: 1000 milliseconds)
			break;
		case 2:
			transmissionDelay = 1025;			// Module 2's transmission frequency (default: 1025 milliseconds)
			break;
		case 3:
			transmissionDelay = 1050;			// Module 3's transmission frequency (default: 1050 milliseconds)
			break;
		case 4:
			transmissionDelay = 1075;			// Module 4's transmission frequency (default: 1075 milliseconds)
			break;
		case 5:
			transmissionDelay = 1100;			// Module 5's transmission frequency (default: 1100 milliseconds)
			break;
	}

	if(bmsMode == 1) {
		ltc6820_awaken();

		HAL_CAN_Start(&hcan1);
		start_timer(&htim2);
		setup_PEC15();

		active_led_on();

		force_mute();
		wait(1);
		force_refup();
		wait(1);
		full_diagnosis();
		wait(1);

		transmitCounter = 0;
		measureCounter = 0;
		diagnosisCounter = 0;
		while(1) {

			// Measuring interrupt: reads cell voltages and module temperatures every 100 ms.
			if(measureCounter > MEASURE_INTERVAL) {
				// We first want to ensure that 6813 is in REFUP state. If it's not, we force it to enter it.
				if(!refup_check()) {
					force_refup();
					wait(1);
				}

				temperature_sense(moduleTemperatures);			// Get all 18 temperature sensors
				voltage_sense(cellVoltages);					// Get all 20 cell voltages

				// For-loop below checks if any thermistor's >60 C.
				for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) {
					if(moduleTemperatures[i] > MAX_TEMPERATURE) {
						overheatFlag = 1;
						faultThermistors[faultThermistorQty] = i + 1;
						faultThermistorQty++;
					}
				}
				// If at least one sensor read >60 C, the overheat counter increases by one.
				if(overheatFlag) { overheatCount++; }

				// If all sensors read <60 C, we decrease the overheat counter IF it has already been incremented before.
				else {
					if(overheatCount > 0) { overheatCount--; }

					for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) { faultThermistors[i] = 0; }

					faultThermistorQty = 0;
				}

				// If at least one sensor has been >60 C for more than 10 interpretations, then it's time to fault!
				if(overheatCount > ATTEMPT_LIMIT) { error_loop(ERROR_OVERHEAT, faultThermistors, faultCells, NOT_APPLICABLE); }

				// For-loop below checks if any cell voltage is out of bounds. Like before, the voltage and index are noted if it finds one.
				for(uint8_t i = 0; i < CELL_QTY; i++) {
					if(cellVoltages[i] > MAX_CELL_VOLTAGE) {
						overvoltFlag = 1;
						faultCells[faultCellQty] = i + 1;
						faultCellQty++;
					}
					else if(cellVoltages[i] < MIN_CELL_VOLTAGE) {
						undervoltFlag = 1;
						faultCells[faultCellQty] = i + 1;
						faultCellQty++;
					}
				}
				// If at least one cell's above 4.2 V, the overvolt counter increases by one.
				if(overvoltFlag) { overvoltCount++; }

				// If at least one cell's below 2.5 V, the undervolt counter increases by one.
				else if(undervoltFlag) { undervoltCount++; }

				else {
					if(overvoltCount > 0) { overvoltCount--; }

					if(undervoltCount > 0) { undervoltCount--; }

					for(uint8_t i = 0; i < CELL_QTY + 1; i++) { faultCells[i] = 0; }

					faultCellQty = 0;
				}

				// If at least one cell is >4.2 V or <2.5 V for more than 10 interpretations, then it's time to fault!
				if(overvoltCount > ATTEMPT_LIMIT) { error_loop(ERROR_OVERVOLT, faultThermistors, faultCells, NOT_APPLICABLE); }

				if(undervoltCount > ATTEMPT_LIMIT) { error_loop(ERROR_UNDERVOLT, faultThermistors, faultCells, NOT_APPLICABLE); }

				// Reset the three status flags to 0 before starting a new measurement cycle.
				overheatFlag = 0;
				overvoltFlag = 0;
				undervoltFlag = 0;

				measureCounter = 0;		// Reset the measurement counter back to 0.
				measureFlag = 1;		// Signifies that this module has completed at least one measurement.
			}

			// Transmission interrupt: Broadcast cell voltages and module temperatures over CAN every X ms (X depends on the module ID).
			if(transmitCounter > transmissionDelay) {
				// Two functions below package the data and send the CAN messages.
				transmit_voltages(cellVoltages);
				transmit_temperatures(moduleTemperatures);

				transmitCounter = 0;	// Reset the transmission counter back to 0.
			}

			if(diagnosisCounter > QUICK_DIAGNOSIS_INTERVAL) {
				quick_diagnosis(packStatus);
				diagnosisCounter = 0;	// Reset diagnosis counter back to 0.
			}

			// CAN FIFO0 filter poll: Check if a fault message or absolute minimum cell voltage message have been received.
			if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
				uint8_t attempts = 0;

				// While-loop tries extracting and interpreting the CAN message up to 10 times.
				while(attempts < ATTEMPT_LIMIT) {
					// If-block below executes if the CAN message has been successfully interpreted.
					if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {

						// Enter fault loop if motherboard OR another module are sending a fault message.
						if(RxHeader.StdId == CAN_FAULT_ID) { silent_error_loop(); }

						// Retrieves the minimum cell voltage in the entire pack (modules 2 to 5).
						if(RxHeader.StdId == CAN_MIN_VCELL_ID) { absMinCellVoltage = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8); }

						// Message that orders the BMS to start balancing.
						if(RxHeader.StdId == CAN_START_BALANCE_ID) {
							if(packStatus == PACK_STATUS_IDLE) {
								balanceMsgReceivedFlag = 73;
							}
						}

						// Modules 2 - 4 will find their local minimum cell voltages and transmit it onto the CAN bus.
						if(RxHeader.StdId == CAN_EXTRACT_VMIN_ID && moduleID != 1 && measureFlag) {
							uint16_t localMinCellVoltage = cellVoltages[0];

							for(uint8_t i = 0; i < CELL_QTY; i++) {
								if(cellVoltages[i] < localMinCellVoltage) {
									localMinCellVoltage = cellVoltages[i];
								}
							}
							transmit_vmin(localMinCellVoltage);
						}

						attempts = 13;
					}
					// If there's a problem with reading the CAN message, the 'attempts' counter will increment.
					else {
						attempts++;
						wait(5);
					}
				}
				// If the STM32 fails to read the CAN message 10 times in a row, fault!
				if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0, MCU_FAULT); }
			}

			// CAN FIFO1 filter poll: Check if the pack status message has been received.
			if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO1) > 0) {
				uint8_t attempts = 0;

				// While-loop tries extracting and interpreting the CAN message up to 10 times.
				while(attempts < ATTEMPT_LIMIT) {
					// If-block below executes if the CAN message has been successfully interpreted.
					if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) {
						// If-block below checks if the message is the PackStatus message.
						if(RxHeader.StdId == CAN_PACK_STATUS_ID) {
							switch(RxData[PACK_STATUS_BYTE_POSITION]) {
								case PACK_STATUS_CHARGING:
									charge_led_on();
									active_led_off();
									break;
								case PACK_STATUS_FAULT:
									silent_error_loop();
									break;
								default:
									active_led_on();
									charge_led_off();
									break;
							}
							packStatus = RxData[PACK_STATUS_BYTE_POSITION];
						}
						attempts = 13;
					}
					// If there's a problem with reading the CAN message, the 'attempts' counter will increment.
					else {
						attempts++;
						wait(5);
					}
				}
				// If the STM32 fails to read the CAN message 10 times in a row, fault!
				if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0, MCU_FAULT); }
			}

			// Module 1 starts to get the minimum cell voltage in the pack to initiate balancing.
			if(balanceMsgReceivedFlag == 73 && moduleID == 1) {
				extract_min_cell_voltage(&absMinCellVoltage);

				transmitCounter = 0;
				measureCounter = 0;
				balanceCounter = 0;
				while(balanceCounter < BALANCE_COMMAND_DURATION) {
					if(measureCounter > MEASURE_INTERVAL) {
						if(!refup_check()) {
							force_refup();
							wait(1);
						}

						temperature_sense(moduleTemperatures);
						voltage_sense(cellVoltages);

						measureCounter = 0;
					}

					if(transmitCounter > transmissionDelay) {
						transmit_voltages(cellVoltages);
						transmit_temperatures(moduleTemperatures);
						transmit_balance_initiation(absMinCellVoltage);

						transmitCounter = 0;
					}
					wait(1);
				}
			}

			// Balance check poll.
			if((absMinCellVoltage > MIN_CELL_VOLTAGE || forceBalance) && packStatus == PACK_STATUS_IDLE && (balanceMsgReceivedFlag == 73 || forceBalance)) {

				if(forceBalance && moduleID == 1) {
					extract_min_cell_voltage(&absMinCellVoltage);

					transmitCounter = 0;
					measureCounter = 0;
					balanceCounter = 0;
					while(balanceCounter < BALANCE_COMMAND_DURATION) {
						if(measureCounter > MEASURE_INTERVAL) {
							if(!refup_check()) {
								force_refup();
								wait(1);
							}

							temperature_sense(moduleTemperatures);
							voltage_sense(cellVoltages);

							measureCounter = 0;
						}

						if(transmitCounter > transmissionDelay) {
							transmit_voltages(cellVoltages);
							transmit_temperatures(moduleTemperatures);
							transmit_balance_initiation(absMinCellVoltage);

							transmitCounter = 0;
						}
						wait(1);
					}
				}

				// Check that 6813 is in REFUP state. If it's not, we force it to enter it.
				if(!refup_check()) {
					force_refup();
					wait(1);
				}
				// Get a fresh set of cell voltages. This'll be used to determine which cells need to be balanced.
				voltage_sense(cellVoltages);

				// Check which cells need to be balanced. Quantity and cell indexes are recorded.
				cellsToBalanceQty = balance_check(cellsToBalance, cellVoltages, absMinCellVoltage);

				// If-block below executes if at least one cell needs to be balanced.
				if(cellsToBalanceQty > 0) {
					uint8_t balanceFlag = 1;
					uint8_t haltBalanceFlag = 73;

					// This while-loop is the balancing loop. The BMS will stay here as long as the cells remain out of balance.
					while(balanceFlag && haltBalanceFlag == 73) {

						balance_cycle(cellsToBalance, cellsToBalanceQty, absMinCellVoltage, &haltBalanceFlag);

						force_refup();
						wait(1);
						voltage_sense(cellVoltages);

						cellsToBalanceQty = 0;
						for(uint8_t i = 0; i < CELL_QTY; i++) { cellsToBalance[i] = 0; }

						cellsToBalanceQty = balance_check(cellsToBalance, cellVoltages, absMinCellVoltage);

						if(cellsToBalanceQty == 0) { balanceFlag = 0; }
					}
					if(haltBalanceFlag == 73) { low_power_state(); }
				}
				absMinCellVoltage = 0;
				balanceMsgReceivedFlag = 0;
				forceBalance = 0;
			}
			wait(1);
		}
	}

	else { low_power_state(); }
}
