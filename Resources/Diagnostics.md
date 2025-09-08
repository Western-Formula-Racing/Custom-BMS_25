# Diagnostics (WIP)
This section's about diagnostics and error handling.

## LTC6813 Diagnostics
The LTC6813 has the following diagnostic commands:
- ADOW. This checks for an open circuit on the C pins
- CVST. Validates *sigma* delta ADC for the cell voltage pins
- STATST. Validates the *sigma* delta ADC for the sum of cells (SC), internal IC temperature (ITMP), analog voltage (VA), digital voltage (VD)
- ADAX. Measures the 2nd reference voltage (VREF2)
- DIAGN. Validates the multiplexers
