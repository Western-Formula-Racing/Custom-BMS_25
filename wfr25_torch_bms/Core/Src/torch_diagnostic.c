#include "main.h"
#include "torch_diagnostic.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_ltc6813.h"
#include "torch_can.h"
#include "torch_temperature.h"
#include "torch_dependencies.h"
#include <math.h>


void full_diagnosis(void)
{
	// Signify the diagnostic state (turn on all LEDs)
	active_led_on();
	charge_led_on();
	balance_led_on();
	hot_led_on();

	mux_test();
	wait(10);
	cell_register_test();
	wait(10);
	aux_register_test();
	wait(10);
	stat_register_test();
	wait(10);
	overlap_cell_measurement_test();
	wait(10);
	ltc6813_analog_supply_check();
	wait(10);
	ltc6813_digital_supply_check();
	wait(10);
	ltc6813_reference_check();
	wait(10);
	ltc6813_temperature_check();
	wait(10);
	open_cell_check();
	wait(10);
	open_thermistor_check();

	balance_led_off();
	hot_led_off();
	charge_led_off();
}


void quick_diagnosis(uint8_t packStatus)
{
	// Signify the diagnostic state (turn on all LEDs)
	if(packStatus == PACK_STATUS_CHARGING) {
		balance_led_on();
		hot_led_on();
		active_led_on();
	}
	else {
		balance_led_on();
		hot_led_on();
		charge_led_on();
	}

	ltc6813_analog_supply_check();
	wait(10);
	ltc6813_digital_supply_check();
	wait(10);
	overlap_cell_measurement_test();
	wait(10);
	open_cell_check();
	wait(10);
	open_thermistor_check();

	if(packStatus == PACK_STATUS_CHARGING) {
		balance_led_off();
		hot_led_off();
		active_led_off();
	}
	else {
		balance_led_off();
		hot_led_off();
		charge_led_off();
	}
}


void ltc6813_analog_supply_check(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint16_t sideA_VA;
	uint16_t sideB_VA;

	uint8_t sideA_statA[8];
	uint8_t sideB_statA[8];

	uint8_t sideA_statA_PECflag;
	uint8_t sideB_statA_PECflag;

	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRSTAT(SIDE_A);
			CLRSTAT(SIDE_B);
			wait(3);

			ADSTATD(SIDE_A);
			ADSTATD(SIDE_B);
			wait(3);

			RDSTATA(sideA_statA, SIDE_A);
			RDSTATA(sideB_statA, SIDE_B);

			sideA_statA_PECflag = verify_PEC15(sideA_statA);
			sideB_statA_PECflag = verify_PEC15(sideB_statA);

			if(sideA_statA_PECflag == 2) { sideA_subAttempts = 13; }

			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_statA_PECflag == 2) { sideB_subAttempts = 13; }

			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_VA = (sideA_statA[5] << 8) | sideA_statA[4];
		sideB_VA = (sideB_statA[5] << 8) | sideB_statA[4];

		if(sideA_VA >= VA_MIN && sideA_VA <= VA_MAX) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(sideB_VA >= VA_MIN && sideB_VA <= VA_MAX) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_OUT_OF_RANGE_VA, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_OUT_OF_RANGE_VA, 0, 0, SIDE_B); }
}


void ltc6813_digital_supply_check(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint16_t sideA_VD;
	uint16_t sideB_VD;

	uint8_t sideA_statB[8];
	uint8_t sideB_statB[8];

	uint8_t sideA_statB_PECflag;
	uint8_t sideB_statB_PECflag;

	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRSTAT(SIDE_A);
			CLRSTAT(SIDE_B);
			wait(3);

			ADSTATD(SIDE_A);
			ADSTATD(SIDE_B);
			wait(3);

			RDSTATB(sideA_statB, SIDE_A);
			RDSTATB(sideB_statB, SIDE_B);

			sideA_statB_PECflag = verify_PEC15(sideA_statB);
			sideB_statB_PECflag = verify_PEC15(sideB_statB);

			if(sideA_statB_PECflag == 2) { sideA_subAttempts = 13; }

			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_statB_PECflag == 2) { sideB_subAttempts = 13; }

			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_VD = (sideA_statB[1] << 8) | sideA_statB[0];
		sideB_VD = (sideB_statB[1] << 8) | sideB_statB[0];

		if(sideA_VD >= VD_MIN && sideA_VD <= VD_MAX) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(sideB_VD >= VD_MIN && sideB_VD <= VD_MAX) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_OUT_OF_RANGE_VD, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_OUT_OF_RANGE_VD, 0, 0, SIDE_B); }
}


void ltc6813_reference_check(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint16_t sideA_ref2;
	uint16_t sideB_ref2;

	uint8_t sideA_auxB[8];
	uint8_t sideB_auxB[8];

	uint8_t sideA_auxB_PECflag;
	uint8_t sideB_auxB_PECflag;

	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRAUX(SIDE_A);
			CLRAUX(SIDE_B);
			wait(3);

			ADAXD(SIDE_A);
			ADAXD(SIDE_B);
			wait(3);

			RDAUXB(sideA_auxB, SIDE_A);
			RDAUXB(sideB_auxB, SIDE_B);

			sideA_auxB_PECflag = verify_PEC15(sideA_auxB);
			sideB_auxB_PECflag = verify_PEC15(sideB_auxB);

			if(sideA_auxB_PECflag == 2) { sideA_subAttempts = 13; }

			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_auxB_PECflag == 2) { sideB_subAttempts = 13; }

			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_ref2 = (sideA_auxB[5] << 8) | sideA_auxB[4];
		sideB_ref2 = (sideB_auxB[5] << 8) | sideB_auxB[4];

		if(sideA_ref2 >= REF2_MIN && sideA_ref2 <= REF2_MAX) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(sideB_ref2 >= REF2_MIN && sideB_ref2 <= REF2_MAX) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_OUT_OF_RANGE_REF2, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_OUT_OF_RANGE_REF2, 0, 0, SIDE_B); }
}


void ltc6813_temperature_check(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	float sideA_LTC6813Temperature;
	float sideB_LTC6813Temperature;

	uint16_t sideA_ITMP;
	uint16_t sideB_ITMP;

	uint8_t sideA_statA[8];
	uint8_t sideB_statA[8];

	uint8_t sideA_statA_PECflag;
	uint8_t sideB_statA_PECflag;

	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRSTAT(SIDE_A);
			CLRSTAT(SIDE_B);
			wait(3);

			ADSTATD(SIDE_A);
			ADSTATD(SIDE_B);
			wait(3);

			RDSTATA(sideA_statA, SIDE_A);
			RDSTATA(sideB_statA, SIDE_B);

			sideA_statA_PECflag = verify_PEC15(sideA_statA);
			sideB_statA_PECflag = verify_PEC15(sideB_statA);

			if(sideA_statA_PECflag == 2) { sideA_subAttempts = 13; }

			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_statA_PECflag == 2) { sideB_subAttempts = 13; }

			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_ITMP = (sideA_statA[3] << 8) | sideA_statA[2];
		sideB_ITMP = (sideB_statA[3] << 8) | sideB_statA[2];

		// Following 2 lines use LTC6813 internal temperature formula (page 27 on LTC6813 data sheet)
		sideA_LTC6813Temperature = (float)sideA_ITMP*(0.0001f/0.0076f) - 276.0f;
		sideB_LTC6813Temperature = (float)sideB_ITMP*(0.0001f/0.0076f) - 276.0f;

		if(sideA_LTC6813Temperature < LTC6813_TEMPERATURE_LIMIT) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(sideB_LTC6813Temperature < LTC6813_TEMPERATURE_LIMIT) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_LTC6813_OVERHEAT, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_LTC6813_OVERHEAT, 0, 0, SIDE_B); }
}


void open_cell_check(void)
{
	uint8_t attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint8_t faultCellQty = 0;
	uint8_t faultCells[CELL_QTY + 1] = {0};				// The CELL_QTY + 1 accounts for cell 0

	uint16_t sideA_cellPU[12];
	uint16_t sideA_cellPD[12];
	int16_t sideA_cellDelta[12];

	uint16_t sideB_cellPU[12];
	uint16_t sideB_cellPD[12];
	int16_t sideB_cellDelta[12];

	uint8_t sideA_cellVoltageA[8];
	uint8_t sideA_cellVoltageB[8];
	uint8_t sideA_cellVoltageC[8];
	uint8_t sideA_cellVoltageD[8];

	uint8_t sideB_cellVoltageA[8];
	uint8_t sideB_cellVoltageB[8];
	uint8_t sideB_cellVoltageC[8];
	uint8_t sideB_cellVoltageD[8];

	uint8_t sideA_cellVoltageA_PECflag;
	uint8_t sideA_cellVoltageB_PECflag;
	uint8_t sideA_cellVoltageC_PECflag;
	uint8_t sideA_cellVoltageD_PECflag;

	uint8_t sideB_cellVoltageA_PECflag;
	uint8_t sideB_cellVoltageB_PECflag;
	uint8_t sideB_cellVoltageC_PECflag;
	uint8_t sideB_cellVoltageD_PECflag;

	// Step 1
	while(attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			for(uint8_t i = 0; i < ADOW_CONVERSION_RUNS; i++) {
				CLRCELL(SIDE_A);
				CLRCELL(SIDE_B);
				wait(3);

				ADOW(PU, SIDE_A);
				ADOW(PU, SIDE_B);
				wait(3);
			}
			RDCVA(sideA_cellVoltageA, SIDE_A);
			RDCVA(sideB_cellVoltageA, SIDE_B);
			wait(1);
			RDCVB(sideA_cellVoltageB, SIDE_A);
			RDCVB(sideB_cellVoltageB, SIDE_B);
			wait(1);
			RDCVC(sideA_cellVoltageC, SIDE_A);
			RDCVC(sideB_cellVoltageC, SIDE_B);
			wait(1);
			RDCVD(sideA_cellVoltageD, SIDE_A);
			RDCVD(sideB_cellVoltageD, SIDE_B);
			wait(1);

			sideA_cellVoltageA_PECflag = verify_PEC15(sideA_cellVoltageA);
			sideA_cellVoltageB_PECflag = verify_PEC15(sideA_cellVoltageB);
			sideA_cellVoltageC_PECflag = verify_PEC15(sideA_cellVoltageC);
			sideA_cellVoltageD_PECflag = verify_PEC15(sideA_cellVoltageD);

			sideB_cellVoltageA_PECflag = verify_PEC15(sideB_cellVoltageA);
			sideB_cellVoltageB_PECflag = verify_PEC15(sideB_cellVoltageB);
			sideB_cellVoltageC_PECflag = verify_PEC15(sideB_cellVoltageC);
			sideB_cellVoltageD_PECflag = verify_PEC15(sideB_cellVoltageD);

			if(sideA_cellVoltageA_PECflag == 2 &&
			   sideA_cellVoltageB_PECflag == 2 &&
			   sideA_cellVoltageC_PECflag == 2 &&
			   sideA_cellVoltageD_PECflag == 2 &&
			   sideA_cellVoltageA[1] != 0xFF &&
			   sideA_cellVoltageA[3] != 0xFF &&
			   sideA_cellVoltageA[5] != 0xFF &&
			   sideA_cellVoltageB[1] != 0xFF &&
			   sideA_cellVoltageB[3] != 0xFF &&
			   sideA_cellVoltageB[5] != 0xFF &&
			   sideA_cellVoltageC[1] != 0xFF &&
			   sideA_cellVoltageC[3] != 0xFF &&
			   sideA_cellVoltageC[5] != 0xFF &&
			   sideA_cellVoltageD[1] != 0xFF &&
			   sideA_cellVoltageD[3] != 0xFF &&
			   sideA_cellVoltageD[5] != 0xFF)
			{
				sideA_subAttempts = 13;
			}
			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_cellVoltageA_PECflag == 2 &&
			   sideB_cellVoltageB_PECflag == 2 &&
			   sideB_cellVoltageC_PECflag == 2 &&
			   sideB_cellVoltageD_PECflag == 2 &&
			   sideB_cellVoltageA[1] != 0xFF &&
			   sideB_cellVoltageA[3] != 0xFF &&
			   sideB_cellVoltageA[5] != 0xFF &&
			   sideB_cellVoltageB[1] != 0xFF &&
			   sideB_cellVoltageB[3] != 0xFF &&
			   sideB_cellVoltageB[5] != 0xFF &&
			   sideB_cellVoltageC[1] != 0xFF &&
			   sideB_cellVoltageC[3] != 0xFF &&
			   sideB_cellVoltageC[5] != 0xFF &&
			   sideB_cellVoltageD[1] != 0xFF &&
			   sideB_cellVoltageD[3] != 0xFF &&
			   sideB_cellVoltageD[5] != 0xFF)
			{
				sideB_subAttempts = 13;
			}
			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_cellPU[0] = (sideA_cellVoltageA[1] << 8) | sideA_cellVoltageA[0];
		sideA_cellPU[1] = (sideA_cellVoltageA[3] << 8) | sideA_cellVoltageA[2];
		sideA_cellPU[2] = (sideA_cellVoltageA[5] << 8) | sideA_cellVoltageA[4];
		sideA_cellPU[3] = (sideA_cellVoltageB[1] << 8) | sideA_cellVoltageB[0];
		sideA_cellPU[4] = (sideA_cellVoltageB[3] << 8) | sideA_cellVoltageB[2];
		sideA_cellPU[5] = (sideA_cellVoltageB[5] << 8) | sideA_cellVoltageB[4];
		sideA_cellPU[6] = (sideA_cellVoltageC[1] << 8) | sideA_cellVoltageC[0];
		sideA_cellPU[7] = (sideA_cellVoltageC[3] << 8) | sideA_cellVoltageC[2];
		sideA_cellPU[8] = (sideA_cellVoltageC[5] << 8) | sideA_cellVoltageC[4];
		sideA_cellPU[9] = (sideA_cellVoltageD[1] << 8) | sideA_cellVoltageD[0];
		sideA_cellPU[10] = (sideA_cellVoltageD[3] << 8) | sideA_cellVoltageD[2];
		sideA_cellPU[11] = (sideA_cellVoltageD[5] << 8) | sideA_cellVoltageD[4];

		sideB_cellPU[0] = (sideB_cellVoltageA[1] << 8) | sideB_cellVoltageA[0];
		sideB_cellPU[1] = (sideB_cellVoltageA[3] << 8) | sideB_cellVoltageA[2];
		sideB_cellPU[2] = (sideB_cellVoltageA[5] << 8) | sideB_cellVoltageA[4];
		sideB_cellPU[3] = (sideB_cellVoltageB[1] << 8) | sideB_cellVoltageB[0];
		sideB_cellPU[4] = (sideB_cellVoltageB[3] << 8) | sideB_cellVoltageB[2];
		sideB_cellPU[5] = (sideB_cellVoltageB[5] << 8) | sideB_cellVoltageB[4];
		sideB_cellPU[6] = (sideB_cellVoltageC[1] << 8) | sideB_cellVoltageC[0];
		sideB_cellPU[7] = (sideB_cellVoltageC[3] << 8) | sideB_cellVoltageC[2];
		sideB_cellPU[8] = (sideB_cellVoltageC[5] << 8) | sideB_cellVoltageC[4];
		sideB_cellPU[9] = (sideB_cellVoltageD[1] << 8) | sideB_cellVoltageD[0];
		sideB_cellPU[10] = (sideB_cellVoltageD[3] << 8) | sideB_cellVoltageD[2];
		sideB_cellPU[11] = (sideB_cellVoltageD[5] << 8) | sideB_cellVoltageD[4];


		// Step 2
		sideA_subAttempts = 0;
		sideB_subAttempts = 0;
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			for(uint8_t i = 0; i < ADOW_CONVERSION_RUNS; i++) {
				CLRCELL(SIDE_A);
				CLRCELL(SIDE_B);
				wait(3);

				ADOW(PD, SIDE_A);
				ADOW(PD, SIDE_B);
				wait(3);
			}
			RDCVA(sideA_cellVoltageA, SIDE_A);
			RDCVA(sideB_cellVoltageA, SIDE_B);
			wait(1);
			RDCVB(sideA_cellVoltageB, SIDE_A);
			RDCVB(sideB_cellVoltageB, SIDE_B);
			wait(1);
			RDCVC(sideA_cellVoltageC, SIDE_A);
			RDCVC(sideB_cellVoltageC, SIDE_B);
			wait(1);
			RDCVD(sideA_cellVoltageD, SIDE_A);
			RDCVD(sideB_cellVoltageD, SIDE_B);
			wait(1);

			sideA_cellVoltageA_PECflag = verify_PEC15(sideA_cellVoltageA);
			sideA_cellVoltageB_PECflag = verify_PEC15(sideA_cellVoltageB);
			sideA_cellVoltageC_PECflag = verify_PEC15(sideA_cellVoltageC);
			sideA_cellVoltageD_PECflag = verify_PEC15(sideA_cellVoltageD);

			sideB_cellVoltageA_PECflag = verify_PEC15(sideB_cellVoltageA);
			sideB_cellVoltageB_PECflag = verify_PEC15(sideB_cellVoltageB);
			sideB_cellVoltageC_PECflag = verify_PEC15(sideB_cellVoltageC);
			sideB_cellVoltageD_PECflag = verify_PEC15(sideB_cellVoltageD);

			if(sideA_cellVoltageA_PECflag == 2 &&
			   sideA_cellVoltageB_PECflag == 2 &&
			   sideA_cellVoltageC_PECflag == 2 &&
			   sideA_cellVoltageD_PECflag == 2 &&
			   sideA_cellVoltageA[1] != 0xFF &&
			   sideA_cellVoltageA[3] != 0xFF &&
			   sideA_cellVoltageA[5] != 0xFF &&
			   sideA_cellVoltageB[1] != 0xFF &&
			   sideA_cellVoltageB[3] != 0xFF &&
			   sideA_cellVoltageB[5] != 0xFF &&
			   sideA_cellVoltageC[1] != 0xFF &&
			   sideA_cellVoltageC[3] != 0xFF &&
			   sideA_cellVoltageC[5] != 0xFF &&
			   sideA_cellVoltageD[1] != 0xFF &&
			   sideA_cellVoltageD[3] != 0xFF &&
			   sideA_cellVoltageD[5] != 0xFF)
			{
				sideA_subAttempts = 13;
			}
			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_cellVoltageA_PECflag == 2 &&
			   sideB_cellVoltageB_PECflag == 2 &&
			   sideB_cellVoltageC_PECflag == 2 &&
			   sideB_cellVoltageD_PECflag == 2 &&
			   sideB_cellVoltageA[1] != 0xFF &&
			   sideB_cellVoltageA[3] != 0xFF &&
			   sideB_cellVoltageA[5] != 0xFF &&
			   sideB_cellVoltageB[1] != 0xFF &&
			   sideB_cellVoltageB[3] != 0xFF &&
			   sideB_cellVoltageB[5] != 0xFF &&
			   sideB_cellVoltageC[1] != 0xFF &&
			   sideB_cellVoltageC[3] != 0xFF &&
			   sideB_cellVoltageC[5] != 0xFF &&
			   sideB_cellVoltageD[1] != 0xFF &&
			   sideB_cellVoltageD[3] != 0xFF &&
			   sideB_cellVoltageD[5] != 0xFF)
			{
				sideB_subAttempts = 13;
			}
			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_cellPD[0] = (sideA_cellVoltageA[1] << 8) | sideA_cellVoltageA[0];
		sideA_cellPD[1] = (sideA_cellVoltageA[3] << 8) | sideA_cellVoltageA[2];
		sideA_cellPD[2] = (sideA_cellVoltageA[5] << 8) | sideA_cellVoltageA[4];
		sideA_cellPD[3] = (sideA_cellVoltageB[1] << 8) | sideA_cellVoltageB[0];
		sideA_cellPD[4] = (sideA_cellVoltageB[3] << 8) | sideA_cellVoltageB[2];
		sideA_cellPD[5] = (sideA_cellVoltageB[5] << 8) | sideA_cellVoltageB[4];
		sideA_cellPD[6] = (sideA_cellVoltageC[1] << 8) | sideA_cellVoltageC[0];
		sideA_cellPD[7] = (sideA_cellVoltageC[3] << 8) | sideA_cellVoltageC[2];
		sideA_cellPD[8] = (sideA_cellVoltageC[5] << 8) | sideA_cellVoltageC[4];
		sideA_cellPD[9] = (sideA_cellVoltageD[1] << 8) | sideA_cellVoltageD[0];
		sideA_cellPD[10] = (sideA_cellVoltageD[3] << 8) | sideA_cellVoltageD[2];
		sideA_cellPD[11] = (sideA_cellVoltageD[5] << 8) | sideA_cellVoltageD[4];

		sideB_cellPD[0] = (sideB_cellVoltageA[1] << 8) | sideB_cellVoltageA[0];
		sideB_cellPD[1] = (sideB_cellVoltageA[3] << 8) | sideB_cellVoltageA[2];
		sideB_cellPD[2] = (sideB_cellVoltageA[5] << 8) | sideB_cellVoltageA[4];
		sideB_cellPD[3] = (sideB_cellVoltageB[1] << 8) | sideB_cellVoltageB[0];
		sideB_cellPD[4] = (sideB_cellVoltageB[3] << 8) | sideB_cellVoltageB[2];
		sideB_cellPD[5] = (sideB_cellVoltageB[5] << 8) | sideB_cellVoltageB[4];
		sideB_cellPD[6] = (sideB_cellVoltageC[1] << 8) | sideB_cellVoltageC[0];
		sideB_cellPD[7] = (sideB_cellVoltageC[3] << 8) | sideB_cellVoltageC[2];
		sideB_cellPD[8] = (sideB_cellVoltageC[5] << 8) | sideB_cellVoltageC[4];
		sideB_cellPD[9] = (sideB_cellVoltageD[1] << 8) | sideB_cellVoltageD[0];
		sideB_cellPD[10] = (sideB_cellVoltageD[3] << 8) | sideB_cellVoltageD[2];
		sideB_cellPD[11] = (sideB_cellVoltageD[5] << 8) | sideB_cellVoltageD[4];

		// Step 3
		for(uint8_t i = 0; i < 12; i++) { sideA_cellDelta[i] = sideA_cellPU[i] - sideA_cellPD[i]; }

		for(uint8_t i = 0; i < 12; i++) { sideB_cellDelta[i] = sideB_cellPU[i] - sideB_cellPD[i]; }

		// Step 4
		uint8_t openCellIndex = 13;
		uint8_t sideA_openCellFlag = 0;
		uint8_t sideB_openCellFlag = 0;

		// If statement below checks if cell 0 is open
		if(sideA_cellPU[0] == 0) {
			faultCells[faultCellQty] = 21;		// Value of 21 implies that cell 0 is open
			faultCellQty++;
		}

		// If statement below checks if cell 10 (GND of side B LTC6813) is open
		if(sideB_cellPU[0] == 0) {
			faultCells[faultCellQty] = 10;
			faultCellQty++;
		}

		// If statement below checks if cell 10 is open using an alternative method
		if(sideA_cellPD[9] == 0) {
			faultCells[faultCellQty] = 10;
			faultCellQty++;
		}

		// If statement below checks if cell 20 is open
		if(sideB_cellPD[9] == 0) {
			faultCells[faultCellQty] = 20;
			faultCellQty++;
		}

		// Two for-loops below check for open cells that show zero volts (complete disconnect)
		for(uint8_t i = 0; i < CELL_QTY/2; i++) {
			if(sideA_cellPU[i] == 0 || sideA_cellPD[i] == 0) {
				faultCells[faultCellQty] = i + 1;
				faultCellQty++;
			}
		}
		for(uint8_t i = 0; i < CELL_QTY/2; i++) {
			if(sideB_cellPU[i] == 0 || sideB_cellPD[i] == 0) {
				faultCells[faultCellQty] = i + 11;
				faultCellQty++;
			}
		}

		// Two for-loops below check for open cells that do not show zero volts (partial disconnect)
		for(uint8_t i = 1; i < CELL_QTY/2; i++) {
			if(sideA_cellDelta[i] < OPEN_CELL_THRESHOLD) {
				faultCells[faultCellQty] = i;
				faultCellQty++;
			}
		}
		for(uint8_t i = 1; i < CELL_QTY/2; i++) {
			if(sideB_cellDelta[i] < OPEN_CELL_THRESHOLD) {
				faultCells[faultCellQty] = i + 10;
				faultCellQty++;
			}
		}

		if(faultCellQty == 0) { attempts = 13; }

		else {
			attempts++;
			wait(1);
		}
	}
	if(attempts != 13) { error_loop(ERROR_CELL_OPEN, 0, faultCells, NOT_APPLICABLE); }
}


void open_thermistor_check(void)
{
	uint8_t attempts = 0;

	float thermistorVoltages[18];

	uint8_t faultThermistorQty = 0;
	uint8_t faultThermistors[MODULE_THERM_QTY] = {0};

	while(attempts < ATTEMPT_LIMIT) {
		read_thermistors(thermistorVoltages);

		for(uint8_t i = 0; i < MODULE_THERM_QTY; i++) {
			if(thermistorVoltages[i] > THERMISTOR_OPEN_CIRCUIT_THRESHOLD) {
				faultThermistors[faultThermistorQty] = i + 1;
				faultThermistorQty++;
			}
		}

		if(faultThermistorQty == 0) { attempts = 13; }

		else {
			attempts++;
			wait(1);
		}
	}
	if(attempts != 13) { error_loop(ERROR_THERMISTOR_OPEN, faultThermistors, 0, NOT_APPLICABLE); }
}


void mux_test(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint8_t sideA_statusB[8];
	uint8_t sideB_statusB[8];

	uint8_t sideA_statusB_PECflag = 0;
	uint8_t sideB_statusB_PECflag = 0;

	uint8_t sideA_muxFailBit;
	uint8_t sideB_muxFailBit;
	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRSTAT(SIDE_A);
			CLRSTAT(SIDE_B);
			wait(3);

			DIAGN(SIDE_A);
			DIAGN(SIDE_B);
			wait(3);

			RDSTATB(sideA_statusB, SIDE_A);
			RDSTATB(sideB_statusB, SIDE_B);

			sideA_statusB_PECflag = verify_PEC15(sideA_statusB);
			sideB_statusB_PECflag = verify_PEC15(sideB_statusB);

			if(sideA_statusB_PECflag == 2) { sideA_subAttempts = 13; }

			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_statusB_PECflag == 2) { sideB_subAttempts = 13; }

			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_muxFailBit = (sideA_statusB[5] >> 1) & 0x01;
		sideB_muxFailBit = (sideB_statusB[5] >> 1) & 0x01;

		if(sideA_muxFailBit == 0) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(sideB_muxFailBit == 0) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_DIAGN, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_DIAGN, 0, 0, SIDE_B); }
}


void stat_register_test(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint16_t sideA_statContents[4];
	uint16_t sideB_statContents[4];

	uint8_t sideA_statA[8];
	uint8_t sideA_statB[8];

	uint8_t sideB_statA[8];
	uint8_t sideB_statB[8];

	uint8_t sideA_statA_PECflag;
	uint8_t sideA_statB_PECflag;

	uint8_t sideB_statA_PECflag;
	uint8_t sideB_statB_PECflag;

	// Test with first variant of STATST command
	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRSTAT(SIDE_A);
			CLRSTAT(SIDE_B);
			wait(3);

			STATST(1, SIDE_A);
			STATST(1, SIDE_B);
			wait(3);

			RDSTATA(sideA_statA, SIDE_A);
			RDSTATA(sideB_statA, SIDE_B);
			wait(1);
			RDSTATB(sideA_statB, SIDE_A);
			RDSTATB(sideB_statB, SIDE_B);

			sideA_statA_PECflag = verify_PEC15(sideA_statA);
			sideB_statA_PECflag = verify_PEC15(sideB_statA);

			sideA_statB_PECflag = verify_PEC15(sideA_statB);
			sideB_statB_PECflag = verify_PEC15(sideB_statB);

			if(sideA_statA_PECflag == 2 && sideA_statB_PECflag == 2) { sideA_subAttempts = 13; }

			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_statA_PECflag == 2 && sideB_statB_PECflag == 2) { sideB_subAttempts = 13; }

			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_statContents[0] = (sideA_statA[1] << 8) | sideA_statA[0];			// SC value
		sideA_statContents[1] = (sideA_statA[3] << 8) | sideA_statA[2];			// ITMP value
		sideA_statContents[2] = (sideA_statA[5] << 8) | sideA_statA[4];			// VA value
		sideA_statContents[3] = (sideA_statB[1] << 8) | sideA_statB[0];			// VD value

		sideB_statContents[0] = (sideB_statA[1] << 8) | sideB_statA[0];			// SC value
		sideB_statContents[1] = (sideB_statA[3] << 8) | sideB_statA[2];			// ITMP value
		sideB_statContents[2] = (sideB_statA[5] << 8) | sideB_statA[4];			// VA value
		sideB_statContents[3] = (sideB_statB[1] << 8) | sideB_statB[0];			// VD value

		uint8_t sideA_badOutputFlag = 0;
		uint8_t sideB_badOutputFlag = 0;

		for(uint8_t i = 0; i < 4; i++) {
			if(sideA_statContents[i] != 0x9555) { sideA_badOutputFlag = 1; }

			if(sideB_statContents[i] != 0x9555) { sideB_badOutputFlag = 1; }
		}

		if(!sideA_badOutputFlag) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(!sideB_badOutputFlag) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_STATST, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_STATST, 0, 0, SIDE_B); }

	// Test with second variant of STATST command
	sideA_attempts = 0;
	sideB_attempts = 0;
	sideA_subAttempts = 0;
	sideB_subAttempts = 0;
	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRSTAT(SIDE_A);
			CLRSTAT(SIDE_B);
			wait(3);

			STATST(0, SIDE_A);
			STATST(0, SIDE_B);
			wait(3);

			RDSTATA(sideA_statA, SIDE_A);
			RDSTATA(sideB_statA, SIDE_B);
			wait(1);
			RDSTATB(sideA_statB, SIDE_A);
			RDSTATB(sideB_statB, SIDE_B);

			sideA_statA_PECflag = verify_PEC15(sideA_statA);
			sideB_statA_PECflag = verify_PEC15(sideB_statA);

			sideA_statB_PECflag = verify_PEC15(sideA_statB);
			sideB_statB_PECflag = verify_PEC15(sideB_statB);

			if(sideA_statA_PECflag == 2 && sideA_statB_PECflag == 2) { sideA_subAttempts = 13; }

			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_statA_PECflag == 2 && sideB_statB_PECflag == 2) { sideB_subAttempts = 13; }

			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_statContents[0] = (sideA_statA[1] << 8) | sideA_statA[0];			// SC value
		sideA_statContents[1] = (sideA_statA[3] << 8) | sideA_statA[2];			// ITMP value
		sideA_statContents[2] = (sideA_statA[5] << 8) | sideA_statA[4];			// VA value
		sideA_statContents[3] = (sideA_statB[1] << 8) | sideA_statB[0];			// VD value

		sideB_statContents[0] = (sideB_statA[1] << 8) | sideB_statA[0];			// SC value
		sideB_statContents[1] = (sideB_statA[3] << 8) | sideB_statA[2];			// ITMP value
		sideB_statContents[2] = (sideB_statA[5] << 8) | sideB_statA[4];			// VA value
		sideB_statContents[3] = (sideB_statB[1] << 8) | sideB_statB[0];			// VD value

		uint8_t badOutputFlag = 0;
		uint8_t sideA_badOutputFlag = 0;
		uint8_t sideB_badOutputFlag = 0;

		for(uint8_t i = 0; i < 4; i++) {
			if(sideA_statContents[i] != 0x6AAA) { sideA_badOutputFlag = 1; }

			if(sideB_statContents[i] != 0x6AAA) { sideB_badOutputFlag = 1; }
		}

		if(!sideA_badOutputFlag) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(!sideB_badOutputFlag) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_STATST, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_STATST, 0, 0, SIDE_B); }
}


void cell_register_test(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint16_t sideA_cellVoltages[12];
	uint16_t sideB_cellVoltages[12];

	uint8_t sideA_cellVoltageA[8];
	uint8_t sideA_cellVoltageB[8];
	uint8_t sideA_cellVoltageC[8];
	uint8_t sideA_cellVoltageD[8];

	uint8_t sideB_cellVoltageA[8];
	uint8_t sideB_cellVoltageB[8];
	uint8_t sideB_cellVoltageC[8];
	uint8_t sideB_cellVoltageD[8];

	uint8_t sideA_cellVoltageA_PECflag;
	uint8_t sideA_cellVoltageB_PECflag;
	uint8_t sideA_cellVoltageC_PECflag;
	uint8_t sideA_cellVoltageD_PECflag;

	uint8_t sideB_cellVoltageA_PECflag;
	uint8_t sideB_cellVoltageB_PECflag;
	uint8_t sideB_cellVoltageC_PECflag;
	uint8_t sideB_cellVoltageD_PECflag;

	// Test with first variant of CVST command
	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRCELL(SIDE_A);
			CLRCELL(SIDE_B);
			wait(3);

			CVST(1, SIDE_A);
			CVST(1, SIDE_B);
			wait(3);

			RDCVA(sideA_cellVoltageA, SIDE_A);
			RDCVA(sideB_cellVoltageA, SIDE_B);
			wait(1);
			RDCVB(sideA_cellVoltageB, SIDE_A);
			RDCVB(sideB_cellVoltageB, SIDE_B);
			wait(1);
			RDCVC(sideA_cellVoltageC, SIDE_A);
			RDCVC(sideB_cellVoltageC, SIDE_B);
			wait(1);
			RDCVD(sideA_cellVoltageD, SIDE_A);
			RDCVD(sideB_cellVoltageD, SIDE_B);

			sideA_cellVoltageA_PECflag = verify_PEC15(sideA_cellVoltageA);
			sideA_cellVoltageB_PECflag = verify_PEC15(sideA_cellVoltageB);
			sideA_cellVoltageC_PECflag = verify_PEC15(sideA_cellVoltageC);
			sideA_cellVoltageD_PECflag = verify_PEC15(sideA_cellVoltageD);

			sideB_cellVoltageA_PECflag = verify_PEC15(sideB_cellVoltageA);
			sideB_cellVoltageB_PECflag = verify_PEC15(sideB_cellVoltageB);
			sideB_cellVoltageC_PECflag = verify_PEC15(sideB_cellVoltageC);
			sideB_cellVoltageD_PECflag = verify_PEC15(sideB_cellVoltageD);

			if(sideA_cellVoltageA_PECflag == 2 &&
			   sideA_cellVoltageB_PECflag == 2 &&
			   sideA_cellVoltageC_PECflag == 2 &&
			   sideA_cellVoltageD_PECflag == 2)
			{
				sideA_subAttempts = 13;
			}
			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_cellVoltageA_PECflag == 2 &&
			   sideB_cellVoltageB_PECflag == 2 &&
			   sideB_cellVoltageC_PECflag == 2 &&
			   sideB_cellVoltageD_PECflag == 2)
			{
				sideB_subAttempts = 13;
			}
			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_cellVoltages[0] = (sideA_cellVoltageA[1] << 8) | sideA_cellVoltageA[0];
		sideA_cellVoltages[1] = (sideA_cellVoltageA[3] << 8) | sideA_cellVoltageA[2];
		sideA_cellVoltages[2] = (sideA_cellVoltageA[5] << 8) | sideA_cellVoltageA[4];
		sideA_cellVoltages[3] = (sideA_cellVoltageB[1] << 8) | sideA_cellVoltageB[0];
		sideA_cellVoltages[4] = (sideA_cellVoltageB[3] << 8) | sideA_cellVoltageB[2];
		sideA_cellVoltages[5] = (sideA_cellVoltageB[5] << 8) | sideA_cellVoltageB[4];
		sideA_cellVoltages[6] = (sideA_cellVoltageC[1] << 8) | sideA_cellVoltageC[0];
		sideA_cellVoltages[7] = (sideA_cellVoltageC[3] << 8) | sideA_cellVoltageC[2];
		sideA_cellVoltages[8] = (sideA_cellVoltageC[5] << 8) | sideA_cellVoltageC[4];
		sideA_cellVoltages[9] = (sideA_cellVoltageD[1] << 8) | sideA_cellVoltageD[0];
		sideA_cellVoltages[10] = (sideA_cellVoltageD[3] << 8) | sideA_cellVoltageD[2];
		sideA_cellVoltages[11] = (sideA_cellVoltageD[5] << 8) | sideA_cellVoltageD[4];

		sideB_cellVoltages[0] = (sideB_cellVoltageA[1] << 8) | sideB_cellVoltageA[0];
		sideB_cellVoltages[1] = (sideB_cellVoltageA[3] << 8) | sideB_cellVoltageA[2];
		sideB_cellVoltages[2] = (sideB_cellVoltageA[5] << 8) | sideB_cellVoltageA[4];
		sideB_cellVoltages[3] = (sideB_cellVoltageB[1] << 8) | sideB_cellVoltageB[0];
		sideB_cellVoltages[4] = (sideB_cellVoltageB[3] << 8) | sideB_cellVoltageB[2];
		sideB_cellVoltages[5] = (sideB_cellVoltageB[5] << 8) | sideB_cellVoltageB[4];
		sideB_cellVoltages[6] = (sideB_cellVoltageC[1] << 8) | sideB_cellVoltageC[0];
		sideB_cellVoltages[7] = (sideB_cellVoltageC[3] << 8) | sideB_cellVoltageC[2];
		sideB_cellVoltages[8] = (sideB_cellVoltageC[5] << 8) | sideB_cellVoltageC[4];
		sideB_cellVoltages[9] = (sideB_cellVoltageD[1] << 8) | sideB_cellVoltageD[0];
		sideB_cellVoltages[10] = (sideB_cellVoltageD[3] << 8) | sideB_cellVoltageD[2];
		sideB_cellVoltages[11] = (sideB_cellVoltageD[5] << 8) | sideB_cellVoltageD[4];

		uint8_t sideA_badOutputFlag = 0;
		uint8_t sideB_badOutputFlag = 0;

		for(uint8_t i = 0; i < 12; i++) {
			if(sideA_cellVoltages[i] != 0x9555) { sideA_badOutputFlag = 1; }

			if(sideB_cellVoltages[i] != 0x9555) { sideB_badOutputFlag = 1; }
		}

		if(!sideA_badOutputFlag) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(!sideB_badOutputFlag) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_CVST, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_CVST, 0, 0, SIDE_B); }

	// Test with second variant of CVST command
	sideA_attempts = 0;
	sideB_attempts = 0;
	sideA_subAttempts = 0;
	sideB_subAttempts = 0;
	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRCELL(SIDE_A);
			CLRCELL(SIDE_B);
			wait(3);

			CVST(0, SIDE_A);
			CVST(0, SIDE_B);
			wait(3);

			RDCVA(sideA_cellVoltageA, SIDE_A);
			RDCVA(sideB_cellVoltageA, SIDE_B);
			wait(1);
			RDCVB(sideA_cellVoltageB, SIDE_A);
			RDCVB(sideB_cellVoltageB, SIDE_B);
			wait(1);
			RDCVC(sideA_cellVoltageC, SIDE_A);
			RDCVC(sideB_cellVoltageC, SIDE_B);
			wait(1);
			RDCVD(sideA_cellVoltageD, SIDE_A);
			RDCVD(sideB_cellVoltageD, SIDE_B);

			sideA_cellVoltageA_PECflag = verify_PEC15(sideA_cellVoltageA);
			sideA_cellVoltageB_PECflag = verify_PEC15(sideA_cellVoltageB);
			sideA_cellVoltageC_PECflag = verify_PEC15(sideA_cellVoltageC);
			sideA_cellVoltageD_PECflag = verify_PEC15(sideA_cellVoltageD);

			sideB_cellVoltageA_PECflag = verify_PEC15(sideB_cellVoltageA);
			sideB_cellVoltageB_PECflag = verify_PEC15(sideB_cellVoltageB);
			sideB_cellVoltageC_PECflag = verify_PEC15(sideB_cellVoltageC);
			sideB_cellVoltageD_PECflag = verify_PEC15(sideB_cellVoltageD);

			if(sideA_cellVoltageA_PECflag == 2 &&
			   sideA_cellVoltageB_PECflag == 2 &&
			   sideA_cellVoltageC_PECflag == 2 &&
			   sideA_cellVoltageD_PECflag == 2)
			{
				sideA_subAttempts = 13;
			}
			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_cellVoltageA_PECflag == 2 &&
			   sideB_cellVoltageB_PECflag == 2 &&
			   sideB_cellVoltageC_PECflag == 2 &&
			   sideB_cellVoltageD_PECflag == 2)
			{
				sideB_subAttempts = 13;
			}
			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_cellVoltages[0] = (sideA_cellVoltageA[1] << 8) | sideA_cellVoltageA[0];
		sideA_cellVoltages[1] = (sideA_cellVoltageA[3] << 8) | sideA_cellVoltageA[2];
		sideA_cellVoltages[2] = (sideA_cellVoltageA[5] << 8) | sideA_cellVoltageA[4];
		sideA_cellVoltages[3] = (sideA_cellVoltageB[1] << 8) | sideA_cellVoltageB[0];
		sideA_cellVoltages[4] = (sideA_cellVoltageB[3] << 8) | sideA_cellVoltageB[2];
		sideA_cellVoltages[5] = (sideA_cellVoltageB[5] << 8) | sideA_cellVoltageB[4];
		sideA_cellVoltages[6] = (sideA_cellVoltageC[1] << 8) | sideA_cellVoltageC[0];
		sideA_cellVoltages[7] = (sideA_cellVoltageC[3] << 8) | sideA_cellVoltageC[2];
		sideA_cellVoltages[8] = (sideA_cellVoltageC[5] << 8) | sideA_cellVoltageC[4];
		sideA_cellVoltages[9] = (sideA_cellVoltageD[1] << 8) | sideA_cellVoltageD[0];
		sideA_cellVoltages[10] = (sideA_cellVoltageD[3] << 8) | sideA_cellVoltageD[2];
		sideA_cellVoltages[11] = (sideA_cellVoltageD[5] << 8) | sideA_cellVoltageD[4];

		sideB_cellVoltages[0] = (sideB_cellVoltageA[1] << 8) | sideB_cellVoltageA[0];
		sideB_cellVoltages[1] = (sideB_cellVoltageA[3] << 8) | sideB_cellVoltageA[2];
		sideB_cellVoltages[2] = (sideB_cellVoltageA[5] << 8) | sideB_cellVoltageA[4];
		sideB_cellVoltages[3] = (sideB_cellVoltageB[1] << 8) | sideB_cellVoltageB[0];
		sideB_cellVoltages[4] = (sideB_cellVoltageB[3] << 8) | sideB_cellVoltageB[2];
		sideB_cellVoltages[5] = (sideB_cellVoltageB[5] << 8) | sideB_cellVoltageB[4];
		sideB_cellVoltages[6] = (sideB_cellVoltageC[1] << 8) | sideB_cellVoltageC[0];
		sideB_cellVoltages[7] = (sideB_cellVoltageC[3] << 8) | sideB_cellVoltageC[2];
		sideB_cellVoltages[8] = (sideB_cellVoltageC[5] << 8) | sideB_cellVoltageC[4];
		sideB_cellVoltages[9] = (sideB_cellVoltageD[1] << 8) | sideB_cellVoltageD[0];
		sideB_cellVoltages[10] = (sideB_cellVoltageD[3] << 8) | sideB_cellVoltageD[2];
		sideB_cellVoltages[11] = (sideB_cellVoltageD[5] << 8) | sideB_cellVoltageD[4];

		uint8_t sideA_badOutputFlag = 0;
		uint8_t sideB_badOutputFlag = 0;

		for(uint8_t i = 0; i < 12; i++) {
			if(sideA_cellVoltages[i] != 0x6AAA) { sideA_badOutputFlag = 1; }

			if(sideB_cellVoltages[i] != 0x6AAA) { sideB_badOutputFlag = 1; }
		}

		if(!sideA_badOutputFlag) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(!sideB_badOutputFlag) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_CVST, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_CVST, 0, 0, SIDE_B); }
}


void aux_register_test(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint16_t sideA_gpio[10];
	uint16_t sideB_gpio[10];

	uint8_t sideA_auxA[8];
	uint8_t sideA_auxB[8];
	uint8_t sideA_auxC[8];
	uint8_t sideA_auxD[8];

	uint8_t sideB_auxA[8];
	uint8_t sideB_auxB[8];
	uint8_t sideB_auxC[8];
	uint8_t sideB_auxD[8];

	uint8_t sideA_auxA_PECflag;
	uint8_t sideA_auxB_PECflag;
	uint8_t sideA_auxC_PECflag;
	uint8_t sideA_auxD_PECflag;

	uint8_t sideB_auxA_PECflag;
	uint8_t sideB_auxB_PECflag;
	uint8_t sideB_auxC_PECflag;
	uint8_t sideB_auxD_PECflag;

	// Test with first variant of AXST command
	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRAUX(SIDE_A);
			CLRAUX(SIDE_B);
			wait(3);

			AXST(1, SIDE_A);
			AXST(1, SIDE_B);
			wait(3);

			RDAUXA(sideA_auxA, SIDE_A);
			RDAUXA(sideB_auxA, SIDE_B);
			wait(1);
			RDAUXB(sideA_auxB, SIDE_A);
			RDAUXB(sideB_auxB, SIDE_B);
			wait(1);
			RDAUXC(sideA_auxC, SIDE_A);
			RDAUXC(sideB_auxC, SIDE_B);
			wait(1);
			RDAUXD(sideA_auxD, SIDE_A);
			RDAUXD(sideB_auxD, SIDE_B);

			sideA_auxA_PECflag = verify_PEC15(sideA_auxA);
			sideA_auxB_PECflag = verify_PEC15(sideA_auxB);
			sideA_auxC_PECflag = verify_PEC15(sideA_auxC);
			sideA_auxD_PECflag = verify_PEC15(sideA_auxD);

			sideB_auxA_PECflag = verify_PEC15(sideB_auxA);
			sideB_auxB_PECflag = verify_PEC15(sideB_auxB);
			sideB_auxC_PECflag = verify_PEC15(sideB_auxC);
			sideB_auxD_PECflag = verify_PEC15(sideB_auxD);

			if(sideA_auxA_PECflag == 2 &&
			   sideA_auxB_PECflag == 2 &&
			   sideA_auxC_PECflag == 2 &&
			   sideA_auxD_PECflag == 2)
			{
				sideA_subAttempts = 13;
			}
			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_auxA_PECflag == 2 &&
			   sideB_auxB_PECflag == 2 &&
			   sideB_auxC_PECflag == 2 &&
			   sideB_auxD_PECflag == 2)
			{
				sideB_subAttempts = 13;
			}
			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_gpio[0] = (sideA_auxA[1] << 8) | sideA_auxA[0];
		sideA_gpio[1] = (sideA_auxA[3] << 8) | sideA_auxA[2];
		sideA_gpio[2] = (sideA_auxA[5] << 8) | sideA_auxA[4];
		sideA_gpio[3] = (sideA_auxB[1] << 8) | sideA_auxB[0];
		sideA_gpio[4] = (sideA_auxB[3] << 8) | sideA_auxB[2];
		sideA_gpio[5] = (sideA_auxB[5] << 8) | sideA_auxB[4];
		sideA_gpio[6] = (sideA_auxC[1] << 8) | sideA_auxC[0];
		sideA_gpio[7] = (sideA_auxC[3] << 8) | sideA_auxC[2];
		sideA_gpio[8] = (sideA_auxC[5] << 8) | sideA_auxC[4];
		sideA_gpio[9] = (sideA_auxD[1] << 8) | sideA_auxD[0];

		sideB_gpio[0] = (sideB_auxA[1] << 8) | sideB_auxA[0];
		sideB_gpio[1] = (sideB_auxA[3] << 8) | sideB_auxA[2];
		sideB_gpio[2] = (sideB_auxA[5] << 8) | sideB_auxA[4];
		sideB_gpio[3] = (sideB_auxB[1] << 8) | sideB_auxB[0];
		sideB_gpio[4] = (sideB_auxB[3] << 8) | sideB_auxB[2];
		sideB_gpio[5] = (sideB_auxB[5] << 8) | sideB_auxB[4];
		sideB_gpio[6] = (sideB_auxC[1] << 8) | sideB_auxC[0];
		sideB_gpio[7] = (sideB_auxC[3] << 8) | sideB_auxC[2];
		sideB_gpio[8] = (sideB_auxC[5] << 8) | sideB_auxC[4];
		sideB_gpio[9] = (sideB_auxD[1] << 8) | sideB_auxD[0];

		uint8_t sideA_badOutputFlag = 0;
		uint8_t sideB_badOutputFlag = 0;

		for(uint8_t i = 0; i < 10; i++) {
			if(sideA_gpio[i] != 0x9555) { sideA_badOutputFlag = 1; }

			if(sideB_gpio[i] != 0x9555) { sideB_badOutputFlag = 1; }
		}

		if(!sideA_badOutputFlag) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(!sideB_badOutputFlag) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_AXST, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_AXST, 0, 0, SIDE_B); }

	// Test with second variant of AXST command
	sideA_attempts = 0;
	sideB_attempts = 0;
	sideA_subAttempts = 0;
	sideB_subAttempts = 0;
	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRAUX(SIDE_A);
			CLRAUX(SIDE_B);
			wait(3);

			AXST(0, SIDE_A);
			AXST(0, SIDE_B);
			wait(3);

			RDAUXA(sideA_auxA, SIDE_A);
			RDAUXA(sideB_auxA, SIDE_B);
			wait(1);
			RDAUXB(sideA_auxB, SIDE_A);
			RDAUXB(sideB_auxB, SIDE_B);
			wait(1);
			RDAUXC(sideA_auxC, SIDE_A);
			RDAUXC(sideB_auxC, SIDE_B);
			wait(1);
			RDAUXD(sideA_auxD, SIDE_A);
			RDAUXD(sideB_auxD, SIDE_B);

			sideA_auxA_PECflag = verify_PEC15(sideA_auxA);
			sideA_auxB_PECflag = verify_PEC15(sideA_auxB);
			sideA_auxC_PECflag = verify_PEC15(sideA_auxC);
			sideA_auxD_PECflag = verify_PEC15(sideA_auxD);

			sideB_auxA_PECflag = verify_PEC15(sideB_auxA);
			sideB_auxB_PECflag = verify_PEC15(sideB_auxB);
			sideB_auxC_PECflag = verify_PEC15(sideB_auxC);
			sideB_auxD_PECflag = verify_PEC15(sideB_auxD);

			if(sideA_auxA_PECflag == 2 &&
			   sideA_auxB_PECflag == 2 &&
			   sideA_auxC_PECflag == 2 &&
			   sideA_auxD_PECflag == 2)
			{
				sideA_subAttempts = 13;
			}
			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_auxA_PECflag == 2 &&
			   sideB_auxB_PECflag == 2 &&
			   sideB_auxC_PECflag == 2 &&
			   sideB_auxD_PECflag == 2)
			{
				sideB_subAttempts = 13;
			}
			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_gpio[0] = (sideA_auxA[1] << 8) | sideA_auxA[0];
		sideA_gpio[1] = (sideA_auxA[3] << 8) | sideA_auxA[2];
		sideA_gpio[2] = (sideA_auxA[5] << 8) | sideA_auxA[4];
		sideA_gpio[3] = (sideA_auxB[1] << 8) | sideA_auxB[0];
		sideA_gpio[4] = (sideA_auxB[3] << 8) | sideA_auxB[2];
		sideA_gpio[5] = (sideA_auxB[5] << 8) | sideA_auxB[4];
		sideA_gpio[6] = (sideA_auxC[1] << 8) | sideA_auxC[0];
		sideA_gpio[7] = (sideA_auxC[3] << 8) | sideA_auxC[2];
		sideA_gpio[8] = (sideA_auxC[5] << 8) | sideA_auxC[4];
		sideA_gpio[9] = (sideA_auxD[1] << 8) | sideA_auxD[0];

		sideB_gpio[0] = (sideB_auxA[1] << 8) | sideB_auxA[0];
		sideB_gpio[1] = (sideB_auxA[3] << 8) | sideB_auxA[2];
		sideB_gpio[2] = (sideB_auxA[5] << 8) | sideB_auxA[4];
		sideB_gpio[3] = (sideB_auxB[1] << 8) | sideB_auxB[0];
		sideB_gpio[4] = (sideB_auxB[3] << 8) | sideB_auxB[2];
		sideB_gpio[5] = (sideB_auxB[5] << 8) | sideB_auxB[4];
		sideB_gpio[6] = (sideB_auxC[1] << 8) | sideB_auxC[0];
		sideB_gpio[7] = (sideB_auxC[3] << 8) | sideB_auxC[2];
		sideB_gpio[8] = (sideB_auxC[5] << 8) | sideB_auxC[4];
		sideB_gpio[9] = (sideB_auxD[1] << 8) | sideB_auxD[0];

		uint8_t sideA_badOutputFlag = 0;
		uint8_t sideB_badOutputFlag = 0;

		for(uint8_t i = 0; i < 10; i++) {
			if(sideA_gpio[i] != 0x6AAA) { sideA_badOutputFlag = 1; }

			if(sideB_gpio[i] != 0x6AAA) { sideB_badOutputFlag = 1; }
		}

		if(!sideA_badOutputFlag) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(!sideB_badOutputFlag) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_AXST, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_AXST, 0, 0, SIDE_B); }
}


void overlap_cell_measurement_test(void)
{
	uint8_t sideA_attempts = 0;
	uint8_t sideB_attempts = 0;
	uint8_t sideA_subAttempts = 0;
	uint8_t sideB_subAttempts = 0;

	uint16_t sideA_cell7_adc1;
	uint16_t sideA_cell7_adc2;
	uint16_t sideA_cell13_adc2;
	uint16_t sideA_cell13_adc3;

	uint16_t sideA_cell7_difference;
	uint16_t sideA_cell13_difference;

	uint16_t sideB_cell7_adc1;
	uint16_t sideB_cell7_adc2;
	uint16_t sideB_cell13_adc2;
	uint16_t sideB_cell13_adc3;

	uint16_t sideB_cell7_difference;
	uint16_t sideB_cell13_difference;

	uint8_t sideA_cellVoltageC[8];
	uint8_t sideA_cellVoltageE[8];

	uint8_t sideB_cellVoltageC[8];
	uint8_t sideB_cellVoltageE[8];

	uint8_t sideA_cellVoltageC_PECflag;
	uint8_t sideA_cellVoltageE_PECflag;

	uint8_t sideB_cellVoltageC_PECflag;
	uint8_t sideB_cellVoltageE_PECflag;

	while(sideA_attempts < ATTEMPT_LIMIT || sideB_attempts < ATTEMPT_LIMIT) {
		while(sideA_subAttempts < ATTEMPT_LIMIT || sideB_subAttempts < ATTEMPT_LIMIT) {
			CLRCELL(SIDE_A);
			CLRCELL(SIDE_B);
			wait(3);

			ADOL(SIDE_A);
			ADOL(SIDE_B);
			wait(3);

			RDCVC(sideA_cellVoltageC, SIDE_A);
			RDCVC(sideB_cellVoltageC, SIDE_B);
			wait(1);
			RDCVE(sideA_cellVoltageE, SIDE_A);
			RDCVE(sideB_cellVoltageE, SIDE_B);
			wait(1);

			sideA_cellVoltageC_PECflag = verify_PEC15(sideA_cellVoltageC);
			sideA_cellVoltageE_PECflag = verify_PEC15(sideA_cellVoltageE);
			sideB_cellVoltageC_PECflag = verify_PEC15(sideB_cellVoltageC);
			sideB_cellVoltageE_PECflag = verify_PEC15(sideB_cellVoltageE);

			if(sideA_cellVoltageC_PECflag == 2 &&
			   sideA_cellVoltageE_PECflag == 2 &&
			   sideA_cellVoltageC[1] != 0xFF &&
			   sideA_cellVoltageC[3] != 0xFF &&
			   sideA_cellVoltageC[5] != 0xFF &&
			   sideA_cellVoltageE[1] != 0xFF &&
			   sideA_cellVoltageE[3] != 0xFF &&
			   sideA_cellVoltageE[5] != 0xFF)
			{
				sideA_subAttempts = 13;
			}
			else {
				sideA_subAttempts++;
				wait(1);
			}

			if(sideB_cellVoltageC_PECflag == 2 &&
			   sideB_cellVoltageE_PECflag == 2 &&
			   sideB_cellVoltageC[1] != 0xFF &&
			   sideB_cellVoltageC[3] != 0xFF &&
			   sideB_cellVoltageC[5] != 0xFF &&
			   sideB_cellVoltageE[1] != 0xFF &&
			   sideB_cellVoltageE[3] != 0xFF &&
			   sideB_cellVoltageE[5] != 0xFF)
			{
				sideB_subAttempts = 13;
			}
			else {
				sideB_subAttempts++;
				wait(1);
			}
		}
		if(sideA_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_A); }

		if(sideB_subAttempts != 13) { error_loop(ERROR_PEC, 0, 0, SIDE_B); }

		sideA_cell7_adc1 = (sideA_cellVoltageC[3] << 8) | sideA_cellVoltageC[2];
		sideA_cell7_adc2 = (sideA_cellVoltageC[1] << 8) | sideA_cellVoltageC[0];
		sideA_cell13_adc2 = (sideA_cellVoltageE[3] << 8) | sideA_cellVoltageE[2];
		sideA_cell13_adc3 = (sideA_cellVoltageE[1] << 8) | sideA_cellVoltageE[0];

		sideB_cell7_adc1 = (sideB_cellVoltageC[3] << 8) | sideB_cellVoltageC[2];
		sideB_cell7_adc2 = (sideB_cellVoltageC[1] << 8) | sideB_cellVoltageC[0];
		sideB_cell13_adc2 = (sideB_cellVoltageE[3] << 8) | sideB_cellVoltageE[2];
		sideB_cell13_adc3 = (sideB_cellVoltageE[1] << 8) | sideB_cellVoltageE[0];

		sideA_cell7_difference = (uint16_t)fabsf((float)sideA_cell7_adc1 - (float)sideA_cell7_adc2);
		sideA_cell13_difference = (uint16_t)fabsf((float)sideA_cell13_adc2 - (float)sideA_cell13_adc3);
		sideB_cell7_difference = (uint16_t)fabsf((float)sideB_cell7_adc1 - (float)sideB_cell7_adc2);
		sideB_cell13_difference = (uint16_t)fabsf((float)sideB_cell13_adc2 - (float)sideB_cell13_adc3);

		if(sideA_cell7_difference <= MAX_ADC_DELTA && sideA_cell13_difference <= MAX_ADC_DELTA) { sideA_attempts = 13; }

		else {
			sideA_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}

		if(sideB_cell7_difference <= MAX_ADC_DELTA && sideB_cell13_difference <= MAX_ADC_DELTA) { sideB_attempts = 13; }

		else {
			sideB_attempts++;
			sideA_subAttempts = 0;
			sideB_subAttempts = 0;
			wait(1);
		}
	}
	if(sideA_attempts != 13) { error_loop(ERROR_ADOL, 0, 0, SIDE_A); }

	if(sideB_attempts != 13) { error_loop(ERROR_ADOL, 0, 0, SIDE_B); }
}

