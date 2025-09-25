#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_ltc6813.h"
#include "torch_voltage.h"
#include "torch_can.h"


void voltage_sense(uint16_t *cellVoltages)
{
	uint8_t attempts = 0;

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

	while(attempts < ATTEMPT_LIMIT) {
		CLRCELL(SIDE_A);
		CLRCELL(SIDE_B);
		wait(3);

		ADCV(SIDE_A);
		ADCV(SIDE_B);
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
		   sideA_cellVoltageD_PECflag == 2 &&
		   sideB_cellVoltageA_PECflag == 2 &&
		   sideB_cellVoltageB_PECflag == 2 &&
		   sideB_cellVoltageC_PECflag == 2 &&
		   sideB_cellVoltageD_PECflag == 2 &&
		   sideA_cellVoltageA[1] != 0xFF &&
		   sideA_cellVoltageA[3] != 0xFF &&
		   sideA_cellVoltageA[5] != 0xFF &&
		   sideB_cellVoltageA[1] != 0xFF &&
		   sideB_cellVoltageA[3] != 0xFF &&
		   sideB_cellVoltageA[5] != 0xFF &&
		   sideA_cellVoltageB[1] != 0xFF &&
		   sideA_cellVoltageB[3] != 0xFF &&
		   sideA_cellVoltageB[5] != 0xFF &&
		   sideB_cellVoltageB[1] != 0xFF &&
		   sideB_cellVoltageB[3] != 0xFF &&
		   sideB_cellVoltageB[5] != 0xFF &&
		   sideA_cellVoltageC[1] != 0xFF &&
		   sideA_cellVoltageC[3] != 0xFF &&
		   sideA_cellVoltageC[5] != 0xFF &&
		   sideB_cellVoltageC[1] != 0xFF &&
		   sideB_cellVoltageC[3] != 0xFF &&
		   sideB_cellVoltageC[5] != 0xFF &&
		   sideA_cellVoltageD[1] != 0xFF &&
		   sideA_cellVoltageD[3] != 0xFF &&
		   sideA_cellVoltageD[5] != 0xFF &&
		   sideB_cellVoltageD[1] != 0xFF &&
		   sideB_cellVoltageD[3] != 0xFF &&
		   sideB_cellVoltageD[5] != 0xFF)
		{
			attempts = 13;
		}
		else {
			attempts++;
			wait(1);
		}
	}
	//if(attempts != 13) { error_loop(ERROR_PEC, 0, 0); }

	*cellVoltages = (sideA_cellVoltageA[1] << 8) | sideA_cellVoltageA[0];
	*(cellVoltages + 1) = (sideA_cellVoltageA[3] << 8) | sideA_cellVoltageA[2];
	*(cellVoltages + 2) = (sideA_cellVoltageA[5] << 8) | sideA_cellVoltageA[4];
	*(cellVoltages + 3) = (sideA_cellVoltageB[1] << 8) | sideA_cellVoltageB[0];
	*(cellVoltages + 4) = (sideA_cellVoltageB[3] << 8) | sideA_cellVoltageB[2];
	*(cellVoltages + 5) = (sideA_cellVoltageB[5] << 8) | sideA_cellVoltageB[4];
	*(cellVoltages + 6) = (sideA_cellVoltageC[1] << 8) | sideA_cellVoltageC[0];
	*(cellVoltages + 7) = (sideA_cellVoltageC[3] << 8) | sideA_cellVoltageC[2];
	*(cellVoltages + 8) = (sideA_cellVoltageC[5] << 8) | sideA_cellVoltageC[4];
	*(cellVoltages + 9) = (sideA_cellVoltageD[1] << 8) | sideA_cellVoltageD[0];

	*(cellVoltages + 10) = (sideB_cellVoltageA[1] << 8) | sideB_cellVoltageA[0];
	*(cellVoltages + 11) = (sideB_cellVoltageA[3] << 8) | sideB_cellVoltageA[2];
	*(cellVoltages + 12) = (sideB_cellVoltageA[5] << 8) | sideB_cellVoltageA[4];
	*(cellVoltages + 13) = (sideB_cellVoltageB[1] << 8) | sideB_cellVoltageB[0];
	*(cellVoltages + 14) = (sideB_cellVoltageB[3] << 8) | sideB_cellVoltageB[2];
	*(cellVoltages + 15) = (sideB_cellVoltageB[5] << 8) | sideB_cellVoltageB[4];
	*(cellVoltages + 16) = (sideB_cellVoltageC[1] << 8) | sideB_cellVoltageC[0];
	*(cellVoltages + 17) = (sideB_cellVoltageC[3] << 8) | sideB_cellVoltageC[2];
	*(cellVoltages + 18) = (sideB_cellVoltageC[5] << 8) | sideB_cellVoltageC[4];
	*(cellVoltages + 19) = (sideB_cellVoltageD[1] << 8) | sideB_cellVoltageD[0];

}
