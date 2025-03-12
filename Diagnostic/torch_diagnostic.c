#include "main.h"
#include "torch_main.h"
#include "torch_diagnostics.h"
#include "torch_LTC6813.h"			// MAKE THIS torch_LTC6813.h later

// This function runs the test commands on both LTC6813s at the start of runtime
void diagnostic_LTC6813(uint16_t *diagnosisResults_ptr)
{
	// ADSTAT
		// Measures VA (4.5 < VA < 5.5)
		// Measures VD (2.7 < VD < 3.6)
		// Measures ITMP (-40 < ITMP < 125) (.. add later. Cells are balanced externally. Internal heat gen's neglible (prioritize other stuff!!))
	// ADOW
		// Open circuit on C pins test (page 32)
	// ADAX
		// Measures VREF2 (2.99 < VREF2 < 3.014)
	// CVST
		// Tests ADC & cell voltage registers
	// STATST
		// Tests ADC & status registers
	// AXST
		// Tests ADC & aux registers
			// ONLY check AUXD for VREF2
	// DIAGN
		// Tests the multiplexers
	// VARS WE WANNA GIVE BACK
		// VA (16 bits)
		// VD (16 bits)
		// VREF2 (16 bits)
		// CVST - C1, variant 1 (16 bits)
		// CVST - C1, variant 2 (16 bits)
		// CVST - C2, variant 1 (16 bits)
		// CVST - C2, variant 2 (16 bits)
		// CVST - C3, variant 1 (16 bits)
		// CVST - C3, variant 2 (16 bits)
		// CVST - C4, variant 1 (16 bits)
		// CVST - C4, variant 2 (16 bits)
		// CVST - C5, variant 1 (16 bits)
		// CVST - C5, variant 2 (16 bits)
		// CVST - C6, variant 1 (16 bits)
		// CVST - C6, variant 2 (16 bits)
		// CVST - C7, variant 1 (16 bits)
		// CVST - C7, variant 2 (16 bits)
		// CVST - C8, variant 1 (16 bits)
		// CVST - C8, variant 2 (16 bits)
		// CVST - C9, variant 1 (16 bits)
		// CVST - C9, variant 2 (16 bits)
		// CVST - C10, variant 1 (16 bits)
		// CVST - C10, variant 2 (16 bits)
		// STATST - ITMP, variant 1 (16 bits)
		// STATST - ITMP, variant 2 (16 bits)
		// STATST - VA, variant 1 (16 bits)
		// STATST - VA, variant 2 (16 bits)
		// STATST - VD, variant 1 (16 bits)
		// STATST - VD, variant 2 (16 bits)
		// AXST - VREF2, variant 1 (16 bits)
		// AXST - VREF2, variant 2 (16 bits)
		// STATUS (16 bits)
			// C1 - C10 open; 1 means good, 0 means open circuit (10 bits)
			// MUXFAIL; 0 means good, 1 means test failed (1 bit)
			// remaining 5 bits are don't cares
		// GOTTA PASS A uint16_t big[64] array into this (32 for side A, 32 for side B)
}