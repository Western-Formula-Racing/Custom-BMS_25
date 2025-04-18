#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_voltage.h"
#include "torch_temperature.h"
#include "torch_ltc6813.h"
#include "torch_states.h"

uint8_t torchState = 0;


void torch_main(void)
{
	uint8_t undervoltCheckLim = 10;
	uint8_t overvoltCheckLim = 10;
	uint8_t undervoltCheck = 0;
	uint8_t overvoltCheck = 0;
	
	uint8_t overheatCheckLim = 10;
	uint8_t overheatCheck = 0;
	
	uint16_t packCurrent;
	uint8_t chargeCurrentCheckLim = 3;
	uint8_t chargeCurrentCheck = 0;
	
	torchState = 5;	// (DEL) forcing states for dev purposes
	
	// 2 SECOND BUFFER
	start_timer(&htim2);
	while(Counter <= 2000) { wait(1); }
	stop_timer(&htim2);
	
	led_diagnostics();
	
	setup_PEC15();		// Creates CRC lookup table
	
	led_active();
	torchState = 1;
	
	while(1) {
		temperature_check();
		
		cell_voltage_check();
		
		can_buffer_check();
		
		if(canBufferCheck == 1) {
			// 
		}
		
		// !! DEV state !!
		while(BMS_state == 5) {

			// CODE BELOW TESTS READING BACK A REGISTER (ERROR HANDLING)
			// The read-back registers have an identical format to how we transmit data to the LTC
				// i.e. for RDCFGA, index [0] refers to CFGAR0, index [1] refers to CFGAR1, etc.
			/*uint8_t sideA_payload[8];
			uint8_t sideB_payload[8];

			uint8_t sideA_registerGroupA[8];
			uint8_t sideB_registerGroupA[8];
			uint8_t sideA_PECflag;
			uint8_t sideB_PECflag;

			uint8_t sideA_registerGroupB[8];
			uint8_t sideB_registerGroupB[8];
			uint8_t sideA_PECflag2;
			uint8_t sideB_PECflag2;

			sideA_payload[0] = 0xFE;
			sideA_payload[1] = 0x00;
			sideA_payload[2] = 0xAB;
			sideA_payload[3] = 0xCD;
			sideA_payload[4] = 0x00;
			sideA_payload[5] = 0x00;

			sideB_payload[0] = 0xFB;
			sideB_payload[1] = 0xAA;
			sideB_payload[2] = 0xAA;
			sideB_payload[3] = 0xAA;
			sideB_payload[4] = 0x00;
			sideB_payload[5] = 0x00;

			WRCFGA(sideA_payload, SIDE_A);
			WRCFGA(sideB_payload, SIDE_B);
			HAL_Delay(1);

			RDCFGA(sideA_registerGroupA, SIDE_A);
			RDCFGA(sideB_registerGroupA, SIDE_B);
			HAL_Delay(1);

			RDCFGB(sideA_registerGroupB, SIDE_A);
			RDCFGB(sideB_registerGroupB, SIDE_B);

			sideA_PECflag = verify_PEC15(sideA_registerGroupA);
			sideB_PECflag = verify_PEC15(sideB_registerGroupA);
			sideA_PECflag2 = verify_PEC15(sideA_registerGroupB);
			sideB_PECflag2 = verify_PEC15(sideB_registerGroupB);*/

			while (1) {
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
				HAL_Delay(1000);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_Delay(1000);
			}

		}
		// !! ACTIVE state !!
		while(BMS_state == 1) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);		// Turns on ACTIVE LED
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);	// Turns off STANDBY LED (changed to DEBUG on final)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);	// Turns off CHARGE LED
			
			temperature_sense(temperatures);
			
			for(uint8_t i = 0; i < 16; i++) {			// Make this THERM_QTY later
				if(temperatures[i] >= temperatureLimit) {
					overheatCheck += 2;
					
					if(overheatCheck > overheatCheckLim) {
						// send error code CAN with temperature flagged
					}
				}
			}
						
			// get pack current from CAN message
			if(packCurrent < 0) {	// if it's (-)
				chargeCurrentCheck += 1;
			
				if(chargeCurrentCheck > chargeCurrentCheckLim) { BMS_state = 2; }
			}
			
			//approx_cell_resistance(cellResistances, temperatures);		// function that solves R(T) for cells
			
			//approx_voltage_sag(cellVoltageSag, cellResistances, packCurrent);	// function that'll solve voltage sag
			
			voltage_sense(cellVoltages);
			for(uint8_t i = 0; i < CELL_QTY; i++) {
				if(cellVoltages[i] == 69) {
					// run failed cell voltage measurement due to PEC diagnostic function
				}
				cellVoltageRateOfChange[i] = cellVoltages[i] - cellVoltagesPrior[i];
			}
			
			for(uint8_t i = 0; i < CELL_QTY; i++) { cellOCV[i] = cellVoltages[i] + cellVoltageSag[i]; }
			
			for(uint8_t i = 0; i < CELL_QTY; i++) {
				if(cellOCV[i] <= cellUndervoltage) {
					undervoltCheck += 2;
					
					if(undervoltCheck > undervoltCheckLim) {
						// send error CAN code with undervolt cell flagged
					}
				}
				else if(cellOCV[i] >= cellOvervoltage) {	// This should never occur.. cell voltage should only increase during charging, when BMS is in CHARGE
					overvoltCheck += 2;
					
					if(overvoltCheck > overheatCheckLim) {
						// send error CAN code with overvolt cell flagged
					}
				}
			}


			//balance_heat_test();

			while (1) {
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
				HAL_Delay(1000);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_Delay(1000);
			}
		}
		// !! CHARGE state !!
		while(BMS_state == 2) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);		// Turns off ACTIVE LED
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);	// Turns off STANDBY LED (changed to DEBUG on final)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);	// Turns on CHARGE LED

			uint8_t temperatures[THERM_QTY];
			uint16_t cellVoltages[CELL_QTY];
			uint8_t chargingFlag = 1;	// 1 means pack is charging; 0 means charging's done and BMS is balancing

			while(chargingFlag == 1) {
				// This loop runs while the pack is charging. It assumes that VCELL will keep increasing
				// Charging stops (chargingFlag = 2) when a VCELL reaches 4.1 V

				// temp sense
				// add for-loop the checks temps
				voltage_sense(cellVoltages);

				for(uint8_t i = 0; i < CELL_QTY; i++) {
					if(cellVoltages[i] >= 41000) {
						// Transmit CAN message that charging is DONE
							// could be no different than standard DAQ message (cause DAQ has all cell voltages)
						chargingFlag = 2;
					}
				}
				HAL_Delay(1);
				chargingFlag = 2; // DEL THIS
			}
			uint16_t minCellVoltage = cellVoltages[0];
			uint8_t minCellVoltageIndex = 0;
			uint8_t cellsToBalance[CELL_QTY];
			uint8_t cellsToBalanceQty = 0;

			// For loop below finds the minimum cell voltage & its index
			for(uint8_t i = 1; i < CELL_QTY; i++) {
				if(cellVoltages[i] < minCellVoltage) {
					minCellVoltage = cellVoltages[i];
					minCellVoltageIndex = i;
				}
			}

			// For loop below finds all cells that have a delta equal to or greater than 10 mV
			for(uint8_t i = 0; i < CELL_QTY; i++) {
				if(cellVoltages[i] >= minCellVoltage + 100) {
					cellsToBalance[cellsToBalanceQty] = i + 1;
					cellsToBalanceQty++;
				}
			}
			if(cellsToBalanceQty > 0) { chargingFlag = 0; }		// Runs if cells need to be balanced
			else { BMS_state = 1; }		// Cells do not need to be balanced. BMS returns to ACTIVE state

			// While loop for cell balancing
			while(chargingFlag == 0) {
				// Acceptable delta is 10 mV
				// For deltas between 11 mV - 49 mV, discharge for 30 seconds at a time
				// For deltas greater than 50 mV, discharge for 1 minute at a time

				// For loop below sorts the cells that need to balanced (cellsToBalance array) in descending order
			    for (uint8_t i = 0; i < cellsToBalanceQty - 1; i++) {
			        for (uint8_t j = 0; j < cellsToBalanceQty - i - 1; j++) {
			            if (cellsToBalance[j] < cellsToBalance[j + 1]) {
			                uint8_t dummy = cellsToBalance[j];
			                cellsToBalance[j] = cellsToBalance[j + 1];
			                cellsToBalance[j + 1] = dummy;
			            }
			        }
			    }
			    //balance_cells(cellsToBalance, cellsToBalanceQty);

				while (1) {
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
		// !! DEBUG state !!
		while(BMS_state == 3) {

		}
		// !! ERROR state !!
		while(BMS_state == 0) {

		}
	}
}
