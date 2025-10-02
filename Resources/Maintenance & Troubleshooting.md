# Maintenance & Troubleshooting
This document explains how to maintain and troubleshoot the BMS interface boards.

## Troubleshooting via Error Codes
If the module detects a critical problem, the TORCH_FAULT CAN message is transmitted. The error code defines the reason for the fault. Let's go over each one and see what can be done to resolve them.

### Module Overheat (error code 69)
This error's thrown if any thermistor within the module reads a temperature greater than 60 degrees Celcius. The overheating thermistor(s) will show up as 'Fault' in the TORCH_FAULT CAN message. 
The only solution to this error is to cool down the battery pack.

### Cell Undervolt (error code 70)
This error's thrown if at least one cell in the module falls below 2.5 V. This is the minimum permitted cell voltage specified on the Sony VTC6 data sheet. The undervolt cell(s) will show up as 'Fault' in the TORCH_FAULT CAN message. 
The cause of this error is the remaining energy in the pack getting too low. There are two solutions to this problem:
1. Charge the battery pack.
2. Lower the maximum pack output current / command less torque.

### Cell Overvolt (error code 71)
This error's thrown if at least one cell in the module rises above 4.2 V. This is the maximum permitted cell voltage specified on the Sony VTC6 data sheet. This overvolt cell(s) will show up as 'Fault' in the TORCH_FAULT CAN message. 
A cell's voltage should only be rising while the battery pack is charging. Therefore the only way this error could ever be thrown is if the charge voltage in the motherboard code is set to a value that's greater than 4.2 V. 
The solution to this error is to simply stop charging/set an appropriate charge cut off voltage on the motherboard.

### Open Cell Connection (error code 72)
This error's thrown if the module detects an open circuit on any of its cell pins. The open cell(s) will show up as 'Fault' in the TORCH_FAULT CAN message. 
There are several reasons why this fault can be thrown:
1. A blown cell voltage tap fuse.
2. A weak solder joint on the interface PCB male connector.
3. A weak solder joint on the embedded module PCB female connector.
4. Weak cell voltage tap connection
5. A problem with the LTC6813.<br>

Let's go over each and every one of these scenarios.

#### Blown Cell Voltage Tap Fuse
The cell voltage tap fuses are located on the embedded module boards. Probe it with a multimeter in continuous mode to see if it's blown or not. 
If there's no continuity, then the fuse is blown. Simply replace it and smile that reason #5 wasn't why this error got thrown.

#### Interface PCB Connector Solder Joint
These are the male connectors on the interface PCBs (the ones that connect the PCB to the embedded module PCBs). Like with the fuses, simply probe with a multimeter in continuity. 
If there's no continuity, either replace the connector or fix up the soldering job.

#### Embedded Module PCB Connector Solder Joint
These are the connectors on the embedded module PCB. Again, check for continuity with a multimeter. If that's the source of the issue, either replace it or fix up the solder joint.

#### Weak Cell Voltage Tap Connection
This is the connection between the module collector plates and the embedded module PCB. Again, probe for continuity and perform a visual inspection. 
If this is the cause, fix the soldering job.

#### LTC6813 Problem
If there's perfect continuity between the cell and the interface PCB, then the issue is on the interface PCB itself. This can be verified by swapping interface PCBs between modules. 
For example, if module 3 is throwing fault 72, try putting the interface PCB from module 1 onto module 3. If it works fine, then the issue is certainly on the module 3 interface PCB.<br>

Perform the two troubleshooting steps below in order:
1. Verify that all test pad voltages are within the appropriate range (refer to the 'Troubleshooting via Probing' section below).
2. Between the interface PCB male connector and the LTC6813 pins resides a 100 ohm resistor. Therefore be sure to check continuity between the male connector and the resistor as well as the resistor and the cell voltage pin. **Be sure to remove the interface PCB off the module when probing either LTC6813 directly!**<br>

If nothing above worked, then it's unfortunately time to contact Paul for a new JLC order. In other words, the faulting interface PCB needs to be replaced.

### Open Thermistor Connection (error code 73)
This error gets thrown if an open circuit's detected on the module thermistors. The open thermistor(s) will show up as 'Fault' in the TORCH_FAULT CAN message.<br>

There are several reasons why this error can be thrown:
1. Poor solder connection on the interface PCB.
2. Poor solder connection on the embedded module PCB.
3. Poor solder connection on the module thermistor itself (!!!).<br>

Let's look at what to do about each one of these scenarios:

#### Interface PCB Solder Issue
Confirm that there's a stable solder joint on each of the following connections located on the interface PCB (refer to Altium schematic for exact component locations):
- male connectors
- thermistor multiplexer
- buffer op amp
- thermistor pull up resistors and series resistors used in RC filters<br>

If everything above is continuous, then the interface PCB is likely not the cause.

#### Embedded Module PCB Solder Issue
Verify the solder joint of the female connectors on the embedded module PCB. If there's a problem, either replace the connector or fix up the soldering job.

#### Module Thermistor Issue
If this truly is the source of the problem, then there's realistically nothing that can be done. The module thermistors are deeply embedded within the module; there is no way to safely access them.<br>

When the embedded module boards were being assembled, great care was taken with soldering the thermistors. Continuity was checked for each and every one before the PCB was integrated into the module. 
At the time of writing, a module's thermistor has never been disconnected. The only way this could really happen is if the module undergoes some serious trauma (i.e. dropping a module, getting into a heavy accident, etc.).

#### LTC6813 Multiplexer Fail (error code 74)
This error gets thrown if either LTC6813 fails to pass the DIAGN self-test command. This command only gets called during the Full Diagnosis state.<br>

If this error gets thrown, perform the following steps:
1. Turn power off.
2. If the interface PCB is hot, give it a few minutes to cool. Otherwise, proceed to step 3.
3. Wait 15 or so seconds.
4. Turn power on.

Try power cycling-waiting four times. If the error is still getting thrown, ensure that the interface PCB is properly seated on the embedded module PCB. 
If it's properly seated, probe the interface PCB test pads (refer to 'Troubleshooting via Probing' section below). If the error will simply not go away, then the problematic LTC6813 can legally be declared dead.

#### LTC6813 Mute Fail (error code 75)
This error gets thrown if the LTC6813 fails to mute/unmute itself (i.e. enable/disable cell balancing). The steps to resolving this are identical to the steps for resolving error code 74.

#### LTC6813 Cell Voltage Register Fault (error code 76)
This error gets thrown if either LTC6813 fails to pass the CVST self-test command. This command only gets called during the Full Diagnosis state.<br>

A consistent throwing of this error means that the registers used to hold cell voltage values are corrupted (i.e. dead LTC6813). The steps to resolve this are identical to the steps outlined for error code 74.

#### LTC6813 Status Register Fault (error code 77)
This error gets thrown if either LTC6813 fails to pass the STATST self-test command. This command only gets called during the Full Diagnosis state.<br>

A consistent throwing of this error means that the status registers are corrupted (i.e. dead LTC6813). The steps to resolve this are identical to the steps outlined for error code 74.

#### LTC6813 Auxiliary Register Fault (error code 78)
This error gets thrown if either LTC6813 fails to pass the AXST self-test command. This command only gets called during the Full Diagnosis state.<br>

A consistent throwing of this error means that the auxiliary registers are corrupted (i.e. dead LTC6813). The steps to resolve this are identical to the steps outlined for error code 74.

#### LTC6813 ADC Fault (error code 79)
This error gets thrown if either LTC6813 fails to pass the ADOL self-test command. This command gets called during the Full Diagnosis and Quick Diagnosis states.<br>

A consistent throwing of this error means that one of the three ADCs are not working properly (i.e. dead LTC6813). The steps to resolve this are identical to the steps outlined for error code 74.

## Troubleshooting via Probing