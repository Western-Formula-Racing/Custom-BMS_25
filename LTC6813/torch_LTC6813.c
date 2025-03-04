#include "main.h"
#include "torch_LTC6813.h"

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

	if (receivedPEC == calculatedPEC) { return 1; }

	else { return 0; }
}


void action_cmd(uint8_t *cmd_ptr, uint8_t sideA)
{
	// TO LTC SIDE A (CELL 1 to 10)
	if (sideA) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	}

	// TO LTC SIDE B (CELL 11 to 20)
	else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi3, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	}
}


void write_cmd(uint8_t *cmd_ptr, uint8_t *payload_ptr, uint8_t sideA)
{
	// TO LTC SIDE A (CELL 1 to 10)
	if (sideA) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_SPI_Transmit(&hspi1, payload_ptr, PAYLOAD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	}

	// TO LTC SIDE B (CELL 11 to 20)
	else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi3, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_SPI_Transmit(&hspi3, payload_ptr, PAYLOAD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	}
}

void read_cmd(uint8_t *cmd_ptr, uint8_t *receivedPayload_ptr, uint8_t sideA)
{
	uint8_t dummies[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	// TO LTC SIDE A (CELL 1 to 10)
	if (sideA) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_SPI_TransmitReceive(&hspi1, dummies, receivedPayload_ptr, PAYLOAD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	}

	// TO LTC SIDE B (CELL 11 to 20)
	else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi3, cmd_ptr, CMD_LEN, HAL_MAX_DELAY);
		HAL_SPI_TransmitReceive(&hspi3, dummies, receivedPayload_ptr, PAYLOAD_LEN, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	}
}

// *** WRITE COMMANDS ***
void WRCFGA(uint8_t *payload_ptr, uint8_t side)
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
}


void WRCFGB(uint8_t *payload_ptr, uint8_t side)
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
}


void WRSCTRL(uint8_t *payload_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;
	uint8_t registerGroupSControl[8];
	uint16_t registerGroupSControl_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x14;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	for(uint8_t i = 0; i < 6; i ++) {
		registerGroupSControl[i] = *(payload_ptr + i);
	}
	registerGroupSControl_PEC = compute_PEC15(registerGroupSControl, 6);
	append_PEC(registerGroupSControl, 6, registerGroupSControl_PEC);

	write_cmd(cmd, registerGroupSControl, side);
}


void WRPWM(uint8_t *payload_ptr, uint8_t side)
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
}


void WRPSB(uint8_t *payload_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;
	uint8_t registerGroupPWMandSControl[8];
	uint16_t registerGroupPWMandSControl_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x1C;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	for(uint8_t i = 0; i < 6; i ++) {
		registerGroupPWMandSControl[i] = *(payload_ptr + i);
	}
	registerGroupPWMandSControl_PEC = compute_PEC15(registerGroupPWMandSControl, 6);
	append_PEC(registerGroupPWMandSControl, 6, registerGroupPWMandSControl_PEC);

	write_cmd(cmd, registerGroupPWMandSControl, side);
}
// *** END WRITE COMMANDS ***


// *** READ COMMANDS ***
void RDCFGA(uint8_t *configRegisterGroupA_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x02;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, configRegisterGroupA_ptr, side);
}


void RDCFGB(uint8_t *configRegisterGroupB_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x26;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, configRegisterGroupB_ptr, side);
}


void RDCVA(uint8_t *cellVoltageRegisterGroupA_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x04;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupA_ptr, side);
}


void RDCVB(uint8_t *cellVoltageRegisterGroupB_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x06;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupB_ptr, side);
}


void RDCVC(uint8_t *cellVoltageRegisterGroupC_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x08;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupC_ptr, side);
}


void RDCVD(uint8_t *cellVoltageRegisterGroupD_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x0A;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupD_ptr, side);
}


void RDCVE(uint8_t *cellVoltageRegisterGroupE_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x09;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupE_ptr, side);
}


void RDCVF(uint8_t *cellVoltageRegisterGroupF_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x0B;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, cellVoltageRegisterGroupF_ptr, side);
}


void RDSTATA(uint8_t *statusRegisterGroupA_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x10;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, statusRegisterGroupA_ptr, side);
}


void RDSTATB(uint8_t *statusRegisterGroupB_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x12;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, statusRegisterGroupB_ptr, side);
}


void RDAUXB(uint8_t *auxiliaryRegisterGroupB_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x0E;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, auxiliaryRegisterGroupB_ptr, side);
}


void RDSCTRL(uint8_t *SControlRegisterGroup_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x16;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, SControlRegisterGroup_ptr, side);
}


void RDPWM(uint8_t *PWMRegisterGroup_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x22;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, PWMRegisterGroup_ptr, side);
}


void RDPSB(uint8_t *PWMandSControlRegisterGroup_ptr, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x1E;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	read_cmd(cmd, PWMandSControlRegisterGroup_ptr, side);
}
// *** END READ COMMANDS ***

// *** ACTION COMMANDS ***
void STSCTRL(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x19;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void CLRSCTRL(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x18;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void ADOW(uint8_t variant, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	if (variant) {
		cmd[0] = 0x03;
		cmd[1] = 0x28;
	}
	else {
		cmd[0] = 0x03;
		cmd[1] = 0x68;
	}

	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void CVST(uint8_t variant, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	if (variant) {
		cmd[0] = 0x03;
		cmd[1] = 0x27;
	}
	else {
		cmd[0] = 0x03;
		cmd[1] = 0x47;
	}

	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void STATST(uint8_t variant, uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	if (variant) {
		cmd[0] = 0x05;
		cmd[1] = 0x2F;
	}
	else {
		cmd[0] = 0x05;
		cmd[1] = 0x4F;
	}

	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void ADSTAT(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x05;
	cmd[1] = 0x68;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void ADAX(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x05;
	cmd[1] = 0x66;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void ADCVSC(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x05;
	cmd[1] = 0x67;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void CLRCELL(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x07;
	cmd[1] = 0x11;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void CLRSTAT(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x07;
	cmd[1] = 0x13;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void DIAGN(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x07;
	cmd[1] = 0x15;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void MUTE(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x28;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}


void UNMUTE(uint8_t side)
{
	uint8_t cmd[4];
	uint16_t cmd_PEC;

	cmd[0] = 0x00;
	cmd[1] = 0x29;
	cmd_PEC = compute_PEC15(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	action_cmd(cmd, side);
}
// *** END ACTION COMMANDS ***
