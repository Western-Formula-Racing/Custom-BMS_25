#include "main.h"
#include "torch_main.h"
#include "torch_stm32.h"
#include "torch_setup.h"
#include "torch_ltc6813.h"


void bms_initialization(void)
{
	if(!refup_check()) { force_refup(); }
	
	voltage_sense();
}