#include "main.h"
#include "torch_LTC6813.h"
uint16_t PEC15_table[256];
uint16_t CRC15_Poly = 0x4599;
//SPI_HandleTypeDef hspi1;			!! Looks like this is not necessary for it to work !!


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

uint16_t compute_PEC15(char *dataPtr, uint8_t len)
{
  uint16_t PEC15_seed = 16;
  uint16_t address;

  for (uint16_t i = 0; i < len; i++) {
    address = ((PEC15_seed >> 7) ^ dataPtr[i]) & 0xFF;
    PEC15_seed = (PEC15_seed << 8 ) ^ PEC15_table[address];
  }
  return (PEC15_seed * 2);
}

void append_PEC(uint8_t *dataPtr, uint8_t len, uint16_t PEC)
{
  dataPtr[len] = (PEC >> 8) & 0xFF;
  dataPtr[len + 1] = PEC & 0xFF;
}

/*
	LET'S SEE IF WE CAN PUT THE LTC6813 INTO THE 'REFUP' STATE
*/
void torch_main(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);	// Pulls PB14 high. This is the !SS line for the LTC6820. Despite the pull up resistor, this should be called at the start... every time
	setup_PEC15();											// Makes CRC lookup table. Only has to be called once

	/*
		Four lines below set up the protocol for write commands (page 59, Table 33).
		EVERY Write command is going to be setup this way.
	*/
	uint8_t cmd[4];
	uint8_t payloadRegisterA[8];		// Represents Configuration Register Group A (page 63, Table 38) + PEC code
	uint16_t cmd_PEC;
	uint16_t payloadRegisterA_PEC;

	// Four lines below setup WRCFGA command (page 60, first row of Table 36) + its PEC
	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd_PEC = PEC15_code(cmd, 2);
	append_PEC(cmd, 2, cmd_PEC);

	// Six lines below set up the Configuration Register Group A modification
	// Note that for REFUP state, we need to set the REFON bit (Bit 2 in CFGAR0) to 1
	payloadRegisterA[0] = 0xFE;
	for (uint8_t i = 1; i <= 5; i++) {
		payloadRegisterA[i] = 0x00;
	}
	payloadRegisterA_PEC = PEC15_code(payloadRegisterA, 6);
	append_PEC(payloadRegisterA, 6, payloadRegisterA_PEC);

	// Four lines below send the packets to the LTC6820
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);			// Pulls the !SS line (PB14) low, waking up the LTC6820
	HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY);				// Sends cmd + PEC
	HAL_SPI_Transmit(&hspi1, payloadRegisterA, 8, HAL_MAX_DELAY);	// Sends Configuration Register Group A payload + PEC
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);			// Pulls !SS line high, ending communication

	/*
		!! WORKFLOW FOR ALL WRITE COMMANDS !!
			- Setup cmd array & its PEC
				- always of type uint8_t, 2 bytes for CMD, 2 bytes for PEC
			- Setup register group array & its PEC
				- every register group has 6 registers, therefore always a uint8_t array of size 8 (2 bytes for PEC)
			- Use HAL_GPIO_WritePin & HAL_SPI_Transmit to send the goods
	*/
	
	// Blink once transmission's done... forever...
	while (1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(1000);
	}
}

