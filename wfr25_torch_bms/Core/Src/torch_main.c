#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_ltc6813.h"
#include "torch_diagnostic.h"
#include "torch_can.h"
#include "torch_balance.h"
#include "torch_config.h"

const uint8_t moduleID = 1;
const uint8_t bmsMode = 1;			// A value of 1 puts the BMS in normal operation, while 0 permanently blink
uint8_t forceBalance = 1;			// A value of 1 forces the BMS to balance cells (default: 0)

uint16_t transmissionDelay;

volatile uint32_t transmitCounter;
volatile uint32_t measureCounter;
volatile uint32_t balanceCounter;
volatile uint32_t transientCounter;


void torch_main(void)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8];

	uint16_t cellVoltages[CELL_QTY];				// Holds all cell voltages within a module
	float moduleTemperatures[MODULE_THERM_QTY];		// Holds all module thermistor temperatures

	uint8_t cellsToBalance[CELL_QTY] = {0};
	uint8_t cellsToBalanceQty = 0;

	uint16_t absMinCellVoltage = 0;

	uint8_t overheatFlag = 0;
	uint8_t overheatCount = 0;

	uint8_t overvoltFlag = 0;
	uint8_t overvoltCount = 0;

	uint8_t undervoltFlag = 0;
	uint8_t undervoltCount = 0;

	uint8_t faultingThermistorIndex;
	float faultingTemperature;
	uint8_t faultingCellIndex;
	uint16_t faultingCellVoltage;

	uint8_t measureFlag = 0;

	/*
	switch(moduleID) {
		case 1:
			transmissionDelay = 900;
			break;
		case 2:
			transmissionDelay = 1000;
			break;
		case 3:
			transmissionDelay = 700;
			break;
		case 4:
			transmissionDelay = 1100;
			break;
		case 5:
			transmissionDelay = 800;
			break;
	}
	*/

	switch(moduleID) {
		case 1:
			transmissionDelay = 1000;
			break;
		case 2:
			transmissionDelay = 1025;
			break;
		case 3:
			transmissionDelay = 1050;
			break;
		case 4:
			transmissionDelay = 1075;
			break;
		case 5:
			transmissionDelay = 1100;
			break;
	}

	if(bmsMode == 1) {
		pull_low(GPIOA, GPIO_PIN_4);		// LTC6820 side A !SS
		pull_low(GPIOA, GPIO_PIN_15);		// LTC6820 side B !SS
		pull_high(GPIOC, GPIO_PIN_4);		// LTC6820 side A force EN
		pull_high(GPIOD, GPIO_PIN_2);		// LTC6820 side B force EN

		HAL_CAN_Start(&hcan1);
		start_timer(&htim2);
		setup_PEC15();

		active_led_on();

		force_mute();
		wait(1);
		force_refup();
		wait(1);
		diagnosis();

		transmitCounter = 0;
		measureCounter = 0;
			while(1) {

				// Measuring interrupt: reads cell voltages and module temperatures every 100 ms.
				if(measureCounter > 100) {
					// We first want to ensure that 6813 is in REFUP state. If it's not, we force it to enter it.
					if(!refup_check()) {
						force_refup();
						wait(1);
					}

					temperature_sense(moduleTemperatures);			// Get all 18 temperature sensors
					voltage_sense(cellVoltages);					// Get all 20 cell voltages

					// For-loop below checks if any thermistor's >60 C. If it finds one, its temperature and index are noted.
					for(uint8_t i = 0; i < THERM_QTY; i++) {
						if(moduleTemperatures[i] > MAX_TEMPERATURE) {
							overheatFlag = 1;
							faultingThermistorIndex = i + 1;
							faultingTemperature = moduleTemperatures[i];
						}
					}
					// If at least one sensor read >60 C, the overheat counter increases by one.
					if(overheatFlag) { overheatCount++; }

					// If all sensors read <60 C, we decrease the overheat counter IF it has already been incremented before.
					else {
						if(overheatCount > 0) { overheatCount--; }
					}

					// If at least one sensor has been >60 C for more than 10 interpretations, then it's time to fault!
					if(overheatCount > ATTEMPT_LIMIT) {
						// Two lines below convert the faulting temperature sensor from type 'float' to type 'uint16_t' (required for CAN transmission).
						float tempScale = 1000.0f;
						uint16_t intFaultingTemperature = (uint16_t)(faultingTemperature*tempScale);

						error_loop(ERROR_OVERHEAT, intFaultingTemperature, faultingThermistorIndex);
					}

					// For-loop below checks if any cell voltage is out of bounds. Like before, the voltage and index are noted if it finds one.
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
					// If at least one cell's above 4.2 V, the overvolt counter increases by one.
					if(overvoltFlag) { overvoltCount++; }

					// If at least one cell's below 2.5 V, the undervolt counter increases by one.
					if(undervoltFlag) { undervoltCount++; }

					// If all cell voltages are within appropriate range, we decrease the over/undervolt counters IF they've already been incremented.
					if(overvoltFlag == 0 && undervoltFlag == 0) {
						if(overvoltCount > 0) { overvoltCount--; }

						if(undervoltCount > 0) { undervoltCount--; }
					}

					// If at least one cell is >4.2 V or <2.5 V for more than 10 interpretations, then it's time to fault!
					if(overvoltCount > ATTEMPT_LIMIT) { error_loop(ERROR_OVERVOLT, faultingCellVoltage, faultingCellIndex); }

					if(undervoltCount > ATTEMPT_LIMIT) { error_loop(ERROR_UNDERVOLT, faultingCellVoltage, faultingCellIndex); }

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

					// Reset the transmission counter back to 0.
					transmitCounter = 0;
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

							// Store the absolute minimum cell voltage. This message will initiate balancing.
							if(RxHeader.StdId == CAN_MIN_VCELL_ID) { absMinCellVoltage = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8); }

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
					if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0); }
				}

				// CAN FIFO1 filter poll: Check if the pack status message has been received.
				if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO1) > 0) {
					uint8_t attempts = 0;

					// While-loop tries extracting and interpreting the CAN message up to 10 times.
					while(attempts < ATTEMPT_LIMIT) {
						// If-block below executes if the CAN message has been successfully interpreted.
						if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) {
							// If-block below checks if the message is the PackStatus message.
								// This is a little redundant... since the FIFO1 filter is configured to only accept the PackStatus message anyway.
							if(RxHeader.StdId == CAN_PACK_STAT_ID) {
								switch(RxData[5]) {
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
					if(attempts != 13) { error_loop(ERROR_CAN_READ, 0, 0); }
				}

				// Balance check poll: If there's a value that's >0 for the absolute minimum cell voltage OR force balance is enabled, begin balancing.
				if(absMinCellVoltage > 0 || forceBalance) {

					if(forceBalance && moduleID == 1) {
						extract_min_cell_voltage(&absMinCellVoltage);

						transmitCounter = 0;
						measureCounter = 0;
						transientCounter = 0;
						while(transientCounter < 5000) {
							if(measureCounter > 100) {
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

						// This while-loop is the balancing loop. The BMS will stay here as long as the cells remain out of balance.
						while(balanceFlag) {

							balance_cycle(cellsToBalance, cellsToBalanceQty, absMinCellVoltage);

							force_refup();
							wait(1);
							voltage_sense(cellVoltages);

							cellsToBalanceQty = 0;
							for(uint8_t i = 0; i < CELL_QTY; i++) { cellsToBalance[i] = 0; }

							cellsToBalanceQty = balance_check(cellsToBalance, cellVoltages, absMinCellVoltage);

							if(cellsToBalanceQty == 0) { balanceFlag = 0; }
						}
					}
					absMinCellVoltage = 0;
					forceBalance = 0;
					low_power_state();
				}
				wait(1);
			}
	}

	if(bmsMode == 0) {
		pull_low(GPIOC, GPIO_PIN_4);		// LTC6820 side A force !EN
		pull_low(GPIOD, GPIO_PIN_2);		// LTC6820 side B force !EN

		while(1) {
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
}
