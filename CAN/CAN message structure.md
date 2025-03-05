# CAN Message Structure (WIP)
This section goes over the types and structure of CAN messages that will be transmitted and received by the TORCH MCU.

## Transmission
The MCU present on each of the accumulator modules will be sending two types of messages:
- Status messages. These include noteworthy parameters of each module (i.e. SOC), as well as error bits in the case of a fault
- Data messages. These include parameters such as the voltage of each cell, temperature sensors, etc.

### Status Message
The status message is primarily meant to be interpreted by the accumulator motherboard (MOBO) and the vehicle's front electronic control unit (ECU). The status message
is considered to be a high-priority message, as it defines the status of the TORCH hardware and its corresponding module. The table below defines the structure of the
status message. The bit functions are described in detail just below the table.

| Bit Index      | Bit Function |
| :----:        |    :----:   |
| 64   | Module ID                |
| 63   | Module ID                |
| 62   | Module ID                |
| 61   | Temperature limit flag   |
| 60   | Cell undervoltage flag   |
| 59   | Cell overvoltage flag    |
| 58   | Cell open circuit flag   |
| 57   | Temperature sensor open circuit flag |
| 56   | LTC6813 cell voltage ADC fault flag  |
| 55   | LTC6813 status ADC fault flag  |
| 54   | LTC6813 MUX fault flag |
| 53   | LTC6813 out of range reference voltage flag   |
| 52   | STM32 SPI fault flag  |
| 51   | STM32 ADC fault flag  |
| 50   | State of charge |
| 49   | State of charge |
| 48   | State of charge |
| 47 - 1   | **Situational** |

#### Module ID
Module ID is simply the module that is sending the message. Because the 2025 WFR EV has 5 modules, there are 5 possible combinations.
- 001 -> Module 1
- 010 -> Module 2
- 011 -> Module 3
- 100 -> Module 4
- 101 -> Module 5
Note that in the list above, the MSb is bit 64 (i.e. for Module 3, bit 64 = 0, bit 63 = 1, and bit 61 = 1).

#### &lt;Bit title&gt; flag
Bits 61 to 51 are error flags. These bits define whether the MOBO should shutdown the vehicle by opening the AMS relay.
The perfectly-working BMS/accumulator has all of these bits set to 1. **If any of these bits are 0, then there is a problem that warrants a vehicle shutdown.**
Here's a description of what each flag means:
- Temperature limit flag means that a thermistor in the module has reached or surpassed 60 degrees celcius
- Cell undervoltage flag means that the voltage of a cell is equal to or lesser than 2.5 V
- Cell overvoltage flag means that the voltage of a cell is equal to or greater than 4.2 V
- Cell open circuit flag means that there is an open circuit detected on one of the C-pins of the LTC6813
- Temperature sensor open circuit flag means that there is an open circuit detected on one of the thermistor connections to the STM32
- LTC6813 cell voltage ADC fault flag means that the internal ADC responsible for cell voltage digitization is not working properly
- LTC6813 status ADC fault flag means that the internal ADC responsible for status parameter digitization is not working properly
- LTC6813 MUX fault flag means that the internal multiplexer(s) aren't working properly
- LTC6813 out of range reference voltage flag means that the internally generated reference voltage for the ADCs is out of its specified range
- STM32 SPI fault flag means that the STM32 is not able to transmit or receive SPI messages
- STM32 ADC fault flag means that the internal ADC of the STM32 is not working properly

#### State of charge
The TORCH uses a voltage-based state of charge (SOC) estimator. The voltage of the lowest cell is used in the calculation, as that is the limiting factor.
The SOC is discrete with 5 levels:
- 101 -> Level 5
  - Cell voltage is 3.800 V - 4.200 V
- 100 -> Level 4
  - Cell voltage is 3.400 V - 3.799 V
- 011 -> Level 3
  - Cell voltage is 3.000 V - 3.399 V
- 010 -> Level 2
  - Cell voltage is 2.600 V - 2.999 V
- 001 -> Level 1
  - Cell voltage is less than 2.600 V

An SOC of Level 1 indicates that the module is very close to critical charge depletion ('dead'), and should therefore be charged before further use.

### Data Message
DAQ messages are primarily meant for the custom BMS GUI when a user is plugged in, as well as for general data collection by the vehicle's data acquisition (DAQ) 
system. Unlike status messages, data messages are considered to be low-priority.
