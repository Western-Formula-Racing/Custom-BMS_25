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
This is when the BMS is passively balancing the cells. The BMS enters the Balancing state if the empty TORCH_START_BALANCE CAN message is received or if forceBalance is set to 1 on module 1. The following chain of events occur when the BMS is ordered to start balancing:
1. Module 1 transmits the TORCH_EXTRACT_VMIN CAN message. This is another empty message that orders modules 2 through 5 to identify the smallest cell voltage in their series stack. After sending this message, module 1 proceeds to find the smallest cell voltage in its own series stack.
2. Once modules 2 through 5 find their smallest cell voltages, they transmit the TORCH_Mx_VMIN CAN messages (where x is equal to the module's ID). These messages hold the cell voltages as an unsigned 16-bit number.
3. Module 1 extracts the values from each TORCH_Mx_VMIN CAN message and determines the smallest among them. This value represents the lowest cell voltage in the entire battery pack.
4. Once the smallest cell voltage in the pack is found, module 1 encodes it into the first two bytes of the TORCH_GLOBAL_MIN_VCELL CAN message and proceeds to transmit it for five seconds. This forces modules 2 through 5 to enter the Balancing state; they balance their cells with respect to the global minimum cell voltage sent to them by module 1. After five seconds of transmission, module 1 enters the Balancing state.<br>

By default, a cell is identified to be in need of balancing if its voltage is 15 mV greater than the smallest cell voltage in the entire pack. If a module possesses cells in need of balancing, it enters what's known as the balancing cycle. A single cycle has the following stages:
1. The module balances all even indexed cells (cells 2, 4, 6...) for one minute.
2. The module balances all odd indexed cells (cells 1, 3, 5...) for one minute.
3. The module waits twenty seconds for the lithium ions to settle. It then proceeds to read its cell voltages. The twenty second pause gives it a more accurate reading.
4. The module enters the Quick Diagnosis state to verify its internal status.
5. If the module still contains cells in need of balancing, it repeats the cycle. Otherwise it leaves the Balancing state.<br>

Besides power cycling, there are three ways the BMS stops balancing:
1. The empty TORCH_STOP_BALANCE CAN message is received. This forces all balancing modules to stop balancing and enter the Active/Charge state.
2. The AIRs are closed. The BMS cannot continue or initiate balancing if the battery pack is armed. If the AIRs are closed and the TORCH_START_BALANCE CAN message is received, the BMS will ignore the command. If AIRs close while the BMS is balancing, it will abort balancing and enter the Active/Charge state.
3. All cells become within 15 mV of the lowest cell voltage in the pack. Balancing is considered completed when this happens.<br>

When balancing's complete, the module enters the Low Power state. This is to minimize current consumption in case one module is wildly out of balance relative to the others.
