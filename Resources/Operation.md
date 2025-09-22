# Theory of Operation
## States
The BMS state diagram is depicted below. Let's go over what each and every one of these operating states mean.

### Full Diagnosis
Upon power up, the BMS performs a full diagnosis of itself. If an internal issue is detected, the BMS enters the error state. The following self-tests are performed in this exact order.

#### LTC6813 Multiplexer Decoder Check
Validates the internal multiplexer channels of both LTC6813s. If an issue is detected, error code 74 is thrown.

#### LTC6813 Digital Filter Check: Cell Voltage Registers
Validates the internal cell voltage registers. If an issue is detected, error code 76 is thrown.

#### LTC6813 Digital Filter Check: Auxiliary Registers
Validates the internal auxiliary (GPIO) registers. If an issue is detected, error code 78 is thrown.

#### LTC6813 Digital Filter Check: Status Registers
Validates the internal status registers. If an issue is detected, error code 77 is thrown.

#### LTC6813 ADC Check
Validates all three internal ADCs. If an issue is detected, error code 79 is thrown.

#### LTC6813 Analog Supply Check
Checks if the analog supply voltage is within range. If it's out of range, error code 80 is thrown.

#### LTC6813 Digital Supply Check
Checks if the digital supply voltage is within range. If it's out of range, error code 81 is thrown.

#### LTC6813 Reference Check
Checks if the second reference voltage is within range. If it's out of range, error code 82 is thrown.

#### LTC6813 Temperature Check
Checks the internal die temperature of the chip. If it's too large, error code 83 is thrown.

#### Open Cell Check
Checks for open circuits on the cell voltage connections. If an open circuit's detected, error code 72 is thrown.

#### Open Thermistor Check
Checks for open circuits on the module thermistor connections. If an open circuit's detected, error code 73 is thrown.

The full diagnosis takes around 120 to 150 milliseconds to complete. If all the tests pass, then the BMS enters its primary loop.

### Active/Charge
This is the primary loop of the BMS. Here it's periodically reading cell voltages, module temperatures, and transmitting its data over CAN.
It's worth noting that from the STM32 program's perspective, Active and Charge are practically identical states; the BMS continues reading module parameters
and transmitting data at the same frequency. The blue 'CHARGE' LED turns on when the pack's charging, while the green 'ACTIVE' LED is on in all other cases (i.e. Idle, Active, Precharge).

### Quick Diagnosis
This is a less extensive self-diagnosis that unlike the full diagnosis, occurs periodically during runtime. Here the BMS checks the LTC6813 analog supply,
digital supply, ADCs, open cell connections, and open thermistor connections. The BMS performs this quick diagnosis every 10 seconds while it's in the Active/Charge
state as well as once at the end of every balancing cycle.

### Balancing
