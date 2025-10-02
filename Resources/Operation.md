# Theory of Operation

## States
The BMS state diagram is depicted below. Let's go over what each and every one of these operating states mean.

### Full Diagnosis
Upon power up, the BMS performs a full diagnosis of itself. If an internal issue is detected, the BMS enters the error state. All LEDs turn on while the self-diagnosis is being performed. The following self-tests are performed in this exact order.

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

### Monitoring
This is the primary loop of the BMS. Here it's solely periodically reading cell voltages, module temperatures, and transmitting its data over CAN. 
The blue 'CHARGE' LED turns on when the pack's charging, while the green 'ACTIVE' LED is on in all other cases (i.e. Idle, Active, Precharge).

### Quick Diagnosis
This is a less extensive self-diagnosis that unlike the full diagnosis, occurs periodically during runtime. Here the BMS checks the LTC6813 analog supply,
digital supply, ADCs, open cell connections, and open thermistor connections. The BMS performs this quick diagnosis every 10 seconds while it's in the Monitoring
state as well as once at the end of every balancing cycle. Like the Full Diagnosis state, all LEDs turn on while the self-diagnosis is being performed.

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

The 'Balancing' LED turns on in stages 1 and 2 of the balancing cycle outlined above; the LED is turned off during the twenty second pause. 
The TORCH_Mx_BALANCE (where x is equal to the module's ID) CAN message is sent during the entire balancing cycle (in other words, it's consistently sent while the module's in the Balancing state).<br>

Each module measures the temperature of 18/20 cell balancing resistors. Resistors Rx and Ry do not have their temperatures read due to the LTC6813 only having nine GPIO pins. 
To compensate for this, resistors Rx and Ry are placed far from the other resistors with a generous air gap surrounding them. This ensures that they stay cooler than the other resistors. 
If a balancing resistors surface temperature surpases 45 degrees Celcius, the 'HOT!' LED will turn on. If a balancing resistor's surface temperature surpasses 115 degrees Celcius, the BMS will skip its balancing cycle and let it cool down. The BMS will not throw a fault due to an overheating balance resistor.


Besides power cycling, there are four ways the BMS stops balancing:
1. The empty TORCH_STOP_BALANCE CAN message is received. This forces all balancing modules to stop balancing and enter the Monitoring state.
2. At least one of the modules transmit the TORCH_FAULT message for any reason.
3. The AIRs are closed. The BMS cannot continue or initiate balancing if the battery pack is armed. If the AIRs are closed and the TORCH_START_BALANCE CAN message is received, the BMS will ignore the command. If AIRs close while the BMS is balancing, it will abort balancing and enter the Monitoring state.
4. All cells become within 15 mV of the lowest cell voltage in the pack. Balancing is considered completed when this happens.<br>

When balancing's complete, the module enters the Low Power state. This is to minimize current consumption in case one module is wildly out of balance relative to the others.

### Low Power
As its name suggest, the Low Power state ensures that the module consumes the minimal possible current from the tractive system. This is acheived by forcing the LTC6813s to enter their SLEEP state. 
Each LTC6813 consumes about 6 uA while in this state, bringing the total current consumption from the tractive system to 12 uA per module. This state can only be entered if the variable bmsMode is not equal to 1 or if cell balancing is completed; the BMS will not enter this state of balancing is stopped by the stop command or by closing AIRs. 
No CAN messages or measurements are performed in the Low Power state; the module simply blinks with a frequency of 1 Hz for eternity.

### Error
The Error state is entered if any of the following occur:
1. A module detects an internal or external error.
2. One of the modules faulted and is transmitting the TORCH_FAULT CAN message.
3. PackStatus is set to 'Fault'.<br>

The BMS continues to measure cell voltages and module temperatures and transmit its readings over CAN while in the Error state; its behavior in this state is very similar to the Monitoring state. 
The major differences between Error and Monitoring are:
1. The BMS cannot change to the diagnosis states or perform cell balancing.
2. Both the 'ACTIVE' and 'CHARGE' LEDs are on (there is no other state where ONLY these two LEDs are on).
3. The faulting module is transmitting the TORCH_FAULT CAN message. This message explains why the fault occurred.<br>

Note that the non-faulting modules do not transmit the TORCH_FAULT CAN message. For example, if module 3 overheats, it will be the only module transmitting TORCH_FAULT. 
If the motherboard throws a Precharge Failed fault, then none of the modules will be transmitting the TORCH_FAULT CAN message; they'll just comply to points 1 and 2 above. 
The only way for a module to escape the Error state is by power cycling.