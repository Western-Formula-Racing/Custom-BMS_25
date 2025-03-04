# LTC6813 Library
This section has the code that allows the TORCH MCU to communicate with the LTC6813s. Not all commands of the LTC6813 have to be used here; refer to the 'LTC6813 Summary' spreadsheet to see a list of the required commands. For more information on the ASIC and how it works, refer to its datasheet.

## Library Overview
For the 2025 WFR EV, the code is tailored for the STM32F446RET6 microcontroller. Communication between the MCU and LTC6813 is done over isoSPI, an ADI developed communication protocol that provides galvanic isolation throug the use of pulse transformers. In order for isoSPI to work, the MCU must communicate with the isoSPI transceiver: the LTC6820. Communication between the MCU and the LTC6820 is done over SPI.

The 'LTC6813 Summary' contains all the commands that may be used on the TORCH; for all LTC6813 commands, reference its datasheet -> Table 36 (page 60). Referring to the spreadsheet's 'Commands' sheet, the 'Code' column contains the SPI pattern that must be sent to the LTC6820. 

For example, to order the LTC6813 internal ADCs to start digitizing and interpreting the cell voltages, we use the ADCVSC command. The code for this command is 0x0567. Therefore, we tell the STM32 (or **any** microcontroller, provided the circuitry is designed properly) to broadcast 0x0567 on its SPI channel that is connected to the LTC6820. The isoSPI portion between the LTC6813 and LTC6820 is abstracted away from the programmer.

## Command Groups
The library partitions the LTC6813 commands into three groups: write, read, and action. All commands are 2 bytes in length.

### Write Commands
Here the MCU writes data to one of the register groups of the LTC6813. This is used to configure the ASIC (i.e. set cell undervoltage thresholds). Each register group of the LTC6813 has six registers, and each register has 8 bits. Therefore, in C terms, for **every** write command, the MCU shall be sending an uint8_t payload[6] array.

*If you look through the code, you'll notice that the MCU actually sends uint8_t arrays of size **8**. This is due to the PEC (see Error Detection section below).*

### Read Commands
As the name suggests, the MCU is reading data from the LTC6813 for all read commands. The LTC6813 can only send back an entire register group during reading; it is not possible to read a single register from a group. Thus, the reception payload is always an array of size 6.

### Action Commands
These commands tell the LTC6813 to do something. It could be to begin digitizing the cell voltages (ADCVSC) or to perform internal diagnostics (ADAX). No data is written to or received from the LTC6813.

## Error Detection
The LTC6813 has a 16 bit cyclic redundancy check (CRC) to validate the transmission and reception of messages. The workings of this CRC algorithm are explained on page 54 of its datasheet. However, one does not really need to know *how* this algorithm works to implement it. In fact, page 78 of the datasheet provides the code for implementation.

The only noteworthy thing here is that every command must contain the PEC in order for the LTC6813 to accept it. That's why in the code all commands are *4* bytes in length, and all write/read payloads are *8* bytes in length. The extra 2 bytes appended at the end of the commands and payloads are the PEC.
