# BMS CAN Messaging

There are 64 bits per CAN message

# First 16 messages (header)

| order | bit digit | description                        | information                                                  |
| ----- | --------- | ---------------------------------- | ------------------------------------------------------------ |
| 1     | 64        | Module                             | (we only have 5 modules total (1 digit))                     |
| 2     | 63        | Module                             | 000, 010, 011, 100, 101                                      |
| 3     | 62        | Module                             |                                                              |
| 4     | 61        | Message Type                       | 00 voltage 01 temp 11 current (reserved)                     |
| 5     | 60        | Message Type                       | 00 voltage 01 temp                                           |
| 6     | 59        | Cell Combination/Temp Probe Number | For cell group 1 to 7; for temperature group  1 to 3         |
| 7     | 58        | Cell Combination/Temp Probe Number | 000 is cell 1 to 3<br />110 is cell 19 & 20 (highest, incomplete group) |
| 8     | 57        | Cell Combination/Temp Probe Number | 000 is temperature 1 to 6<br />110 is temperature 13 to 18   |
| 9     | 56        | Skip                               | 0                                                            |
| 10    | 55        | Skip                               | 0                                                            |
| 11    | 54        | Skip                               | 0                                                            |
| 12    | 53        | Skip                               | 0                                                            |
| 13    | 52        | Skip                               | 0                                                            |
| 14    | 51        | Skip                               | 0                                                            |
| 15    | 50        | Status                             | 01 active, 10 charge, 11 debug                               |
| 16    | 49        | Status                             | 01 active, 10 charge, 11 debug                               |

# Data (the later 48 bits)

Cell voltage is 4 digits of decimal (16 bits, like 3.234V)

temperature is 3 digits of decimal (8 bits, in full degree celsius, like 35C)



# GUI

GUI in Python for compatibility between macOS and Windows

There are 5 battery modules, 1 to 5; each module has 20 cells, and 18 temperature probes

Cell should be in a 5x4 grid, and temperature should be in 6x3 grid

On the page, there should be 5 5x4 modules for voltage, as well as 5 6x3 for temperature probes

Define 4 global variables for the high low warning threshold for voltage and temperature. If the certain cell or sensor is higher, render it red, otherwise blue, and green for normal 





16 descriptors

48 for data





7 messages per cell voltage cycle 

3 messages for temp cycle

(10 messages per module, 50 per cycle)