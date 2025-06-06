#include "main.h"
#include "torch_LTC6813.h"
#include "torch_main.h"
#include "torch_STM32.h"
#include "torch_faults.h"

// DELETE COMMENTED OUT FUNCTIONS ONCE NEW boards ARE TESTED

uint16_t PEC15_table[256];
uint16_t CRC15_Poly = 0x4599;


void setup_PEC15(void)
{
  uint16_t PEC15_seed = 16;

  for (uint16_t i = 0; i < 256; i++) {
    PEC15_seed = i << 7;

    for (uint8_t bit = 8; bit > 0; bit--){
      if (PEC15_seed & 0x4000) {
        PEC15_seed = ((PEC15_seed << 1));
        PEC15_seed = (PEC15_seed ^ CRC15_Poly);
      }
      else {
        PEC15_seed = ((PEC15_seed << 1));
      }
    }
    PEC15_table[i] = PEC15_seed & 0xFFFF;
  }
}


uint16_t compute_PEC15(uint8_t *data_ptr, uint8_t len)
{
  uint16_t PEC15_seed = 16;
  uint16_t address;

  for (uint16_t i = 0; i < len; i++) {
    address = ((PEC15_seed >> 7) ^ data_ptr[i]) & 0xFF;
    PEC15_seed = (PEC15_seed << 8 ) ^ PEC15_table[address];
  }
  return (PEC15_seed * 2);
}


void append_PEC(uint8_t *data_ptr, uint8_t len, uint16_t PEC)
{
	data_ptr[len] = (PEC >> 8) & 0xFF;
	data_ptr[len + 1] = PEC & 0xFF;
}


uint8_t verify_PEC15(uint8_t *receivedData_ptr)
{
	uint16_t receivedPEC = (receivedData_ptr[PAYLOAD_LEN - 2] << 8) | receivedData_ptr[PAYLOAD_LEN - 1];

	uint16_t calculatedPEC = compute_PEC15(receivedData_ptr, PAYLOAD_LEN - 2);

	if (receivedPEC == calculatedPEC) { return 2; }

	else { return 1; }
}


void action_cmd(uint8_t *cmd_ptr, uint8_t sideA)
{
	// TO LTC SIDE A (CELL 1 to 10)
	if(sideA) {
		/*HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);*/
		pull_low(GPIOA, GPIO_PIN_4);
		SPI_transmit(&hspi1, cmd_ptr, CMD_LEN);
		pull_high(GPIOA, GPIO_PIN_4);
	}

	// TO LTC SIDE B (CELL 11 to 20)
	else {
		/*HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi3, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);*/
		pull_low(GPIOA, GPIO_PIN_15);
		SPI_transmit(&hspi3, cmd_ptr, CMD_LEN);
		pull_high(GPIOA, GPIO_PIN_15);
	}
}


void write_cmd(uint8_t *cmd_ptr, uint8_t *payload_ptr, uint8_t sideA)
{
	// TO LTC SIDE A (CELL 1 to 10)
	if(sideA) {
		pull_low(GPIOA, GPIO_PIN_4);
		SPI_transmit(&hspi1, cmd_ptr, CMD_LEN);
		SPI_transmit(&hspi1, payload_ptr, PAYLOAD_LEN);
		pull_high(GPIOA, GPIO_PIN_4);
	}

	// TO LTC SIDE B (CELL 11 to 20)
	else {
		pull_low(GPIOA, GPIO_PIN_15);
		SPI_transmit(&hspi3, cmd_ptr, CMD_LEN);
		SPI_transmit(&hspi3, payload_ptr, PAYLOAD_LEN);
		pull_high(GPIOA, GPIO_PIN_15);
	}
}

void read_cmd(uint8_t *cmd_ptr, uint8_t *receivedPayload_ptr, uint8_t sideA)
{
	//uint8_t dummies[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	// TO LTC SIDE A (CELL 1 to 10)
	if(sideA) {
		/*HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_SPI_TransmitReceive(&hspi1, dummies, receivedPayload_ptr, PAYLOAD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);*/
		pull_low(GPIOA, GPIO_PIN_4);
		SPI_transmit(&hspi1, cmd_ptr, CMD_LEN);
		SPI_receive(&hspi1, receivedPayload_ptr, PAYLOAD_LEN);
		pull_high(GPIOA, GPIO_PIN_4);
	}

	// TO LTC SIDE B (CELL 11 to 20)
	else {
		/*HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi3, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_SPI_TransmitReceive(&hspi3, dummies, receivedPayload_ptr, PAYLOAD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);*/
		pull_low(GPIOA, GPIO_PIN_15);
		SPI_transmit(&hspi3, cmd_ptr, CMD_LEN);
		SPI_receive(&hspi3, receivedPayload_ptr, PAYLOAD_LEN);
		pull_high(GPIOA, GPIO_PIN_15);
	}
}

// *** WRITE COMMANDS ***
/*void WRCFGA(uint8_t *payload_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;
	uint8_t configRegisterGroupA[8];
	uint16_t configRegisterGroupA_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	for(uint8_t i = 0; i < 6; i ++) {
		configRegisterGroupA[i] = *(payload_ptr + i);
	}
	configRegisterGroupA_PEC = compute_PEC15(configRegisterGroupA, 6);
	append_PEC(configRegisterGroupA, 6, configRegisterGroupA_PEC);

	write_cmd(cmd, configRegisterGroupA, side);
}*/
void WRCFGA(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t wrcfgaCmd[4];
	uint16_t wrcfgaCmd_PEC;
	uint8_t wrcfgaPayload[8];
	uint16_t wrcfgaPayload_PEC;
	
	wrcfgaCmd[0] = 0x00;
	wrcfgaCmd[1] = 0x01;
	wrcfgaCmd_PEC = compute_PEC15(wrcfgaCmd, 2);
	append_PEC(wrcfgaCmd, 2, wrcfgaCmd_PEC);
	
	for(uint8_t i = 0; i < 6; i++) { wrcfgaPayload[i] = asicRegisters->configRegisterA[i]; }
	
	wrcfgaPayload_PEC = compute_PEC15(wrcfgaPayload, 6);
	append_PEC(wrcfgaPayload, 6, wrcfgaPayload_PEC);
	
	write_cmd(wrcfgaCmd, wrcfgaPayload, side);
}


/*void WRCFGB(uint8_t *payload_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;
	uint8_t configRegisterGroupB[8];
	uint16_t configRegisterGroupB_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x24;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	for(uint8_t i = 0; i < 2; i ++) {
		configRegisterGroupB[i] = *(payload_ptr + i);
	}

	// CFGBR2 to CFGBR5 has reserved bits that're 0. That's why we're forcing them to 0
	for(uint8_t i = 2; i < 6; i++) {
		configRegisterGroupB[i] = 0x00;
	}

	configRegisterGroupB_PEC = compute_PEC15(configRegisterGroupB, 6);
	append_PEC(configRegisterGroupB, 6, configRegisterGroupB_PEC);

	write_cmd(cmd, configRegisterGroupB, side);
}*/
void WRCFGB(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t wrcfgbCmd[4];
	uint16_t wrcfgbCmd_PEC;
	uint8_t wrcfgbPayload[8];
	uint16_t wrcfgbPayload_PEC;
	
	wrcfgbCmd[0] = 0x00;
	wrcfgbCmd[1] = 0x24;
	wrcfgbCmd_PEC = compute_PEC15(wrcfgbCmd, 2);
	append_PEC(wrcfgbCmd, 2, wrcfgbCmd_PEC);
	
	for(uint8_t i = 0; i < 2; i++) { wrcfgbPayload[i] = asicRegisters->configRegisterB[i]; }
	
	// CFGBR2 to CFGBR5 has reserved bits that're 0. That's why we're forcing them to 0
	for(uint8_t i = 2; i < 6; i++) { wrcfgbPayload[i] = 0x00; }
	
	wrcfgbPayload_PEC = compute_PEC15(wrcfgbPayload, 6);
	append_PEC(wrcfgbPayload, 6, wrcfgbPayload_PEC);
	
	write_cmd(wrcfgbCmd, wrcfgbPayload, side);
}


/*void WRPWM(uint8_t *payload_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;
	uint8_t registerGroupPWM[8];
	uint16_t registerGroupPWM_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x20;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	for(uint8_t i = 0; i < 6; i ++) {
		registerGroupPWM[i] = *(payload_ptr + i);
	}
	registerGroupPWM_PEC = compute_PEC15(registerGroupPWM, 6);
	append_PEC(registerGroupPWM, 6, registerGroupPWM_PEC);

	write_cmd(cmd, registerGroupPWM, side);
}*/
void WRPWM(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t wrpwmCmd[4];
	uint16_t wrpwmCmd_PEC;
	uint8_t wrpwmPayload[8];
	uint16_t wrpwmPayload_PEC;
	
	wrpwmCmd[0] = 0x00;
	wrpwmCmd[1] = 0x20;
	wrpwmCmd_PEC = compute_PEC15(wrpwmCmd, 2);
	append_PEC(wrpwmCmd, 2, wrpwmCmd_PEC);
	
	for(uint8_t i = 0; i < 6; i++) { wrpwmPayload[i] = asicRegisters->pwmRegister[i]; }
	
	wrpwmPayload_PEC = compute_PEC15(wrpwmPayload, 6);
	append_PEC(wrpwmPayload, 6, wrpwmPayload_PEC);
	
	write_cmd(wrpwmCmd, wrpwmPayload, side);
}
// *** END WRITE COMMANDS ***


// *** READ COMMANDS ***
/*void RDCFGA(uint8_t *configRegisterGroupA_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x02;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, configRegisterGroupA_ptr, side);
}*/
void RDCFGA(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdcfgaCmd[4];
	uint16_t rdcfgaCmd_PEC;
	uint8_t rdcfgaPayload[8];
	uint8_t rdcfgaPayload_PECflag;
	
	uint8_t attempts = 0;

	rdcfgaCmd[0] = 0x00;
	rdcfgaCmd[1] = 0x02;
	rdcfgaCmd_PEC = compute_PEC15(rdcfgaCmd, 2);
	append_PEC(rdcfgaCmd, 2, rdcfgaCmd_PEC);
	
	while(attempts < 5) {
		read_cmd(rdcfgaCmd, rdcfgaPayload, side);
		
		rdcfgaPayload_PECflag = verify_PEC15(rdcfgaPayload);
		
		if(rdcfgaPayload_PECflag == 2) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->configRegisterA[i] = rdcfgaPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDCFGB(uint8_t *configRegisterGroupB_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x26;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, configRegisterGroupB_ptr, side);
}*/
void RDCFGB(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdcfgbCmd[4];
	uint16_t rdcfgbCmd_PEC;
	uint8_t rdcfgbPayload[8];
	uint8_t rdcfgbPayload_PECflag;
	
	uint8_t attempts = 0;

	rdcfgbCmd[0] = 0x00;
	rdcfgbCmd[1] = 0x26;
	rdcfgbCmd_PEC = compute_PEC15(rdcfgbCmd, 2);
	append_PEC(rdcfgbCmd, 2, rdcfgbCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdcfgbCmd, rdcfgbPayload, side);
		
		rdcfgbPayload_PECflag = verify_PEC15(rdcfgbPayload);
		
		if(rdcfgbPayload_PECflag == 2) {
			for(uint8_t i = 0; i < 2; i++) { asicRegisters->configRegisterB[i] = rdcfgbPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDCVA(uint8_t *cellVoltageRegisterGroupA_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x04;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupA_ptr, side);
}*/
void RDCVA(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdcvaCmd[4];
	uint16_t rdcvaCmd_PEC;
	uint8_t rdcvaPayload[8];
	uint8_t rdcvaPayload_PECflag;
	
	uint8_t attempts = 0;
	uint8_t registerDefaultsFlag = 0;		// Flag for whether the LTC6813 successfully ADC'd

	rdcvaCmd[0] = 0x00;
	rdcvaCmd[1] = 0x04;
	rdcvaCmd_PEC = compute_PEC15(rdcvaCmd, 2);
	append_PEC(rdcvaCmd, 2, rdcvaCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdcvaCmd, rdcvaPayload, side);
		
		rdcvaPayload_PECflag = verify_PEC15(rdcvaPayload);
		
		
		if(rdcvaPayload_PECflag == 2 && rdcvaPayload[1] != 0xFF && rdcvaPayload[3] != 0xFF && rdcvaPayload[5] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->voltageRegisterA[i] = rdcvaPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDCVB(uint8_t *cellVoltageRegisterGroupB_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x06;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupB_ptr, side);
}*/
void RDCVB(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdcvbCmd[4];
	uint16_t rdcvbCmd_PEC;
	uint8_t rdcvbPayload[8];
	uint8_t rdcvbPayload_PECflag;
	
	uint8_t attempts = 0;

	rdcvbCmd[0] = 0x00;
	rdcvbCmd[1] = 0x04;
	rdcvbCmd_PEC = compute_PEC15(rdcvbCmd, 2);
	append_PEC(rdcvbCmd, 2, rdcvbCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdcvbCmd, rdcvbPayload, side);
		
		rdcvbPayload_PECflag = verify_PEC15(rdcvbPayload);
		
		if(rdcvbPayload_PECflag == 2 && rdcvbPayload[1] != 0xFF && rdcvbPayload[3] != 0xFF && rdcvbPayload[5] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->voltageRegisterB[i] = rdcvbPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDCVC(uint8_t *cellVoltageRegisterGroupC_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x08;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupC_ptr, side);
}*/
void RDCVC(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdcvcCmd[4];
	uint16_t rdcvcCmd_PEC;
	uint8_t rdcvcPayload[8];
	uint8_t rdcvcPayload_PECflag;
	
	uint8_t attempts = 0;

	rdcvcCmd[0] = 0x00;
	rdcvcCmd[1] = 0x08;
	rdcvcCmd_PEC = compute_PEC15(rdcvcCmd, 2);
	append_PEC(rdcvcCmd, 2, rdcvcCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdcvcCmd, rdcvcPayload, side);
		
		rdcvcPayload_PECflag = verify_PEC15(rdcvcPayload);
		
		if(rdcvcPayload_PECflag == 2 && rdcvcPayload[1] != 0xFF && rdcvcPayload[3] != 0xFF && rdcvcPayload[5] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->voltageRegisterC[i] = rdcvcPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDCVD(uint8_t *cellVoltageRegisterGroupD_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x0A;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupD_ptr, side);
}*/
void RDCVD(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdcvdCmd[4];
	uint16_t rdcvdCmd_PEC;
	uint8_t rdcvdPayload[8];
	uint8_t rdcvdPayload_PECflag;
	
	uint8_t attempts = 0;

	rdcvdCmd[0] = 0x00;
	rdcvdCmd[1] = 0x0A;
	rdcvdCmd_PEC = compute_PEC15(rdcvdCmd, 2);
	append_PEC(rdcvdCmd, 2, rdcvdCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdcvdCmd, rdcvdPayload, side);
		
		rdcvdPayload_PECflag = verify_PEC15(rdcvdPayload);
		
		if(rdcvdPayload_PECflag == 2 && rdcvdPayload[1] != 0xFF && rdcvdPayload[3] != 0xFF && rdcvdPayload[5] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->voltageRegisterD[i] = rdcvdPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


void RDAUXA(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdauxaCmd[4];
	uint16_t rdauxaCmd_PEC;
	uint8_t rdauxaPayload[8];
	uint8_t rdauxaPayload_PECflag;
	
	uint8_t attempts = 0;

	rdauxaCmd[0] = 0x00;
	rdauxaCmd[1] = 0x0C;
	rdauxaCmd_PEC = compute_PEC15(rdauxaCmd, 2);
	append_PEC(rdauxaCmd, 2, rdauxaCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdauxaCmd, rdauxaPayload, side);
		
		rdauxaPayload_PECflag = verify_PEC15(rdauxaPayload);
		
		if(rdauxaPayload_PECflag == 2 && rdauxaPayload[1] != 0xFF && rdauxaPayload[3] != 0xFF && rdauxaPayload[5] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->auxRegisterA[i] = rdauxaPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


void RDAUXB(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdauxbCmd[4];
	uint16_t rdauxbCmd_PEC;
	uint8_t rdauxbPayload[8];
	uint8_t rdauxbPayload_PECflag;
	
	uint8_t attempts = 0;

	rdauxbCmd[0] = 0x00;
	rdauxbCmd[1] = 0x0E;
	rdauxbCmd_PEC = compute_PEC15(rdauxbCmd, 2);
	append_PEC(rdauxbCmd, 2, rdauxbCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdauxbCmd, rdauxbPayload, side);
		
		rdauxbPayload_PECflag = verify_PEC15(rdauxbPayload);
		
		if(rdauxbPayload_PECflag == 2 && rdauxbPayload[1] != 0xFF && rdauxbPayload[3] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->auxRegisterB[i] = rdauxbPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


void RDAUXC(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdauxcCmd[4];
	uint16_t rdauxcCmd_PEC;
	uint8_t rdauxcPayload[8];
	uint8_t rdauxcPayload_PECflag;
	
	uint8_t attempts = 0;

	rdauxcCmd[0] = 0x00;
	rdauxcCmd[1] = 0x0D;
	rdauxcCmd_PEC = compute_PEC15(rdauxcCmd, 2);
	append_PEC(rdauxcCmd, 2, rdauxcCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdauxcCmd, rdauxcPayload, side);
		
		rdauxcPayload_PECflag = verify_PEC15(rdauxcPayload);
		
		if(rdauxcPayload_PECflag == 2 && rdauxcPayload[1] != 0xFF && rdauxcPayload[3] != 0xFF && rdauxcPayload[5] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->auxRegisterC[i] = rdauxcPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


void RDAUXD(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdauxdCmd[4];
	uint16_t rdauxdCmd_PEC;
	uint8_t rdauxdPayload[8];
	uint8_t rdauxdPayload_PECflag;
	
	uint8_t attempts = 0;

	rdauxdCmd[0] = 0x00;
	rdauxdCmd[1] = 0x0F;
	rdauxdCmd_PEC = compute_PEC15(rdauxdCmd, 2);
	append_PEC(rdauxdCmd, 2, rdauxdCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdauxdCmd, rdauxdPayload, side);
		
		rdauxdPayload_PECflag = verify_PEC15(rdauxdPayload);
		
		if(rdauxdPayload_PECflag == 2 && rdauxdPayload[1] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->auxRegisterD[i] = rdauxdPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDSTATA(uint8_t *statusRegisterGroupA_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x10;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, statusRegisterGroupA_ptr, side);
}*/
void RDSTATA(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdstataCmd[4];
	uint16_t rdstataCmd_PEC;
	uint8_t rdstataPayload[8];
	uint8_t rdstataPayload_PECflag;
	
	uint8_t attempts = 0;

	rdstataCmd[0] = 0x00;
	rdstataCmd[1] = 0x10;
	rdstataCmd_PEC = compute_PEC15(rdstataCmd, 2);
	append_PEC(rdstataCmd, 2, rdstataCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdstataCmd, rdstataPayload, side);
		
		rdstataPayload_PECflag = verify_PEC15(rdstataPayload);
		
		if(rdstataPayload_PECflag == 2 && rdstataPayload[5] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->statRegisterA[i] = rdstataPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDSTATB(uint8_t *statusRegisterGroupB_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x12;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, statusRegisterGroupB_ptr, side);
}*/
void RDSTATB(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdstatbCmd[4];
	uint16_t rdstatbCmd_PEC;
	uint8_t rdstatbPayload[8];
	uint8_t rdstatbPayload_PECflag;
	
	uint8_t attempts = 0;

	rdstatbCmd[0] = 0x00;
	rdstatbCmd[1] = 0x12;
	rdstatbCmd_PEC = compute_PEC15(rdstatbCmd, 2);
	append_PEC(rdstatbCmd, 2, rdstatbCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdstatbCmd, rdstatbPayload, side);
		
		rdstatbPayload_PECflag = verify_PEC15(rdstatbPayload);
		
		if(rdstatbPayload_PECflag == 2 && rdstatbPayload[1] != 0xFF) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->statRegisterB[i] = rdstatbPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}


/*void RDPWM(uint8_t *PWMRegisterGroup_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x22;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, PWMRegisterGroup_ptr, side);
}*/
void RDPWM(LTC6813 *asicRegisters, uint8_t side)
{
	uint8_t rdpwmCmd[4];
	uint16_t rdpwmCmd_PEC;
	uint8_t rdpwmPayload[8];
	uint8_t rdpwmPayload_PECflag;

	rdpwmCmd[0] = 0x00;
	rdpwmCmd[1] = 0x22;
	rdpwmCmd_PEC = compute_PEC15(rdpwmCmd, 2);
	append_PEC(rdpwmCmd, 2, rdpwmCmd_PEC);

	while(attempts < 5) {
		read_cmd(rdpwmCmd, rdpwmPayload, side);
		
		rdpwmPayload_PECflag = verify_PEC15(rdpwmPayload);
		
		if(rdpwmPayload_PECflag == 2) {
			for(uint8_t i = 0; i < 6; i++) { asicRegisters->pwmRegister[i] = rdpwmPayload[i]; }
			attempts = 10;
		}
		else {
			attempts++;
			HAL_Delay(1);
		}
	}
	if(attempts != 10) { error(ERROR_PEC); }
}
// *** END READ COMMANDS ***

// *** ACTION COMMANDS ***
void ADOW(uint8_t variant, uint8_t side)
{
	uint8_t adowCmd[4];
	uint16_t adowCmd_PEC;

	if (variant) {
		adowCmd[0] = 0x03;
		adowCmd[1] = 0x28;
	}
	else {
		adowCmd[0] = 0x03;
		adowCmd[1] = 0x68;
	}

	adowCmd_PEC = compute_PEC15(adowCmd, 2);
	append_PEC(adowCmd, 2, adowCmd_PEC);

	action_cmd(adowCmd, side);
}


void CVST(uint8_t variant, uint8_t side)
{
	uint8_t cvstCmd[4];
	uint16_t cvstCmd_PEC;

	if (variant) {
		cvstCmd[0] = 0x03;
		cvstCmd[1] = 0x27;
	}
	else {
		cvstCmd[0] = 0x03;
		cvstCmd[1] = 0x47;
	}

	cvstCmd_PEC = compute_PEC15(cvstCmd, 2);
	append_PEC(cvstCmd, 2, cvstCmd_PEC);

	action_cmd(cvstCmd, side);
}


void STATST(uint8_t variant, uint8_t side)
{
	uint8_t statstCmd[4];
	uint16_t statstCmd_PEC;

	if (variant) {
		statstCmd[0] = 0x05;
		statstCmd[1] = 0x2F;
	}
	else {
		statstCmd[0] = 0x05;
		statstCmd[1] = 0x4F;
	}

	statstCmd_PEC = compute_PEC15(statstCmd, 2);
	append_PEC(statstCmd, 2, statstCmd_PEC);

	action_cmd(statstCmd, side);
}


void ADSTATD(uint8_t side)
{
	uint8_t adstatdCmd[4];
	uint16_t adstatdCmd_PEC;

	adstatdCmd[0] = 0x05;
	adstatdCmd[1] = 0x08;
	adstatdCmd_PEC = compute_PEC15(adstatdCmd, 2);
	append_PEC(adstatdCmd, 2, adstatdCmd_PEC);

	action_cmd(adstatdCmd, side);
}


void ADAXD(uint8_t side)
{
	uint8_t adaxdCmd[4];
	uint16_t adaxdCmd_PEC;

	adaxdCmd[0] = 0x05;
	adaxdCmd[1] = 0x00;
	adaxdCmd_PEC = compute_PEC15(adaxdCmd, 2);
	append_PEC(adaxdCmd, 2, adaxdCmd_PEC);

	action_cmd(adaxdCmd, side);
}


void ADCV(uint8_t side)
{
	uint8_t adcvCmd[4];
	uint16_t adcvCmd_PEC;

	adcvCmd[0] = 0x03;
	adcvCmd[1] = 0x70;
	adcvCmd_PEC = compute_PEC15(adcvCmd, 2);
	append_PEC(adcvCmd, 2, adcvCmd_PEC);

	action_cmd(adcvCmd, side);
}


void CLRCELL(uint8_t side)
{
	uint8_t clrcellCmd[4];
	uint16_t clrcellCmd_PEC;

	clrcellCmd[0] = 0x07;
	clrcellCmd[1] = 0x11;
	clrcellCmd_PEC = compute_PEC15(clrcellCmd, 2);
	append_PEC(clrcellCmd, 2, clrcellCmd_PEC);

	action_cmd(clrcellCmd, side);
}


void CLRSTAT(uint8_t side)
{
	uint8_t clrstatCmd[4];
	uint16_t clrstatCmd_PEC;

	clrstatCmd[0] = 0x07;
	clrstatCmd[1] = 0x13;
	clrstatCmd_PEC = compute_PEC15(clrstatCmd, 2);
	append_PEC(clrstatCmd, 2, clrstatCmd_PEC);

	action_cmd(clrstatCmd, side);
}


void DIAGN(uint8_t side)
{
	uint8_t diagnCmd[4];
	uint16_t diagnCmd_PEC;

	diagnCmd[0] = 0x07;
	diagnCmd[1] = 0x15;
	diagnCmd_PEC = compute_PEC15(diagnCmd, 2);
	append_PEC(diagnCmd, 2, diagnCmd_PEC);

	action_cmd(diagnCmd, side);
}


void MUTE(uint8_t side)
{
	uint8_t muteCmd[4];
	uint16_t muteCmd_PEC;

	muteCmd[0] = 0x00;
	muteCmd[1] = 0x28;
	muteCmd_PEC = compute_PEC15(muteCmd, 2);
	append_PEC(muteCmd, 2, muteCmd_PEC);

	action_cmd(muteCmd, side);
}


void UNMUTE(uint8_t side)
{
	uint8_t unmuteCmd[4];
	uint16_t unmuteCmd_PEC;

	unmuteCmd[0] = 0x00;
	unmuteCmd[1] = 0x29;
	unmuteCmd_PEC = compute_PEC15(unmuteCmd, 2);
	append_PEC(unmuteCmd, 2, unmuteCmd_PEC);

	action_cmd(unmuteCmd, side);
}
// *** END ACTION COMMANDS ***

// HELPER FUNCTIONS
uint8_t force_refup(void)
{
	LTC6813 asicRegisters = {0};
	
	asicRegisters.configRegisterA[0] = 0xFE;
	
	for(uint8_t i = 1; i < 6; i++) { asicRegisters.configRegisterA[i] = 0x00; }

	WRCFGA(&asicRegisters, SIDE_A);
	WRCFGA(&asicRegisters, SIDE_B);
	wait(1);
	
	if(refup_check()) { return 1; }
	
	else { return 0; }
}


uint8_t refup_check(void)
{
	LTC6813 asicRegisters = {0};
	uint8_t refonBit_sideA;
	uint8_t refonBit_sideB;
	
	RDCFGA(&asicRegisters, SIDE_A);
	refonBit_sideA = (asicRegisters.configRegisterA[0] >> 2) & 0x01;
	RDCFGA(&asicRegisters, SIDE_B);
	refonBit_sideB = (asicRegisters.configRegisterA[0] >> 2) & 0x01;
	
	if(refonBit_sideA == 1 && refonBit_sideB == 1) { return 1; }		// Both REFON bits are 1 (both LTCs are in the REFUP state)
	
	else { return 0; }													// One OR none of the REFON bits are 1
}
